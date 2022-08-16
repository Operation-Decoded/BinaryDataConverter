#pragma once

#include "Channel.hpp"

class ReadWriter
{
public:
    virtual void convert(std::string name, std::shared_ptr<Reader> inChannel, std::shared_ptr<Writer> outChannel) = 0;
};

template<typename T> class ReadWriteTuple : public ReadWriter
{
    using ReadFunction  = T (Reader::*)();
    using WriteFunction = void (Writer::*)(std::string, T);

    ReadFunction reader;
    WriteFunction writer;

public:
    ReadWriteTuple(ReadFunction reader, WriteFunction writer)
        : reader(reader)
        , writer(writer)
    {
    }

    virtual void
    convert(std::string name, std::shared_ptr<Reader> inChannel, std::shared_ptr<Writer> outChannel) override;
};

template<typename T>
void ReadWriteTuple<T>::convert(std::string name, std::shared_ptr<Reader> inChannel, std::shared_ptr<Writer> outChannel)
{
    T val = ((*inChannel).*reader)();
    ((*outChannel).*writer)(name, val);
}
