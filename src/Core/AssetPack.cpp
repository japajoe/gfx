#include "AssetPack.hpp"
#include <cstring>
#include <iostream>
#include <filesystem>

namespace _gfs = std::filesystem;

namespace GFX
{
    FileBuffer::FileBuffer(std::ifstream &ifs, uint32_t offset, uint32_t size)
    {
        vMemory.resize(size);
        ifs.seekg(offset);
        char *pData = reinterpret_cast<char*>(vMemory.data());
        ifs.read(pData, vMemory.size());
        setg(pData, pData, pData + size);
    }

    AssetPack::AssetPack() 
    {

    }

    AssetPack::~AssetPack() 
    { 
        baseFile.close(); 
    }

    bool AssetPack::AddFile(const std::string &sFile)
    {
        return AddFile(sFile, sFile);
    }

    bool AssetPack::AddFile(const std::string &sFile, const std::string &sFileName)
    {
        const std::string file = MakePosix(sFile);
        const std::string fileName = MakePosix(sFileName);

        if (_gfs::exists(file))
        {
            AssetFile e;
            e.nSize = (uint32_t)_gfs::file_size(file);
            e.nOffset = 0; // Unknown at this stage
            e.sFileName = fileName;
            mapFiles[file] = e;
            return true;
        }
        return false;
    }

    bool AssetPack::Load(const std::string &sFile, const std::string &sKey)
    {
        // Open the resource file
        baseFile.open(sFile, std::ifstream::binary);
        if (!baseFile.is_open())
        {
            printf("Failed to open resource pack\n");
            return false;
        }

        // Read checksum and compare. If the check fails then the provided key was incorrect
        uint16_t checksum = 0;
        baseFile.read((char *)&checksum, sizeof(uint16_t));
        unsigned char *ptrKey = reinterpret_cast<unsigned char*>(const_cast<char*>(sKey.data()));

        uint16_t crc = Checksum(ptrKey, sKey.size());

        if(crc != checksum)
        {
            printf("Checksum error\n");
            baseFile.close();
            return false;
        }

        // 1) Read Scrambled index
        uint32_t nIndexSize = 0;
        baseFile.read((char *)&nIndexSize, sizeof(uint32_t));

        std::vector<char> buffer(nIndexSize);
        for (uint32_t j = 0; j < nIndexSize; j++)
            buffer[j] = baseFile.get();

        std::vector<char> decoded = Scramble(buffer, sKey);
        size_t pos = 0;
        auto read = [&decoded, &pos](char *dst, size_t size) {
            memcpy((void *)dst, (const void *)(decoded.data() + pos), size);
            pos += size;
        };

        auto get = [&read]() -> int { 
            char c; read(&c, 1); return c; 
        };

        // 2) Read Map
        uint32_t nMapEntries = 0;
        read((char *)&nMapEntries, sizeof(uint32_t));

        for (uint32_t i = 0; i < nMapEntries; i++)
        {
            uint32_t nFilePathSize = 0;
            read((char *)&nFilePathSize, sizeof(uint32_t));

            std::string sFileName(nFilePathSize, ' ');
            for (uint32_t j = 0; j < nFilePathSize; j++)
                sFileName[j] = get();

            AssetFile e;
            read((char *)&e.nSize, sizeof(uint32_t));
            read((char *)&e.nOffset, sizeof(uint32_t));
            e.sFileName = sFileName;
            mapFiles[sFileName] = e;
        }

        // Don't close base file! we will provide a stream
        // pointer when the file is requested
        return true;
    }

    bool AssetPack::Save(const std::string &sFile, const std::string &sKey)
    {
        // Create/Overwrite the resource file
        std::ofstream ofs(sFile, std::ofstream::binary);
        if (!ofs.is_open())
            return false;

        // Write checksum
        unsigned char *ptrKey = reinterpret_cast<unsigned char*>(const_cast<char*>(sKey.data()));
        uint16_t crc = Checksum(ptrKey, sKey.size());
        ofs.write((char *)&crc, sizeof(uint16_t));

        // Iterate through map
        uint32_t nIndexSize = 0; // Unknown for now
        ofs.write((char *)&nIndexSize, sizeof(uint32_t));
        uint32_t nMapSize = uint32_t(mapFiles.size());
        ofs.write((char *)&nMapSize, sizeof(uint32_t));
        for (auto &e : mapFiles)
        {
            // Write the path of the file
            size_t nPathSize = e.second.sFileName.size();
            ofs.write((char *)&nPathSize, sizeof(uint32_t));
            ofs.write(e.second.sFileName.c_str(), nPathSize);

            // Write the file entry properties
            ofs.write((char *)&e.second.nSize, sizeof(uint32_t));
            ofs.write((char *)&e.second.nOffset, sizeof(uint32_t));
        }

        // 2) Write the individual Data
        std::streampos offset = ofs.tellp();
        nIndexSize = (uint32_t)offset;
        for (auto &e : mapFiles)
        {
            // Store beginning of file offset within resource pack file
            e.second.nOffset = (uint32_t)offset;

            // Load the file to be added
            std::vector<uint8_t> vBuffer(e.second.nSize);
            std::ifstream i(e.first, std::ifstream::binary);
            i.read((char *)vBuffer.data(), e.second.nSize);
            i.close();

            // Write the loaded file into resource pack file
            ofs.write((char *)vBuffer.data(), e.second.nSize);
            offset += e.second.nSize;
        }

        // 3) Scramble Index
        std::vector<char> stream;
        auto write = [&stream](const char *data, size_t size)
        {
            size_t sizeNow = stream.size();
            stream.resize(sizeNow + size);
            memcpy(stream.data() + sizeNow, data, size);
        };

        // Iterate through map
        write((char *)&nMapSize, sizeof(uint32_t));
        for (auto &e : mapFiles)
        {
            // Write the path of the file
            size_t nPathSize = e.second.sFileName.size();
            write((char *)&nPathSize, sizeof(uint32_t));
            write(e.second.sFileName.c_str(), nPathSize);

            // Write the file entry properties
            write((char *)&e.second.nSize, sizeof(uint32_t));
            write((char *)&e.second.nOffset, sizeof(uint32_t));
        }
        std::vector<char> sIndexString = Scramble(stream, sKey);
        uint32_t nIndexStringLen = uint32_t(sIndexString.size());
        // 4) Rewrite Map (it has been updated with offsets now)
        // at start of file
        //ofs.seekp(0, std::ios::beg);
        ofs.seekp(sizeof(uint16_t), std::ios::beg); //Beginning is after checksum

        ofs.write((char *)&nIndexStringLen, sizeof(uint32_t));
        ofs.write(sIndexString.data(), nIndexStringLen);
        ofs.close();
        return true;
    }

    FileBuffer AssetPack::GetFileBuffer(const std::string &sFile)
    {
        return FileBuffer(baseFile, mapFiles[sFile].nOffset, mapFiles[sFile].nSize);
    }

    std::vector<uint8_t> AssetPack::GetFileData(const std::string &sFile)
    {
        if(mapFiles.count(sFile) > 0)
        {
            auto buffer = FileBuffer(baseFile, mapFiles[sFile].nOffset, mapFiles[sFile].nSize);
            return buffer.vMemory;
        }

        return std::vector<uint8_t>();
    }

    bool AssetPack::Loaded() const
    {
        return baseFile.is_open();
    }

    bool AssetPack::FileExists(const std::string &sFile) const
    {
        return mapFiles.count(sFile) > 0;
    }

    std::vector<char> AssetPack::Scramble(const std::vector<char> &data, const std::string &key)
    {
        if (key.empty())
            return data;
        std::vector<char> o;
        size_t c = 0;
        for (auto s : data)
            o.push_back(s ^ key[(c++) % key.size()]);
        return o;
    };

    std::string AssetPack::MakePosix(const std::string &path)
    {
        std::string o;
        for (auto s : path)
            o += std::string(1, s == '\\' ? '/' : s);
        return o;
    };

    std::map<std::string, AssetFile> &AssetPack::GetFiles()
    {
        return mapFiles;
    }

    uint16_t AssetPack::Checksum(const unsigned char* buf, uint16_t length)
    {
        uint32_t sum = 0;

        sum += length;

        // build the sum of 16bit words
        while (length > 1)
        {
            sum += static_cast<uint32_t>(0xFFFF & (*buf << 8 | *(buf + 1)));
            buf += 2;
            length -= 2;
        }

        // if there is a byte left then add it (padded with zero)
        if (length != 0)
        {
            //--- made by SKA ---                sum += (0xFF & *buf)<<8;
            sum += static_cast<uint32_t>(0xFFFF & (*buf << 8 | 0x00));
        }

        // now calculate the sum over the bytes in the sum
        // until the result is only 16bit long
        while ((sum >> 16) != 0)
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }

        // build 1's complement:
        return static_cast<uint16_t>(sum ^ 0xFFFF);
    }
}