#pragma once

#include "Channel.hpp"

#include <fstream>

class SurviveReader : public Reader
{
    std::vector<std::string> stringList;
    std::ifstream fileStream;
    std::size_t expectedEntryCount = 0;
    std::size_t currentEntryCount  = 0;

public:
    SurviveReader(boost::json::object config);

    // Read Functions
    virtual int8_t readInt8();
    virtual int16_t readInt16();
    virtual int32_t readInt24();
    virtual int32_t readInt32();

    virtual uint8_t readUInt8();
    virtual uint16_t readUInt16();
    virtual uint32_t readUInt24();
    virtual uint32_t readUInt32();

    virtual float readFloat();
    virtual double readDouble();

    virtual std::string readString();

    virtual std::vector<int32_t> readInt24Array();
    virtual std::vector<int32_t> readInt32Array();
    virtual std::vector<uint32_t> readUInt24Array();
    virtual std::vector<uint32_t> readUInt32Array();
    virtual std::vector<float> readFloatArray();
    virtual std::vector<double> readDoubleArray();

    virtual bool hasNext();
};

class SurviveWriter : public Writer
{
    std::vector<std::string> stringList{};
    std::ofstream fileStream;
    std::string textPath;

public:
    SurviveWriter(boost::json::object config);

    // Write Functions
    virtual void writeInt8(std::string name, int8_t value);
    virtual void writeInt16(std::string name, int16_t value);
    virtual void writeInt24(std::string name, int32_t value);
    virtual void writeInt32(std::string name, int32_t value);

    virtual void writeUInt8(std::string name, uint8_t value);
    virtual void writeUInt16(std::string name, uint16_t value);
    virtual void writeUInt24(std::string name, uint32_t value);
    virtual void writeUInt32(std::string name, uint32_t value);

    virtual void writeFloat(std::string name, float value);
    virtual void writeDouble(std::string name, double value);
    virtual void writeString(std::string name, std::string value);

    virtual void writeInt24Array(std::string name, std::vector<int32_t> values);
    virtual void writeInt32Array(std::string name, std::vector<int32_t> values);
    virtual void writeUInt24Array(std::string name, std::vector<uint32_t> values);
    virtual void writeUInt32Array(std::string name, std::vector<uint32_t> values);
    virtual void writeFloatArray(std::string name, std::vector<float> values);
    virtual void writeDoubleArray(std::string name, std::vector<double> values);

    // Structure Functions
    virtual void startFile(boost::json::object structure);
    virtual void startEntry();
    virtual void finishEntry();
    virtual void finishFile();
};