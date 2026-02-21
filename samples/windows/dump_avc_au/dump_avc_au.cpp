/*
 *  Copyright (c) Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/
#include "stdafx.h"
#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

namespace fs = std::filesystem;

// Network Abstraction Layer Unit Definitions per H.264/AVC spec
enum NALUType
{
    UNSPEC    = 0,  // Unspecified
    SLICE     = 1,  // Coded slice of a non-IDR picture
    DPA       = 2,  // Coded slice data partition A
    DPB       = 3,  // Coded slice data partition B
    DPC       = 4,  // Coded slice data partition C
    IDR       = 5,  // Coded slice of an IDR picture
    SEI       = 6,  // Supplemental enhancement information
    SPS       = 7,  // Sequence parameter set
    PPS       = 8,  // Picture parameter set
    AUD       = 9,  // Access unit delimiter
    EOSEQ     = 10, // End of sequence
    EOSTREAM  = 11, // End of stream
    FILL      = 12  // Filler data
};

enum NALUPriority
{
    DISPOSABLE  = 0,
    LOW         = 1,
    HIGH        = 2,
    HIGHEST     = 3,
};

NALUType nal_unit_type(uint8_t byte)
{
    return static_cast<NALUType>(byte & 0x1f);
}

NALUPriority nal_unit_ref_idc(uint8_t byte)
{
    return static_cast<NALUPriority>((byte & 0x60) >> 5);
}

#define MAP_ENUM_VALUE(p) strings[p] = #p
std::wostream& operator<<(std::wostream& out, const NALUType value)
{
    static std::map<NALUType, std::string> strings;
    if (strings.empty())
    {
        MAP_ENUM_VALUE(SLICE);
        MAP_ENUM_VALUE(DPA);
        MAP_ENUM_VALUE(DPB);
        MAP_ENUM_VALUE(DPC);
        MAP_ENUM_VALUE(IDR);
        MAP_ENUM_VALUE(SEI);
        MAP_ENUM_VALUE(SPS);
        MAP_ENUM_VALUE(PPS);
        MAP_ENUM_VALUE(AUD);
        MAP_ENUM_VALUE(EOSEQ);
        MAP_ENUM_VALUE(EOSTREAM);
        MAP_ENUM_VALUE(FILL);
    }
    return out << strings[value].c_str();
}

std::wostream& operator<<(std::wostream& out, const NALUPriority value)
{
    static std::map<NALUPriority, std::string> strings;
    if (strings.empty())
    {
        MAP_ENUM_VALUE(DISPOSABLE);
        MAP_ENUM_VALUE(LOW);
        MAP_ENUM_VALUE(HIGH);
        MAP_ENUM_VALUE(HIGHEST);
    }
    return out << strings[value].c_str();
}

void print_nalu_header(const uint8_t* data)
{
    wcout << left << L"  "
          << setw(10) << nal_unit_type(*data) << L": "
          << nal_unit_ref_idc(*data)
          << endl;
}

void print_nalus(MediaBuffer* buffer)
{
    while (buffer->dataSize() > 1)
    {
        int dataOffset = buffer->dataOffset();
        int dataSize = buffer->dataSize();
        const uint8_t* data = buffer->data();

        if (dataSize >= 3 &&
            0x00 == data[0] &&
            0x00 == data[1] &&
            0x01 == data[2])
        {
            print_nalu_header(data + 3);
            buffer->setData(dataOffset + 3, dataSize - 3);
        }
        else if (dataSize >= 4 &&
                 0x00 == data[0] &&
                 0x00 == data[1] &&
                 0x00 == data[2] &&
                 0x01 == data[3])
        {
            print_nalu_header(data + 4);
            buffer->setData(dataOffset + 4, dataSize - 4);
        }
        else
        {
            buffer->setData(dataOffset + 1, dataSize - 1);
        }
    }
}

void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
    if (action)
    {
        wcout << action << L": ";
    }

    if (primo::error::ErrorFacility::Success == e->facility())
    {
        wcout << L"Success" << endl;
        return;
    }

    if (e->message())
    {
        wcout << e->message() << L", ";
    }

    wcout << L"facility:" << e->facility() << L", error:" << e->code() << endl;
}

void write_au_file(const wstring& outputDir, int au_index, MediaBuffer* buffer)
{
    wostringstream file_name;
    file_name << outputDir << L"/au_" << setw(4) << setfill(L'0') << au_index << L".h264";
    ofstream file(file_name.str(), ios::binary | ios::trunc);
    if (file)
    {
        file.write((const char*)buffer->data(), buffer->dataSize());
    }
}

bool parse_h264_stream(const wstring& inputFile, const wstring& outputDir)
{
    // Create an input socket from file
    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setFile(inputFile.c_str());

    // Create an output socket with one video pin
    auto outStreamInfo = primo::make_ref(Library::createVideoStreamInfo());

    auto outPin = primo::make_ref(Library::createMediaPin());
    outPin->setStreamInfo(outStreamInfo.get());

    auto outSocket = primo::make_ref(Library::createMediaSocket());
    outSocket->pins()->add(outPin.get());

    // Create Transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    auto res = transcoder->open();
    printError(L"transcoder open", transcoder->error());

    if (!res)
        return false;

    int32_t inputIndex = 0;
    auto accessUnit = primo::make_ref(Library::createMediaSample());

    if (!makeDir(outputDir))
    {
        wcout << L"cannot create output directory: " << outputDir << endl;
        return false;
    }

    int au_index = 0;
    while (transcoder->pull(inputIndex, accessUnit.get()))
    {
        auto au_buffer = accessUnit->buffer();
        wcout << L"AU #" << au_index << L", " << au_buffer->dataSize() << L" bytes" << endl;
        write_au_file(outputDir, au_index, au_buffer);
        print_nalus(au_buffer);
        ++au_index;
    }
    transcoder->close();
    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch (prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:   return 1;
        default:      break;
    }

    Library::initialize();

    // set your license string
    // Library::setLicense("avblocks-license-xml");

    bool parseResult = parse_h264_stream(opt.input_file, opt.output_dir);

    Library::shutdown();

    if (!parseResult)
        return 1;

    wcout << L"\nSuccessfully parsed input file: " << opt.input_file << L"\n";
    wcout << L"Output directory: " << opt.output_dir << L"\n\n";
    return 0;
}
