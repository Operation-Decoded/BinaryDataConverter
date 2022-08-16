#include "CSVChannel.hpp"

#include <boost/algorithm/string.hpp>

#include <filesystem>
#include <iostream>

CSVReader::CSVReader(boost::json::object config)
{
    std::string path(config["path"].as_string());

    if (!std::filesystem::exists(path)) throw std::runtime_error("Input file does not exist!");

    fileStream = std::make_unique<std::ifstream>(path, std::ios::in);
    parser     = std::make_unique<aria::csv::CsvParser>(*fileStream.get());
    parser->begin(); // skip header
}

CSVWriter::CSVWriter(boost::json::object config)
{
    std::string path(config["path"].as_string());
    fileStream = std::ofstream(path, std::ios::out);
}

int8_t CSVReader::readInt8()
{
    std::string string = read();
    return (int8_t)std::stol(string);
}
int16_t CSVReader::readInt16()
{
    std::string string = read();
    return (int16_t)std::stol(string);
}
int32_t CSVReader::readInt24()
{
    std::string string = read();
    return (int32_t)((std::stol(string) << 8) >> 8);
}
int32_t CSVReader::readInt32()
{
    std::string string = read();
    return (int32_t)std::stol(string);
}

uint8_t CSVReader::readUInt8()
{
    std::string string = read();
    return (int8_t)std::stoul(string);
}
uint16_t CSVReader::readUInt16()
{
    std::string string = read();
    return (uint16_t)std::stoul(string);
}
uint32_t CSVReader::readUInt24()
{
    std::string string = read();
    return (uint32_t)((std::stoul(string) << 8) >> 8);
}
uint32_t CSVReader::readUInt32()
{
    std::string string = read();
    return (uint32_t)std::stoul(string);
}

float CSVReader::readFloat()
{
    std::string string = read();
    return std::stof(string);
};
double CSVReader::readDouble()
{
    std::string string = read();
    return std::stod(string);
};

std::string CSVReader::readString() { return read(); };

std::vector<int32_t> CSVReader::readInt24Array()
{
    std::vector<std::string> entries;
    std::vector<int32_t> output;

    boost::split(entries, read(), boost::is_any_of(" "));
    std::transform(entries.begin(),
                   entries.end(),
                   output.begin(),
                   [&](std::string& ref) { return (std::stol(ref) << 8) >> 8; });

    return output;
};
std::vector<int32_t> CSVReader::readInt32Array()
{
    std::vector<std::string> entries;
    std::vector<int32_t> output;

    boost::split(entries, read(), boost::is_any_of(" "));
    std::transform(entries.begin(), entries.end(), output.begin(), [&](std::string& ref) { return std::stol(ref); });

    return output;
};
std::vector<uint32_t> CSVReader::readUInt24Array()
{
    std::vector<std::string> entries;
    std::vector<uint32_t> output;

    boost::split(entries, read(), boost::is_any_of(" "));
    std::transform(entries.begin(),
                   entries.end(),
                   output.begin(),
                   [&](std::string& ref) { return (std::stoul(ref) << 8) >> 8; });

    return output;
};
std::vector<uint32_t> CSVReader::readUInt32Array()
{
    std::vector<std::string> entries;
    std::vector<uint32_t> output;

    boost::split(entries, read(), boost::is_any_of(" "));
    std::transform(entries.begin(), entries.end(), output.begin(), [&](std::string& ref) { return std::stoul(ref); });

    return output;
};

std::vector<float> CSVReader::readFloatArray()
{
    std::vector<std::string> entries;
    std::vector<float> output;

    boost::split(entries, read(), boost::is_any_of(" "));
    std::transform(entries.begin(), entries.end(), output.begin(), [&](std::string ref) { return std::stof(ref); });

    return output;
};
std::vector<double> CSVReader::readDoubleArray()
{
    std::vector<std::string> entries;
    std::vector<double> output;

    boost::split(entries, read(), boost::is_any_of(" "));
    std::transform(entries.begin(), entries.end(), output.begin(), [&](std::string ref) { return std::stod(ref); });

    return output;
};

bool CSVReader::hasNext()
{
    static int count = 0;
    count++;
    bool isEmpty  = parser->empty();
    currentRow    = *parser->begin();
    currentColumn = 0;
    return !isEmpty;
}

std::string CSVReader::read() { return currentRow[currentColumn++]; }

// Write Functions
void CSVWriter::writeInt8(std::string name, int8_t value) { write((int32_t)value); }
void CSVWriter::writeInt16(std::string name, int16_t value) { write(value); }
void CSVWriter::writeInt24(std::string name, int32_t value) { write(value); }
void CSVWriter::writeInt32(std::string name, int32_t value) { write(value); }

void CSVWriter::writeUInt8(std::string name, uint8_t value) { write((uint32_t)value); }
void CSVWriter::writeUInt16(std::string name, uint16_t value) { write(value); }
void CSVWriter::writeUInt24(std::string name, uint32_t value) { write(value); }
void CSVWriter::writeUInt32(std::string name, uint32_t value) { write(value); }

void CSVWriter::writeFloat(std::string name, float value) { write(value); }
void CSVWriter::writeDouble(std::string name, double value) { write(value); }
void CSVWriter::writeString(std::string name, std::string value) { write(std::quoted(value, '\"', '\"')); }

void CSVWriter::writeInt24Array(std::string name, std::vector<int32_t> values)
{
    std::vector<std::string> strings;
    std::transform(values.begin(), values.end(), strings.begin(), [=](int32_t val) { return std::to_string(val); });
    write(boost::algorithm::join(strings, " "));
}
void CSVWriter::writeInt32Array(std::string name, std::vector<int32_t> values)
{
    std::vector<std::string> strings;
    std::transform(values.begin(), values.end(), strings.begin(), [=](int32_t val) { return std::to_string(val); });
    write(boost::algorithm::join(strings, " "));
}
void CSVWriter::writeUInt24Array(std::string name, std::vector<uint32_t> values)
{
    std::vector<std::string> strings;
    std::transform(values.begin(), values.end(), strings.begin(), [=](uint32_t val) { return std::to_string(val); });
    write(boost::algorithm::join(strings, " "));
}
void CSVWriter::writeUInt32Array(std::string name, std::vector<uint32_t> values)
{
    std::vector<std::string> strings;
    std::transform(values.begin(), values.end(), strings.begin(), [=](uint32_t val) { return std::to_string(val); });
    write(boost::algorithm::join(strings, " "));
}
void CSVWriter::writeFloatArray(std::string name, std::vector<float> values)
{
    std::vector<std::string> strings;
    std::transform(values.begin(), values.end(), strings.begin(), [=](float val) { return std::to_string(val); });
    write(boost::algorithm::join(strings, " "));
}
void CSVWriter::writeDoubleArray(std::string name, std::vector<double> values)
{
    std::vector<std::string> strings;
    std::transform(values.begin(), values.end(), strings.begin(), [=](double val) { return std::to_string(val); });
    write(boost::algorithm::join(strings, " "));
}

// Structure Functions
void CSVWriter::startFile(boost::json::object structure)
{
    startEntry();
    for (auto entry : structure)
        write(entry.key());
    finishEntry();
}
void CSVWriter::startEntry() {}
void CSVWriter::finishEntry()
{
    isFirst = true;
    fileStream << std::endl;
}
void CSVWriter::finishFile() {}

template<typename T> void CSVWriter::write(T value)
{
    if (!isFirst)
        fileStream << ",";
    else
        isFirst = false;

    fileStream << value;
}