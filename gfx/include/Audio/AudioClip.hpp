#ifndef GFX_AUDIOCLIP_HPP
#define GFX_AUDIOCLIP_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace GFX
{
    class AudioClip
    {
    public:
        AudioClip();
        AudioClip(const std::string &filepath, bool streamFromDisk = true);
        AudioClip(const std::vector<uint8_t> &data);
        AudioClip(void *data, size_t size);
        std::string GetFilePath() const;
        void *GetHandle() const;
        size_t GetDataSize() const;
        bool GetStreamFromDisk() const;
        void SetName(const std::string &name);
        std::string GetName() const;
    private:
        std::string filePath;
        std::string name;
        std::vector<uint8_t> data;
        size_t dataSize;
        void *handle;
        bool streamFromDisk;
    };
};

#endif