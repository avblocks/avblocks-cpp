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

// Network Abstraction Layer Unit Type Definitions per H.265/HEVC spec (ITU-T H.265 Table 7-1)
enum NALUType
{
    TRAIL_N      = 0,
    TRAIL_R      = 1,
    TSA_N        = 2,
    TSA_R        = 3,
    STSA_N       = 4,
    STSA_R       = 5,
    RADL_N       = 6,
    RADL_R       = 7,
    RASL_N       = 8,
    RASL_R       = 9,
    BLA_W_LP     = 16,
    BLA_W_RADL   = 17,
    BLA_N_LP     = 18,
    IDR_W_RADL   = 19,
    IDR_N_LP     = 20,
    CRA_NUT      = 21,
    VPS_NUT      = 32,
    SPS_NUT      = 33,
    PPS_NUT      = 34,
    AUD_NUT      = 35,
    EOS_NUT      = 36,
    EOB_NUT      = 37,
    FD_NUT       = 38,
    PREFIX_SEI   = 39,
    SUFFIX_SEI   = 40,
    UNSPEC       = 62,
};

NALUType nal_unit_type(uint8_t first_byte)
{
    int type = (first_byte >> 1) & 0x3F;
    return static_cast<NALUType>(type);
}

#define MAP_ENUM_VALUE(p) strings[p] = #p
std::wostream& operator<<(std::wostream& out, const NALUType value)
{
    static std::map<NALUType, std::string> strings;
    if (strings.empty())
    {
        MAP_ENUM_VALUE(TRAIL_N);
        MAP_ENUM_VALUE(TRAIL_R);
        MAP_ENUM_VALUE(TSA_N);
        MAP_ENUM_VALUE(TSA_R);
        MAP_ENUM_VALUE(STSA_N);
        MAP_ENUM_VALUE(STSA_R);
        MAP_ENUM_VALUE(RADL_N);
        MAP_ENUM_VALUE(RADL_R);
        MAP_ENUM_VALUE(RASL_N);
        MAP_ENUM_VALUE(RASL_R);
        MAP_ENUM_VALUE(BLA_W_LP);
        MAP_ENUM_VALUE(BLA_W_RADL);
        MAP_ENUM_VALUE(BLA_N_LP);
        MAP_ENUM_VALUE(IDR_W_RADL);
        MAP_ENUM_VALUE(IDR_N_LP);
        MAP_ENUM_VALUE(CRA_NUT);
        MAP_ENUM_VALUE(VPS_NUT);
        MAP_ENUM_VALUE(SPS_NUT);
        MAP_ENUM_VALUE(PPS_NUT);
        MAP_ENUM_VALUE(AUD_NUT);
        MAP_ENUM_VALUE(EOS_NUT);
        MAP_ENUM_VALUE(EOB_NUT);
        MAP_ENUM_VALUE(FD_NUT);
        MAP_ENUM_VALUE(PREFIX_SEI);
        MAP_ENUM_VALUE(SUFFIX_SEI);
    }

    auto it = strings.find(value);
    if (it != strings.end())
        return out << it->second.c_str();

    wostringstream tmp;
    tmp << L"RSV_" << static_cast<int>(value);
    return out << tmp.str();
}

void print_nalu_header(const uint8_t* data)
{
    wcout << left << L"  "
          << setw(12) << nal_unit_type(*data)
          << endl;
}

void print_nalus(MediaBuffer* buffer)
{
    while (buffer->dataSize() > 1)
    {
        int dataOffset = buffer->dataOffset();
        int dataSize   = buffer->dataSize();
        const uint8_t* data = buffer->data();

        if (dataSize >= 3 &&
            0x00 == data[0] &&
            0x00 == data[1] &&
            0x01 == data[2])
        {
            if (dataSize >= 4)
                print_nalu_header(data + 3);

            buffer->setData(dataOffset + 3, dataSize - 3);
        }
        else if (dataSize >= 4 &&
                 0x00 == data[0] &&
                 0x00 == data[1] &&
                 0x00 == data[2] &&
                 0x01 == data[3])
        {
            if (dataSize >= 5)
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
    file_name << outputDir << L"/au_" << setw(4) << setfill(L'0') << au_index << L".h265";
    ofstream file(file_name.str(), ios::binary | ios::trunc);
    if (file)
    {
        file.write((const char*)buffer->data(), buffer->dataSize());
    }
}

bool parse_h265_stream(const wstring& inputFile, const wstring& outputDir)
{
    // Create an input socket from file
    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setFile(inputFile.c_str());

    // Create an output socket with one video pin configured for H.265
    auto outStreamInfo = primo::make_ref(Library::createVideoStreamInfo());
    outStreamInfo->setStreamType(StreamType::H265);

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

    bool parseResult = parse_h265_stream(opt.input_file, opt.output_dir);

    Library::shutdown();

    if (!parseResult)
        return 1;

    wcout << L"\nSuccessfully parsed input file: " << opt.input_file << L"\n";
    wcout << L"Output directory: " << opt.output_dir << L"\n\n";
    return 0;
}
