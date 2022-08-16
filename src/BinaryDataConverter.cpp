#include "BinaryDataConverter.hpp"

#include "BinaryChannel.hpp"
#include "CSVChannel.hpp"
#include "Channel.hpp"
#include "ReadWriter.hpp"
#include "SurviveChannel.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <parser.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

using ReadWriterMap = std::map<std::string, std::shared_ptr<ReadWriter>>;
ReadWriterMap registerReadWriter();

ReadWriterMap readWriter = registerReadWriter();

std::unique_ptr<Reader> readerFactory(boost::json::object config)
{
    std::string format(config["format"].as_string());
    boost::algorithm::to_lower(format);

    if (format.compare("binary") == 0) return std::make_unique<BinaryReader>(config);
    if (format.compare("csv") == 0) return std::make_unique<CSVReader>(config);
    if (format.compare("survivebinary") == 0) return std::make_unique<SurviveReader>(config);

    return nullptr;
}

std::unique_ptr<Writer> writerFactory(boost::json::object config)
{
    std::string format(config["format"].as_string());
    boost::algorithm::to_lower(format);

    if (format.compare("binary") == 0) return std::make_unique<BinaryWriter>(config);
    if (format.compare("csv") == 0) return std::make_unique<CSVWriter>(config);
    if (format.compare("survivebinary") == 0) return std::make_unique<SurviveWriter>(config);

    return nullptr;
}

ReadWriterMap registerReadWriter()
{
    ReadWriterMap map;
    map["int8"]  = std::make_shared<ReadWriteTuple<int8_t>>(&Reader::readInt8, &Writer::writeInt8);
    map["int16"] = std::make_shared<ReadWriteTuple<int16_t>>(&Reader::readInt16, &Writer::writeInt16);
    map["int24"] = std::make_shared<ReadWriteTuple<int32_t>>(&Reader::readInt24, &Writer::writeInt24);
    map["int32"] = std::make_shared<ReadWriteTuple<int32_t>>(&Reader::readInt32, &Writer::writeInt32);

    map["uint8"]  = std::make_shared<ReadWriteTuple<uint8_t>>(&Reader::readUInt8, &Writer::writeUInt8);
    map["uint16"] = std::make_shared<ReadWriteTuple<uint16_t>>(&Reader::readUInt16, &Writer::writeUInt16);
    map["uint24"] = std::make_shared<ReadWriteTuple<uint32_t>>(&Reader::readUInt24, &Writer::writeUInt24);
    map["uint32"] = std::make_shared<ReadWriteTuple<uint32_t>>(&Reader::readUInt32, &Writer::writeUInt32);

    map["float"]  = std::make_shared<ReadWriteTuple<float>>(&Reader::readFloat, &Writer::writeFloat);
    map["double"] = std::make_shared<ReadWriteTuple<double>>(&Reader::readDouble, &Writer::writeDouble);
    map["string"] = std::make_shared<ReadWriteTuple<std::string>>(&Reader::readString, &Writer::writeString);

    map["int24array"] =
        std::make_shared<ReadWriteTuple<std::vector<int32_t>>>(&Reader::readInt24Array, &Writer::writeInt24Array);
    map["int32array"] =
        std::make_shared<ReadWriteTuple<std::vector<int32_t>>>(&Reader::readInt32Array, &Writer::writeInt32Array);
    map["uint24array"] =
        std::make_shared<ReadWriteTuple<std::vector<uint32_t>>>(&Reader::readUInt24Array, &Writer::writeUInt24Array);
    map["uint32array"] =
        std::make_shared<ReadWriteTuple<std::vector<uint32_t>>>(&Reader::readUInt32Array, &Writer::writeUInt32Array);

    map["floatarray"] =
        std::make_shared<ReadWriteTuple<std::vector<float>>>(&Reader::readFloatArray, &Writer::writeFloatArray);
    map["doublearray"] =
        std::make_shared<ReadWriteTuple<std::vector<double>>>(&Reader::readDoubleArray, &Writer::writeDoubleArray);
    return map;
}

void runProgram(boost::program_options::variables_map& vm)
{
    // parse json
    std::string path = vm["file"].as<std::string>();

    if (!std::filesystem::is_regular_file(path)) throw std::runtime_error("Structure file does not exist.");

    std::ifstream structFile(path);
    std::stringstream contents;
    contents << structFile.rdbuf();
    boost::json::value json = boost::json::parse(contents.str());
    auto& input             = json.as_object()["input"].as_object();
    auto& output            = json.as_object()["output"].as_object();
    auto& structure         = json.at("structure").as_object();

    // parse user input
    bool pack = vm.count("pack");

    if (vm.count("gameFile")) input["path"] = vm["gameFile"].as<std::string>();
    if (vm.count("gameText")) input["textPath"] = vm["gameText"].as<std::string>();
    if (vm.count("gameOffset")) input["offset"] = vm["gameOffset"].as<std::int64_t>();
    if (vm.count("gameCount")) input["entryCount"] = vm["gameCount"].as<std::int64_t>();

    if (vm.count("userFile")) output["/output/path"] = vm["userFile"].as<std::string>();
    if (vm.count("userText")) output["/output/textPath"] = vm["userText"].as<std::string>();
    if (vm.count("userOffset")) output["/output/offset"] = vm["userOffset"].as<std::int64_t>();
    if (vm.count("userCount")) output["/input/entryCount"] = vm["userCount"].as<std::int64_t>();

    // create reader/writer
    std::shared_ptr<Reader> inReader  = readerFactory(pack ? output : input);
    std::shared_ptr<Writer> outWriter = writerFactory(pack ? input : output);

    // write file
    outWriter->startFile(structure);
    while (inReader->hasNext())
    {
        outWriter->startEntry();

        for (auto a : structure)
        {
            std::string name(a.key());
            std::string type(a.value().as_string());
            boost::algorithm::to_lower(type);
            readWriter[type]->convert(name, inReader, outWriter);
        }

        outWriter->finishEntry();
    }
    outWriter->finishFile();
}

int main(int count, char* args[])
{
    namespace po = boost::program_options;

    po::variables_map vm;

    try
    {
        po::positional_options_description pos;
        po::options_description desc("Usage: binaryconverter.exe <structurePath> [options]\n\nAllowed Options");

        auto options = desc.add_options();
        options("help,h", "This text.");
        options("file,f", po::value<std::string>(), "Path to the structure .json file to use");
        options("userFile,o",
                po::value<std::string>(),
                "Overwrites the path to the user file.\n"
                "Gets read when --pack is set.\nWhen not set the path from the structure .json is used.");
        options("gameFile,i",
                po::value<std::string>(),
                "Overwrites the path to the game file.\n"
                "Gets written when --pack is set.\n"
                "When not set the path from the structure .json is used.");
        options("gameCount,gc",
                po::value<int64_t>(),
                "Overwrites the entryCount parameter for the user file.\n"
                "Only valid for binary files. Ignored when set to 0.");
        options("userCount,uc",
                po::value<int64_t>(),
                "Overwrites the entryCount parameter for the user file.\n"
                "Only valid for binary files. Ignored when set to 0.");
        options("gameOffset,go", po::value<int64_t>(), "Overwrites the offset parameter for the game file.");
        options("userOffset,uo", po::value<int64_t>(), "Overwrites the offset parameter for the user file.");
        options("gameText,gt", po::value<std::string>(), "Overwrites the textPath parameter for the user file.");
        options("userText,ut", po::value<std::string>(), "Overwrites the textPath parameter for the user file.");
        options("pack,p", "Reverses input/output sections, used to re-create game files.");

        pos.add("file", -1);

        po::store(po::command_line_parser(count, args).options(desc).positional(pos).allow_unregistered().run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 1;
        }
    }
    catch (std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

    if (!vm.count("file"))
    {
        std::cout << "You must specify a file path!" << std::endl;
        return 1;
    }

    try
    {
        runProgram(vm);
    }
    catch (std::runtime_error& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return 0;
}