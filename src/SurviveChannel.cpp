#include "SurviveChannel.hpp"

#include <boost/algorithm/string.hpp>

#include <filesystem>
#include <iostream>

SurviveReader::SurviveReader(boost::json::object config)
{
    std::string path(config["path"].as_string());
    std::string textPath = config["textPath"].is_null() ? "" : std::string(config["textPath"].as_string());
    std::size_t offset   = config["offset"].is_null() ? 0 : config["offset"].as_int64();
    expectedEntryCount   = config["entryCount"].is_null() ? 0 : config["entryCount"].as_int64();

    if (!std::filesystem::exists(path)) throw std::runtime_error("Input file does not exist!");

    fileStream = std::ifstream(path, std::ios::in | std::ios::binary);
    fileStream.seekg(offset);

    if (!textPath.empty())
    {
        if (!std::filesystem::exists(textPath)) throw std::runtime_error("Input file does not exist!");

        std::ifstream textStream(textPath, std::ios::in);
        std::string str;
        std::getline(textStream, str);

        boost::algorithm::split(stringList, str, boost::is_any_of(u8","));
    }
}

template<typename T> constexpr T reverseValue(T val)
{
    auto it = reinterpret_cast<uint8_t*>(&val);
    std::reverse(it, it + sizeof(val));
    return val;
}

// Read Functions
int8_t SurviveReader::readInt8()
{
    int8_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return reverseValue(val);
}

int32_t SurviveReader::readInt24()
{
    int32_t val = 0;
    fileStream.read(reinterpret_cast<char*>(&val), 3);

    bool isNegative = val & 0x00000080;
    val             = reverseValue(val & 0xFFFFFF7F) >> 8;
    if (isNegative) val *= -1;

    return val;
}

float SurviveReader::readFloat()
{
    uint32_t val;
    fileStream.read(reinterpret_cast<char*>(&val), 3);
    auto littleVal = reverseValue(val) >> 8;

    bool isNegative = littleVal & 0x800000;
    int32_t shift   = (littleVal & 0x7FFFFF) >> 20;
    float value     = static_cast<float>(littleVal & 0x0FFFFF);

    for (auto i = 0; i < shift; i++)
        value /= 10.0f;

    if (isNegative) value *= -1.0f;

    return value;
}

std::string SurviveReader::readString()
{
    uint16_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    auto littleVal = reverseValue(val);

    return stringList[littleVal];
}

/* not supported */
int16_t SurviveReader::readInt16()
{
    int16_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return reverseValue(val);
}

int32_t SurviveReader::readInt32()
{
    int32_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return reverseValue(val);
}

uint8_t SurviveReader::readUInt8()
{
    uint8_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return reverseValue(val);
}
uint16_t SurviveReader::readUInt16()
{
    uint16_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return reverseValue(val);
}
uint32_t SurviveReader::readUInt24()
{
    uint32_t val;
    fileStream.read(reinterpret_cast<char*>(&val), 3);
    return reverseValue(val) >> 8;
}
uint32_t SurviveReader::readUInt32()
{
    uint32_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return reverseValue(val);
}

uint8_t SurviveReader::readHex8() { throw std::runtime_error("Unimplemented Feature: SurviveReader::readHex8"); }
uint16_t SurviveReader::readHex16() { throw std::runtime_error("Unimplemented Feature: SurviveReader::readHex16"); }
uint32_t SurviveReader::readHex32() { throw std::runtime_error("Unimplemented Feature: SurviveReader::readHex32"); }

double SurviveReader::readDouble() { return readFloat(); }

std::vector<int32_t> SurviveReader::readInt24Array()
{
    std::vector<int32_t> values;
    int32_t count = readInt24();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readInt24());

    return values;
}
std::vector<int32_t> SurviveReader::readInt32Array()
{
    std::vector<int32_t> values;
    int32_t count = readInt32();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readInt32());

    return values;
}
std::vector<uint32_t> SurviveReader::readUInt24Array()
{
    std::vector<uint32_t> values;
    uint32_t count = readInt24();

    for (uint32_t i = 0u; i < count; i++)
        values.push_back(readUInt24());

    return values;
}
std::vector<uint32_t> SurviveReader::readUInt32Array()
{
    std::vector<uint32_t> values;
    int32_t count = readInt32();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readUInt32());

    return values;
}
std::vector<float> SurviveReader::readFloatArray()
{
    std::vector<float> values;
    int32_t count = readInt32();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readFloat());

    return values;
}
std::vector<double> SurviveReader::readDoubleArray()
{
    std::vector<double> values;
    int32_t count = readInt32();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readFloat());

    return values;
}

// Structure Functions
bool SurviveReader::hasNext()
{
    fileStream.peek();
    if (!fileStream || fileStream.eof()) return false;

    if (expectedEntryCount != 0) return currentEntryCount++ < expectedEntryCount;

    return true;
}

/* Survive Writer */
SurviveWriter::SurviveWriter(boost::json::object config)
{
    std::string path(config["path"].as_string());
    std::size_t offset = config["offset"].is_null() ? 0 : config["offset"].as_int64();
    textPath           = config["textPath"].is_null() ? "" : std::string(config["textPath"].as_string());

    fileStream = std::ofstream(path, std::ios::out | std::ios::binary);
    fileStream.seekp(offset);
}

// Write Functions
void SurviveWriter::writeInt8(std::string name, int8_t value)
{
    value = reverseValue(value);
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}

void SurviveWriter::writeInt24(std::string name, int32_t value)
{
    bool isNegative = value < 0;
    value           = std::abs(value);
    if (isNegative) value |= 0x800000;

    value = reverseValue(value) >> 8;
    fileStream.write(reinterpret_cast<char*>(&value), 3);
}

void SurviveWriter::writeFloat(std::string name, float value)
{
    constexpr float epsilon = 0.005f;
    bool isNegative         = value < 0.0f;
    float tmpValue          = std::abs(value);
    uint32_t shift          = 0u;

    for (; shift < 8u; shift++)
    {
        if (std::abs(tmpValue - std::round(tmpValue)) < epsilon) break;
        tmpValue *= 10.0f;
    }

    int32_t intValue    = static_cast<int32_t>(std::round(tmpValue));
    uint32_t finalValue = intValue & 0x0FFFFF;
    finalValue          = finalValue | (shift << 20);
    if (value < 0) finalValue = finalValue | 0x800000;

    finalValue = reverseValue(finalValue) >> 8;

    fileStream.write(reinterpret_cast<char*>(&finalValue), 3);
}

void SurviveWriter::writeString(std::string name, std::string value)
{
    int16_t index = reverseValue(static_cast<int16_t>(stringList.size()));
    stringList.push_back(value);

    fileStream.write(reinterpret_cast<char*>(&index), sizeof(index));
}

/* not supported */
void SurviveWriter::writeInt16(std::string name, int16_t value)
{
    value = reverseValue(value);
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}

void SurviveWriter::writeInt32(std::string name, int32_t value)
{
    value = reverseValue(value);
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}

void SurviveWriter::writeUInt8(std::string name, uint8_t value)
{
    value = reverseValue(value);
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}
void SurviveWriter::writeUInt16(std::string name, uint16_t value)
{
    value = reverseValue(value);
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}
void SurviveWriter::writeUInt24(std::string name, uint32_t value)
{
    value = reverseValue(value) >> 8;
    fileStream.write(reinterpret_cast<char*>(&value), 3);
}
void SurviveWriter::writeUInt32(std::string name, uint32_t value)
{
    value = reverseValue(value);
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}

void SurviveWriter::writeHex8(std::string name, uint8_t value) { throw std::runtime_error("Unimplemented Feature: SurviveReader::writeHex8"); }
void SurviveWriter::writeHex16(std::string name, uint16_t value) { throw std::runtime_error("Unimplemented Feature: SurviveReader::writeHex16"); }
void SurviveWriter::writeHex32(std::string name, uint32_t value) { throw std::runtime_error("Unimplemented Feature: SurviveReader::writeHex32"); }

void SurviveWriter::writeDouble(std::string name, double value) { writeFloat(name, static_cast<float>(value)); }

void SurviveWriter::writeInt24Array(std::string name, std::vector<int32_t> values)
{
    writeInt24(name, (int32_t)values.size());
    for (auto val : values)
        writeInt24(name, val);
}
void SurviveWriter::writeInt32Array(std::string name, std::vector<int32_t> values)
{
    writeInt32(name, (int32_t)values.size());
    for (auto val : values)
        writeInt32(name, val);
}
void SurviveWriter::writeUInt24Array(std::string name, std::vector<uint32_t> values)
{
    writeInt24(name, (int32_t)values.size());
    for (auto val : values)
        writeUInt24(name, val);
}
void SurviveWriter::writeUInt32Array(std::string name, std::vector<uint32_t> values)
{
    writeInt32(name, (int32_t)values.size());
    for (auto val : values)
        writeUInt32(name, val);
}
void SurviveWriter::writeFloatArray(std::string name, std::vector<float> values)
{
    writeInt24(name, (int32_t)values.size());
    for (auto val : values)
        writeFloat(name, val);
}
void SurviveWriter::writeDoubleArray(std::string name, std::vector<double> values)
{
    writeInt24(name, (int32_t)values.size());
    for (auto val : values)
        writeDouble(name, val);
}

// Structure Functions
void SurviveWriter::startFile(boost::json::object structure) {}
void SurviveWriter::startEntry() {}
void SurviveWriter::finishEntry() {}
void SurviveWriter::finishFile()
{
    if (!textPath.empty() && !stringList.empty())
    {
        std::ofstream textStream(textPath, std::ios::out | std::ios::binary);

        for (auto& str : stringList)
            textStream << str << ",";
    }
}