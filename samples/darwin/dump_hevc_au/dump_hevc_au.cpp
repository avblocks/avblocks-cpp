/*
 *  Copyright (c) Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <memory.h>
#include <filesystem>

#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

namespace fs = std::filesystem;

/*
 Access Unit: one picture (VPS/SPS/PPS parameters + all slices)
 The elementary stream may contain optional access unit delimiters (AUD).
 A picture may consist of one or more slices.

 HEVC NAL unit header (2 bytes):
   forbidden_zero_bit    (1 bit)
   nal_unit_type         (6 bits)
   nuh_layer_id          (6 bits)
   nuh_temporal_id_plus1 (3 bits)

 nal_unit_type = (first_byte >> 1) & 0x3F
*/

// Network Abstraction Layer Unit Type Definitions per H.265/HEVC spec (ITU-T H.265 Table 7-1)
enum NALUType
{
    TRAIL_N      = 0,   // Trailing picture, non-reference
    TRAIL_R      = 1,   // Trailing picture, reference
    TSA_N        = 2,   // Temporal sub-layer access, non-reference
    TSA_R        = 3,   // Temporal sub-layer access, reference
    STSA_N       = 4,   // Step-wise temporal sub-layer access, non-reference
    STSA_R       = 5,   // Step-wise temporal sub-layer access, reference
    RADL_N       = 6,   // Random access decodable leading, non-reference
    RADL_R       = 7,   // Random access decodable leading, reference
    RASL_N       = 8,   // Random access skipped leading, non-reference
    RASL_R       = 9,   // Random access skipped leading, reference
    BLA_W_LP     = 16,  // Broken link access with leading pictures
    BLA_W_RADL   = 17,  // Broken link access with RADL pictures
    BLA_N_LP     = 18,  // Broken link access without leading pictures
    IDR_W_RADL   = 19,  // IDR with RADL pictures
    IDR_N_LP     = 20,  // IDR without leading pictures
    CRA_NUT      = 21,  // Clean random access
    VPS_NUT      = 32,  // Video parameter set
    SPS_NUT      = 33,  // Sequence parameter set
    PPS_NUT      = 34,  // Picture parameter set
    AUD_NUT      = 35,  // Access unit delimiter
    EOS_NUT      = 36,  // End of sequence
    EOB_NUT      = 37,  // End of bitstream
    FD_NUT       = 38,  // Filler data
    PREFIX_SEI   = 39,  // Supplemental enhancement information (prefix)
    SUFFIX_SEI   = 40,  // Supplemental enhancement information (suffix)
    UNSPEC       = 62,  // Unspecified
};

NALUType nal_unit_type(uint8_t first_byte)
{
    // nal_unit_type occupies bits [6:1] of the first header byte
    int type = (first_byte >> 1) & 0x3F;
    return static_cast<NALUType>(type);
}

#define MAP_ENUM_VALUE(p) strings[p] = #p
std::ostream& operator<<(std::ostream& out, const NALUType value)
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
        return out << it->second;

    return out << "RSV_" << static_cast<int>(value);
}

void print_nalu_header(const uint8_t* data)
{
    // HEVC NAL unit header is 2 bytes; nal_unit_type is in the first byte
    std::cout << left << "  "
              << setw(12) << nal_unit_type(*data)
              << std::endl;
}

void print_nalus(MediaBuffer* buffer)
{
    // This parsing code assumes that MediaBuffer contains
    // a single Access Unit of one or more complete NAL Units
    while (buffer->dataSize() > 1)
    {
        int dataOffset = buffer->dataOffset();
        int dataSize   = buffer->dataSize();
        const uint8_t* data = buffer->data();

        // is this a NALU with a 3-byte start code prefix (0x000001)?
        if (dataSize >= 3 &&
            0x00 == data[0] &&
            0x00 == data[1] &&
            0x01 == data[2])
        {
            if (dataSize >= 4)
                print_nalu_header(data + 3);

            buffer->setData(dataOffset + 3, dataSize - 3);
        }
        // OR is this a NALU with a 4-byte start code prefix (0x00000001)?
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

        // NOTE: Some NALUs may have a trailing zero byte. The `while`
        // condition `buffer->dataSize() > 1` will effectively
        // skip the trailing zero byte.
    }
}

void printError(const char* action, const primo::error::ErrorInfo* e)
{
    if (action)
    {
        cout << action << ": ";
    }

    if (primo::error::ErrorFacility::Success == e->facility())
    {
        cout << "Success" << endl;
        return;
    }

    if (e->message())
    {
        cout << primo::ustring(e->message()) << " ";
    }

    cout << "(facility:" << e->facility() << " error:" << e->code() << ")" << endl;
}

void write_au_file(string outputDir, int au_index, MediaBuffer* buffer)
{
    ostringstream file_name;
    file_name << outputDir << "/au_" << setw(4) << setfill('0') << au_index << ".h265";
    ofstream file(file_name.str(), ios::binary | ios::trunc);
    if (file)
    {
        file.write((const char*)buffer->data(), buffer->dataSize());
    }
}

bool parse_h265_stream(string inputFile, string outputDir)
{
    // Create an input socket from file
    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setFile(primo::ustring(inputFile));

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
    printError("transcoder open", transcoder->error());

    if (!res)
        return false;

    int32_t inputIndex = 0;
    auto accessUnit = primo::make_ref(Library::createMediaSample());

    if (!makeDir(outputDir))
    {
        cout << "cannot create output directory: " << outputDir << endl;
        return false;
    }

    int au_index = 0;
    while (transcoder->pull(inputIndex, accessUnit.get()))
    {
        // Each call to Transcoder::pull returns one Access Unit.
        // The Access Unit may contain one or more NAL units.

        auto au_buffer = accessUnit->buffer();
        cout << "AU #" << au_index << ", " << au_buffer->dataSize() << " bytes" << endl;
        write_au_file(outputDir, au_index, au_buffer);
        print_nalus(au_buffer);
        ++au_index;
    }
    transcoder->close();
    return true;
}

int main(int argc, char* argv[])
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

    cout << "\nSuccessfully parsed input file: " << opt.input_file << "\n";
    cout << "Output directory: " << opt.output_dir << "\n\n";
    return 0;
}
