#include "BinaryChannel.hpp"

#include <filesystem>

/* Binary Reader  */
BinaryReader::BinaryReader(boost::json::object config)
{
    std::string path(config["path"].as_string());
    std::size_t offset = config["offset"].is_null() ? 0 : config["offset"].as_int64();
    expectedEntryCount = config["entryCount"].is_null() ? 0 : config["entryCount"].as_int64();

    if (!std::filesystem::exists(path)) throw std::runtime_error("Input file does not exist!");

    fileStream = std::ifstream(path, std::ios::in | std::ios::binary);
    fileStream.seekg(offset);
}

// Read Functions
int8_t BinaryReader::readInt8()
{
    int8_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}
int16_t BinaryReader::readInt16()
{
    int16_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}
int32_t BinaryReader::readInt24()
{
    int32_t val;
    fileStream.read(reinterpret_cast<char*>(&val), 3);
    return (val << 8) >> 8;
}
int32_t BinaryReader::readInt32()
{
    int32_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}

uint8_t BinaryReader::readUInt8()
{
    uint8_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}
uint16_t BinaryReader::readUInt16()
{
    uint16_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}
uint32_t BinaryReader::readUInt24()
{
    uint32_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return (val << 8) >> 8;
}
uint32_t BinaryReader::readUInt32()
{
    uint32_t val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}

float BinaryReader::readFloat()
{
    float val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}
double BinaryReader::readDouble()
{
    double val;
    fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return val;
}

std::string BinaryReader::readString()
{
    std::stringstream sstream;
    char val;

    do
    {
        fileStream.read(reinterpret_cast<char*>(&val), sizeof(val));
        sstream << val;
    } while (val != '\0');

    return sstream.str();
}

std::vector<int32_t> BinaryReader::readInt24Array()
{
    std::vector<int32_t> values;
    int32_t count = readInt24();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readInt24());

    return values;
}
std::vector<int32_t> BinaryReader::readInt32Array()
{
    std::vector<int32_t> values;
    int32_t count = readInt32();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readInt32());

    return values;
}
std::vector<uint32_t> BinaryReader::readUInt24Array()
{
    std::vector<uint32_t> values;
    uint32_t count = readInt24();

    for (uint32_t i = 0u; i < count; i++)
        values.push_back(readUInt24());

    return values;
}
std::vector<uint32_t> BinaryReader::readUInt32Array()
{
    std::vector<uint32_t> values;
    int32_t count = readInt32();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readUInt32());

    return values;
}
std::vector<float> BinaryReader::readFloatArray()
{
    std::vector<float> values;
    int32_t count = readInt32();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readFloat());

    return values;
}
std::vector<double> BinaryReader::readDoubleArray()
{
    std::vector<double> values;
    int32_t count = readInt32();

    for (int32_t i = 0; i < count; i++)
        values.push_back(readFloat());

    return values;
}

bool BinaryReader::hasNext()
{
    if (!fileStream) return false;

    if (expectedEntryCount != 0) return currentEntryCount++ < expectedEntryCount;

    return true;
}

/* Binary Writer */
BinaryWriter::BinaryWriter(boost::json::object config)
{
    std::string path(config["path"].as_string());
    std::size_t offset = config["offset"].is_null() ? 0 : config["offset"].as_int64();

    fileStream = std::ofstream(path, std::ios::out | std::ios::binary);
    fileStream.seekp(offset);
}

// Write Functions
void BinaryWriter::writeInt8(std::string name, int8_t value)
{
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}
void BinaryWriter::writeInt16(std::string name, int16_t value)
{
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}
void BinaryWriter::writeInt24(std::string name, int32_t value) { fileStream.write(reinterpret_cast<char*>(&value), 3); }
void BinaryWriter::writeInt32(std::string name, int32_t value)
{
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}

void BinaryWriter::writeUInt8(std::string name, uint8_t value)
{
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}
void BinaryWriter::writeUInt16(std::string name, uint16_t value)
{
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}
void BinaryWriter::writeUInt24(std::string name, uint32_t value)
{
    fileStream.write(reinterpret_cast<char*>(&value), 3);
}
void BinaryWriter::writeUInt32(std::string name, uint32_t value)
{
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}

void BinaryWriter::writeFloat(std::string name, float value)
{
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}
void BinaryWriter::writeDouble(std::string name, double value)
{
    fileStream.write(reinterpret_cast<char*>(&value), sizeof(value));
}
void BinaryWriter::writeString(std::string name, std::string value)
{
    fileStream.write(reinterpret_cast<char*>(&value), value.length());
}

void BinaryWriter::writeInt24Array(std::string name, std::vector<int32_t> values)
{
    writeInt24(name, (int32_t)values.size());
    for (auto val : values)
        writeInt24(name, val);
}
void BinaryWriter::writeInt32Array(std::string name, std::vector<int32_t> values)
{
    writeInt32(name, (int32_t)values.size());
    for (auto val : values)
        writeInt32(name, val);
}
void BinaryWriter::writeUInt24Array(std::string name, std::vector<uint32_t> values)
{
    writeInt24(name, (int32_t)values.size());
    for (auto val : values)
        writeUInt24(name, val);
}
void BinaryWriter::writeUInt32Array(std::string name, std::vector<uint32_t> values)
{
    writeInt32(name, (int32_t)values.size());
    for (auto val : values)
        writeUInt32(name, val);
}
void BinaryWriter::writeFloatArray(std::string name, std::vector<float> values)
{
    writeInt32(name, (int32_t)values.size());
    for (auto val : values)
        writeFloat(name, val);
}
void BinaryWriter::writeDoubleArray(std::string name, std::vector<double> values)
{
    writeInt32(name, (int32_t)values.size());
    for (auto val : values)
        writeDouble(name, val);
}

// Structure Functions
void BinaryWriter::startFile(boost::json::object structure) {}
void BinaryWriter::startEntry() {}
void BinaryWriter::finishEntry() {}
void BinaryWriter::finishFile() {}