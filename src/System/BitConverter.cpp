#include "BitConverter.hpp"

namespace GFX
{
    bool BitConverter::IsLittleEndian()
    {
        uint32_t i = 1;
        unsigned char* c = reinterpret_cast<unsigned char*>(&i);
        return *c > 0;
    }

	void BitConverter::ConvertToByteOrder(unsigned char *bytes, int length, ByteOrder byteOrder)
	{
		if(byteOrder == ByteOrder::LittleEndian)
		{
			if(!IsLittleEndian())
				FlipBytes(bytes, length);
		}
		else
		{
			if(IsLittleEndian())
				FlipBytes(bytes, length);
		}
	}

    void BitConverter::GetBytes(int64_t value, unsigned char * buffer, int offset, ByteOrder byteOrder)
    {
        std::memcpy(&buffer[offset], &value, sizeof(int64_t));
		ConvertToByteOrder(&buffer[offset], sizeof(int64_t), byteOrder);
    }

    void BitConverter::GetBytes(uint64_t value, unsigned char * buffer, int offset, ByteOrder byteOrder)
    {
        std::memcpy(&buffer[offset], &value, sizeof(uint64_t));
		ConvertToByteOrder(&buffer[offset], sizeof(uint64_t), byteOrder);
    }

    void BitConverter::GetBytes(int32_t value, unsigned char * buffer, int offset, ByteOrder byteOrder)
    {
        std::memcpy(&buffer[offset], &value, sizeof(int32_t));
		ConvertToByteOrder(&buffer[offset], sizeof(int32_t), byteOrder);
    }        

    void BitConverter::GetBytes(uint32_t value, unsigned char * buffer, int offset, ByteOrder byteOrder)
    {
        std::memcpy(&buffer[offset], &value, sizeof(uint32_t));
		ConvertToByteOrder(&buffer[offset], sizeof(uint32_t), byteOrder);
    }

    void BitConverter::GetBytes(int16_t value, unsigned char * buffer, int offset, ByteOrder byteOrder)
    {
        std::memcpy(&buffer[offset], &value, sizeof(int16_t));
		ConvertToByteOrder(&buffer[offset], sizeof(int16_t), byteOrder);
    }

    void BitConverter::GetBytes(uint16_t value, unsigned char * buffer, int offset, ByteOrder byteOrder)
    {
        std::memcpy(&buffer[offset], &value, sizeof(uint16_t));
		ConvertToByteOrder(&buffer[offset], sizeof(uint16_t), byteOrder);
    }

    void BitConverter::GetBytes(double value, unsigned char * buffer, int offset, ByteOrder byteOrder)
    {
        std::memcpy(&buffer[offset], &value, sizeof(double));
		ConvertToByteOrder(&buffer[offset], sizeof(double), byteOrder);
    }

    void BitConverter::GetBytes(float value, unsigned char * buffer, int offset, ByteOrder byteOrder)
    {
        std::memcpy(&buffer[offset], &value, sizeof(float));
		ConvertToByteOrder(&buffer[offset], sizeof(float), byteOrder);
    }

    int BitConverter::GetBytes(const std::string &value, unsigned char * buffer, int offset)
    {
        char *chars = const_cast<char*>(value.c_str());
        std::memcpy(&buffer[offset], chars, value.length());
        return value.length();
    }

    int BitConverter::GetBytes(const std::string &value, size_t size, unsigned char * buffer, int offset)
    {
        char *chars = const_cast<char*>(value.c_str());
        std::memcpy(&buffer[offset], chars, size);
        return size;
    }

    int64_t BitConverter::ToInt64(unsigned char *bytes, int offset, ByteOrder byteOrder)
    {
		return ConvertTo<uint64_t>(bytes, offset, byteOrder);
    }

    uint64_t BitConverter::ToUInt64(unsigned char *bytes, int offset, ByteOrder byteOrder)
    {
		return ConvertTo<uint64_t>(bytes, offset, byteOrder);
    }

    int32_t BitConverter::ToInt32(unsigned char *bytes, int offset, ByteOrder byteOrder)
    {
        return ConvertTo<int32_t>(bytes, offset, byteOrder);
    }

    uint32_t BitConverter::ToUInt32(unsigned char *bytes, int offset, ByteOrder byteOrder)
    {
        return ConvertTo<uint32_t>(bytes, offset, byteOrder);
    }

    int16_t BitConverter::ToInt16(unsigned char *bytes, int offset, ByteOrder byteOrder)
    {
        return ConvertTo<int16_t>(bytes, offset, byteOrder);
    }

    uint16_t BitConverter::ToUInt16(unsigned char *bytes, int offset, ByteOrder byteOrder)
    {
        return ConvertTo<uint16_t>(bytes, offset, byteOrder);
    }

    float BitConverter::ToSingle(unsigned char *bytes, int offset, ByteOrder byteOrder)
    {
        return ConvertTo<float>(bytes, offset, byteOrder);
    }

    double BitConverter::ToDouble(unsigned char *bytes, int offset, ByteOrder byteOrder)
    {
        return ConvertTo<double>(bytes, offset, byteOrder);
    }

    std::string BitConverter::ToString(unsigned char *bytes, int offset, int length)
    {
        char *characters = reinterpret_cast<char*>(&bytes[offset]);
        std::string result(characters, length);
        return result;
    }

    void BitConverter::FlipBytes(unsigned char *bytes, int offset, int length)
    {
        FlipBytes(&bytes[offset], length);
    }

    void BitConverter::FlipBytes(unsigned char* bytes, int length)
    {
        for (int i = 0; i < length / 2; ++i) 
        {
            unsigned char t = bytes[i];
            bytes[i] = bytes[length - i - 1];
            bytes[length - i - 1] = t;
        }
    }

    int BitConverter::GetByteCount(const std::string &value)
    {
        return value.length();
    }
}