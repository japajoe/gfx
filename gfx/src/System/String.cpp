#include "String.hpp"
#include <sstream>
#include <cctype> // For std::isspace
#include <algorithm> // For std::transform

namespace GFX
{
    std::vector<std::string> String::Split(const std::string &str, char separator)
    {
        std::vector<std::string> substrings;
        std::istringstream ss(str);
        std::string token;

        while (std::getline(ss, token, separator)) 
        {
            if(token.size() > 0)
                substrings.push_back(token);
        }

        return substrings;
    }

    std::string String::Trim(const std::string &str)
    {
        // Find the first non-whitespace character from the beginning
        size_t start = str.find_first_not_of(" \t\n\r\f\v");

        // Find the last non-whitespace character from the end
        size_t end = str.find_last_not_of(" \t\n\r\f\v");

        // Handle the case where the string is all whitespace
        if (start == std::string::npos)
            return "";

        // Extract the substring between start and end
        return str.substr(start, end - start + 1);
    }

    std::string String::TrimStart(const std::string &str)
    {
        size_t start = str.find_first_not_of(" \t\n\r\f\v");

        if (start == std::string::npos)
            return "";

        // Extract the substring starting from the first non-whitespace character
        return str.substr(start);
    }

    std::string String::TrimEnd(const std::string &str)
    {
        size_t end = str.find_last_not_of(" \t\n\r\f\v");

        if (end == std::string::npos)
            return "";

        // Extract the substring from the beginning to the last non-whitespace character
        return str.substr(0, end + 1);
    }

    bool String::StartsWith(const std::string &str, const std::string &suffix)
    {
        if (str.length() >= suffix.length()) 
            return (0 == str.compare(0, suffix.length(), suffix));
        return false;
    }

    bool String::EndsWith(const std::string &str, const std::string &suffix)
    {
        if (str.length() >= suffix.length()) 
            return (0 == str.compare(str.length() - suffix.length(), suffix.length(), suffix));
        return false;
    }

    bool String::Contains(const std::string &haystack, const std::string &needle)
    {
        return haystack.find(needle) != std::string::npos;
    }

    void String::Replace(std::string &str, const std::string &target, const std::string &replacement)
    {
        size_t startPos = 0;

        while ((startPos = str.find(target, startPos)) != std::string::npos)
        {
            str.replace(startPos, target.length(), replacement);
            startPos += replacement.length();
        }
    }

    std::string String::ToLower(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    std::string String::ToUpper(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::string String::SubString(const std::string &str, size_t startIndex)
    {
        return str.substr(startIndex);
    }

    std::string String::SubString(const std::string &str, size_t startIndex, size_t length)
    {
        return str.substr(startIndex, length);
    }

    int64_t String::IndexOf(const std::string &str, const std::string &subStr)
    {
        size_t pos = str.find(subStr);
        if (pos != std::string::npos) 
            return static_cast<int64_t>(pos);
        return -1; // Indicates substring not found
    }

    bool String::TryParseInt8(const std::string &str, int8_t &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseUInt8(const std::string &str, uint8_t &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseInt16(const std::string &str, int16_t &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseUInt16(const std::string &str, uint16_t &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseInt32(const std::string &str, int32_t &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseUInt32(const std::string &str, uint32_t &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseInt64(const std::string &str, int64_t &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseUInt64(const std::string &str, uint64_t &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseFloat(const std::string &str, float &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }

    bool String::TryParseDouble(const std::string &str, double &value)
    {
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
    }
}