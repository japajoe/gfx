#ifndef GFX_ASSETPACK_HPP
#define GFX_ASSETPACK_HPP

#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <fstream>

namespace GFX
{
    struct FileBuffer : public std::streambuf
    {
        FileBuffer(std::ifstream &ifs, uint32_t offset, uint32_t size);
        std::vector<uint8_t> vMemory;
    };

    struct AssetFile
    {
        uint32_t nSize;
        uint32_t nOffset;
        std::string sFileName;
    };

    class AssetPack : public std::streambuf
    {
    public:
        AssetPack();
        ~AssetPack();
        bool AddFile(const std::string &sFile);
        bool AddFile(const std::string &sFile, const std::string &sFileName);
        bool Load(const std::string &sFile, const std::string &sKey);
        bool Save(const std::string &sFile, const std::string &sKey);
        FileBuffer GetFileBuffer(const std::string &sFile);
        std::vector<uint8_t> GetFileData(const std::string &sFile);
        bool Loaded() const;
        std::map<std::string, AssetFile> &GetFiles();
        bool FileExists(const std::string &sFile) const;
    private:
        std::map<std::string, AssetFile> mapFiles;
        std::ifstream baseFile;
        std::vector<char> Scramble(const std::vector<char> &data, const std::string &key);
        std::string MakePosix(const std::string &path);
        uint16_t Checksum(const unsigned char* buf, uint16_t length);
    };
}

#endif