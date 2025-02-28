#include "AudioClip.hpp"

namespace GFX
{
    AudioClip::AudioClip()
    {
        this->handle = nullptr;
        this->dataSize = 0;
        this->streamFromDisk = false;
    }

    AudioClip::AudioClip(const std::string &filepath, bool streamFromDisk)
    {
        this->filePath = filepath;
        this->handle = nullptr;
        this->dataSize = 0;
        this->streamFromDisk = streamFromDisk;
    }

    AudioClip::AudioClip(const std::vector<uint8_t> &data)
    {
        this->data = data;
        this->handle = reinterpret_cast<void*>(const_cast<uint8_t*>(this->data.data()));
        this->dataSize = this->data.size();
        this->streamFromDisk = false;
    }

    AudioClip::AudioClip(void *data, size_t size)
    {
        this->handle = data;
        this->dataSize = size;
        this->streamFromDisk = false;
    }

    std::string AudioClip::GetFilePath() const
    {
        return filePath;
    }

    void *AudioClip::GetHandle() const
    {
        return handle;
    }

    size_t AudioClip::GetDataSize() const
    {
        return dataSize;
    }

    bool AudioClip::GetStreamFromDisk() const
    {
        return streamFromDisk;
    }

    void AudioClip::SetName(const std::string &name)
    {
        this->name = name;
    }
    
    std::string AudioClip::GetName() const
    {
        return name;
    }
};