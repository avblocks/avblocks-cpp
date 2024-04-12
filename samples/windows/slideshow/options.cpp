#include "stdafx.h"
#include "options.h"
#include "program_options.h"
#include "util.h"

namespace fs = std::filesystem;

using namespace std;
using namespace primo::program_options;
using namespace primo::codecs;
using namespace primo::avblocks;

PresetDescriptor avb_presets[] = {
    
    // video presets
    { Preset::Video::DVD::PAL_4x3_MP2,      L"mpg" },
    { Preset::Video::DVD::PAL_16x9_MP2,     L"mpg" },
    { Preset::Video::DVD::NTSC_4x3_MP2,     L"mpg" },
    { Preset::Video::DVD::NTSC_16x9_MP2,    L"mpg" },
    { Preset::Video::AppleTV::H264_480p,    L"mp4" },
    { Preset::Video::AppleTV::H264_720p,    L"mp4" },
    { Preset::Video::AppleTV::MPEG4_480p,   L"mp4" },
    { Preset::Video::AppleLiveStreaming::WiFi_H264_640x480_30p_1200K_AAC_96K,   L"ts" },
    { Preset::Video::AppleLiveStreaming::WiFi_Wide_H264_1280x720_30p_4500K_AAC_128K,    L"ts" },
    { Preset::Video::Generic::MP4::Base_H264_AAC,   L"mp4" },
    { Preset::Video::iPad::H264_576p,   L"mp4" },
    { Preset::Video::iPad::H264_720p,   L"mp4" },
    { Preset::Video::iPad::MPEG4_480p,  L"mp4" },
    { Preset::Video::iPhone::H264_480p, L"mp4" },
    { Preset::Video::iPhone::MPEG4_480p,    L"mp4" },
    { Preset::Video::iPod::H264_240p,   L"mp4" },
    { Preset::Video::iPod::MPEG4_240p,  L"mp4" },
    { Preset::Video::AndroidPhone::H264_360p,   L"mp4" },
    { Preset::Video::AndroidPhone::H264_720p,   L"mp4" },
    { Preset::Video::AndroidTablet::H264_720p,  L"mp4" },
    { Preset::Video::AndroidTablet::WebM_VP8_720p,  L"webm" },
    { Preset::Video::VCD::PAL,  L"mpg" },
    { Preset::Video::VCD::NTSC, L"mpg" },
    { Preset::Video::Generic::WebM::Base_VP8_Vorbis,    L"webm" }
};

const int avb_presets_len = sizeof(avb_presets) / sizeof(PresetDescriptor);

void listPresets()
{
    wcout << L"\nPRESETS" << endl;
    wcout << L"-------" << endl;
    for (int i=0; i < avb_presets_len; ++i)
    {
        const PresetDescriptor& preset = avb_presets[i];
        wcout << left << setw(45) <<  preset.name << L" (." << preset.extension << L")" << endl;
    }
}

template<typename T>
PresetDescriptor* getPresetByName(const T* presetName)
{
    auto preset = 
        find_if(begin(avb_presets), end(avb_presets), 
        [presetName] (const PresetDescriptor& preset) {
            return 0 == compareNoCase(preset.name, presetName);
        });

    if (preset != end(avb_presets))
        return preset;

    return NULL;
}

inline std::wistringstream &operator>>(std::wistringstream &in, string &obj)
{
    wstring preset;
    in >> preset;
    
    obj.assign(preset.begin(), preset.end());

    return in;
}

void help(OptionsConfig<wchar_t>& optionsConfig)
{
    wcout << L"\nUsage: slideshow --input <directory> --output <file> [--preset <PRESET>]";
    wcout << L" [--presets]";
    wcout << endl;
    doHelp(wcout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
    opt.input_dir = getExeDir() + L"/../../assets/img";

    fs::path output(getExeDir() + L"/../../output/slideshow");
    fs::create_directories(output);

    wostringstream s;
    s << output.c_str() << L"/cube.mp4";
    opt.output_file = s.str();

    opt.preset = *getPresetByName(Preset::Video::Generic::MP4::Base_H264_AAC);    
}

bool validateOptions(Options& opt)
{
    if (!opt.preset.name)
    {
        opt.preset = *getPresetByName(Preset::Video::Generic::MP4::Base_H264_AAC);
    }
    
    // fix output: append file extension
    if (!opt.output_file.empty() && opt.preset.extension)
    {        
        wstring::size_type pos = opt.output_file.rfind('.');
        if (0 == pos || string::npos == pos)
        {
            opt.output_file.append(L".");
        }
        else
        {
            opt.output_file = opt.output_file.substr(0, pos+1);
        }

        opt.output_file.append(opt.preset.extension);
    }

    if (opt.output_file.empty())
    {
        return false;
    }

    return true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.input_dir;
        wcout << L" --output " << opt.output_file;
        wcout << L" --preset " << opt.preset.name;
        wcout << endl;
        return Parsed;
    }

    OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,?",		opt.help,						L"")
        (L"input,i",		opt.input_dir, wstring(),			L"input directory containing images for the slideshow.")
        (L"output,o",		opt.output_file, wstring(),			L"output filename (without extension). The extension is added based on the preset.")
        (L"preset,p",		opt.preset, PresetDescriptor(),		L"output preset id. Use --presets to list all supported presets")
        (L"presets",		opt.list_presets,					L"list presets");

    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (ParseFailure<wchar_t> &ex)
    {
        wcout << ex.message() << endl;
        help(optionsConfig);
        return Error;
    }

    if(opt.help)
    {
        help(optionsConfig);
        return Command;
    }

    if (opt.list_presets)
    {
        listPresets();
        return Command;
    }

    if (!validateOptions(opt))
    {
        help(optionsConfig);
        return Error;
    }

    return Parsed;
}

std::wistringstream &operator>>(std::wistringstream &in, PresetDescriptor &preset)
{
    std::wstring presetName;
    in >> presetName;

    PresetDescriptor* presetDesc = getPresetByName(presetName.c_str());
    if(!presetDesc)
        throw primo::program_options::ParseFailure<wchar_t>(L"", presetName, L"Parse error");

    preset = *presetDesc;
    return in;
}