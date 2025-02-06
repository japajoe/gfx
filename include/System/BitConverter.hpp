#ifndef GFX_BITCONVERTER_HPP
#define GFX_BITCONVERTER_HPP

#include <cstdint>
#include <string>
#include <cstring>
#include <type_traits>

namespace GFX
{
    enum class ByteOrder
    {
        LittleEndian,
        BigEndian
    };

    class BitConverter
    {
	private:
		static void ConvertToByteOrder(unsigned char *bytes, int length, ByteOrder byteOrder);
		template<typename T>
		static typename std::enable_if<std::is_arithmetic<T>::value, T>::type
		ConvertTo(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian) 
		{
			unsigned char buffer[8];
			std::memcpy(buffer, &bytes[offset], sizeof(T));
			ConvertToByteOrder(buffer, sizeof(T), byteOrder);
			return *(T*)buffer;
		}
    public:
        static bool IsLittleEndian();
        static void GetBytes(int64_t value, unsigned char * buffer, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static void GetBytes(uint64_t value, unsigned char * buffer, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static void GetBytes(int32_t value, unsigned char * buffer, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static void GetBytes(uint32_t value, unsigned char * buffer, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static void GetBytes(int16_t value, unsigned char * buffer, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static void GetBytes(uint16_t value, unsigned char * buffer, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static void GetBytes(double value, unsigned char * buffer, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static void GetBytes(float value, unsigned char * buffer, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static int GetBytes(const std::string &value, unsigned char * buffer, int offset);
        static int GetBytes(const std::string &value, size_t size, unsigned char * buffer, int offset);
        static int64_t ToInt64(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static uint64_t ToUInt64(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static int32_t ToInt32(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static uint32_t ToUInt32(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static int16_t ToInt16(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static uint16_t ToUInt16(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static float ToSingle(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static double ToDouble(unsigned char *bytes, int offset, ByteOrder byteOrder = ByteOrder::LittleEndian);
        static std::string ToString(unsigned char *bytes, int offset, int length);
        static void FlipBytes(unsigned char *bytes, int offset, int length);
        static void FlipBytes(unsigned char* bytes, int length);
        static int GetByteCount(const std::string &value);
    };
}

#endif