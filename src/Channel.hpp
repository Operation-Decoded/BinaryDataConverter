#pragma once

#include <boost/json.hpp>

#include <string>
#include <vector>

class Reader
{
public:
    // Read Functions
    virtual int8_t readInt8()   = 0;
    virtual int16_t readInt16() = 0;
    virtual int32_t readInt24() = 0;
    virtual int32_t readInt32() = 0;

    virtual uint8_t readUInt8()   = 0;
    virtual uint16_t readUInt16() = 0;
    virtual uint32_t readUInt24() = 0;
    virtual uint32_t readUInt32() = 0;

    virtual float readFloat()   = 0;
    virtual double readDouble() = 0;

    virtual std::string readString() = 0;

    virtual std::vector<int32_t> readInt24Array()   = 0;
    virtual std::vector<int32_t> readInt32Array()   = 0;
    virtual std::vector<uint32_t> readUInt24Array() = 0;
    virtual std::vector<uint32_t> readUInt32Array() = 0;
    virtual std::vector<float> readFloatArray()     = 0;
    virtual std::vector<double> readDoubleArray()   = 0;

    virtual bool hasNext() = 0;
};

class Writer
{
public:
    // Write Functions
    virtual void writeInt8(std::string name, int8_t value)   = 0;
    virtual void writeInt16(std::string name, int16_t value) = 0;
    virtual void writeInt24(std::string name, int32_t value) = 0;
    virtual void writeInt32(std::string name, int32_t value) = 0;

    virtual void writeUInt8(std::string name, uint8_t value)   = 0;
    virtual void writeUInt16(std::string name, uint16_t value) = 0;
    virtual void writeUInt24(std::string name, uint32_t value) = 0;
    virtual void writeUInt32(std::string name, uint32_t value) = 0;

    virtual void writeFloat(std::string name, float value)        = 0;
    virtual void writeDouble(std::string name, double value)      = 0;
    virtual void writeString(std::string name, std::string value) = 0;

    virtual void writeInt24Array(std::string name, std::vector<int32_t> values)   = 0;
    virtual void writeInt32Array(std::string name, std::vector<int32_t> values)   = 0;
    virtual void writeUInt24Array(std::string name, std::vector<uint32_t> values) = 0;
    virtual void writeUInt32Array(std::string name, std::vector<uint32_t> values) = 0;
    virtual void writeFloatArray(std::string name, std::vector<float> values)     = 0;
    virtual void writeDoubleArray(std::string name, std::vector<double> values)   = 0;

    // Structure Functions
    virtual void startFile(boost::json::object structure) = 0;
    virtual void startEntry()                             = 0;
    virtual void finishEntry()                            = 0;
    virtual void finishFile()                             = 0;
};