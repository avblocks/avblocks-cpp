/*
 *  Copyright (c) Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/
#include "stdafx.h"
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
using namespace primo::program_options;

ColorDescriptor color_formats[] = {

    { ColorFormat::YV12,   L"yv12",   L"Planar Y, V, U (4:2:0) (note V,U order!)" },
    { ColorFormat::NV12,   L"nv12",   L"Planar Y, merged U->V (4:2:0)" },
    { ColorFormat::YUY2,   L"yuy2",   L"Composite Y->U->Y->V (4:2:2)" },
    { ColorFormat::UYVY,   L"uyvy",   L"Composite U->Y->V->Y (4:2:2)" },
    { ColorFormat::YUV411, L"yuv411", L"Planar Y, U, V (4:1:1)" },
    { ColorFormat::YUV420, L"yuv420", L"Planar Y, U, V (4:2:0)" },
    { ColorFormat::YUV422, L"yuv422", L"Planar Y, U, V (4:2:2)" },
    { ColorFormat::Y41P,   L"y41p",   L"Composite Y, U, V (4:1:1)" },
    { ColorFormat::BGR32,  L"bgr32",  L"Composite B->G->R" },
    { ColorFormat::BGRA32, L"bgra32", L"Composite B->G->R->A" },
    { ColorFormat::BGR24,  L"bgr24",  L"Composite B->G->R" },
    { ColorFormat::BGR565, L"bgr565", L"Composite B->G->R, 5 bit per B & R, 6 bit per G" },
    { ColorFormat::BGR555, L"bgr555", L"Composite B->G->R->A, 5 bit per component, 1 bit per A" },
    { ColorFormat::BGR444, L"bgr444", L"Composite B->G->R->A, 4 bit per component" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);

ColorDescriptor* getColorByName(const wchar_t* colorName)
{
    for (int i = 0; i < color_formats_len; ++i)
    {
        ColorDescriptor& color = color_formats[i];
        if (compareNoCase(color.name, colorName))
            return &color;
    }
    return NULL;
}

ColorDescriptor* getColorById(primo::codecs::ColorFormat::Enum Id)
{
    for (int i = 0; i < color_formats_len; ++i)
    {
        ColorDescriptor& color = color_formats[i];
        if (color.Id == Id)
            return &color;
    }
    return NULL;
}

void listColors()
{
    wcout << L"\nCOLORS:\n";
    wcout << L"-------\n";
    for (int i = 0; i < color_formats_len; ++i)
    {
        const ColorDescriptor& color = color_formats[i];
        wcout << left << setw(20) << color.name << color.description << endl;
    }
    wcout << endl;
}

void help(OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"\nUsage: dec_hevc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>]";
    wcout << L" [--colors]" << endl;
    doHelp(wcout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.input_dir = getExeDir() + L"/../../assets/vid/foreman_qcif.h265.au/";
    opt.output_color = *getColorById(primo::codecs::ColorFormat::YUV420);
}

std::wistringstream& operator>>(std::wistringstream& in, FrameSize& frameSize)
{
    in >> frameSize.width;
    wchar_t ch;
    in >> ch;
    in >> frameSize.height;
    return in;
}

std::wistringstream& operator>>(std::wistringstream& in, ColorDescriptor& color)
{
    std::wstring strColorName;
    in >> strColorName;
    ColorDescriptor* colorDesc = getColorByName(strColorName.c_str());
    if (!colorDesc)
        throw ParseFailure<wchar_t>(L"", strColorName, L"Parse error");
    color = *colorDesc;
    return in;
}

bool validateOptions(Options& opt)
{
    return opt.input_dir.empty() ? false : true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.input_dir;
        wcout << endl;
        return Parsed;
    }

    OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",   opt.help,                                  L"")
        (L"input,i",  opt.input_dir,   wstring(),                 L"input directory (contains sequence of compressed file)")
        (L"output,o", opt.output_file, wstring(),                 L"output YUV file")
        (L"frame,f",  opt.frame_size,  FrameSize(),               L"frame size, <width>x<height>")
        (L"rate,r",   opt.fps,         0.0,                       L"frame rate")
        (L"color,c",  opt.output_color, ColorDescriptor(),        L"output color format. Use --colors to list all supported color formats")
        (L"colors",   opt.list_colors,                            L"list COLOR constants");

    try
    {
        scanArgv(optcfg, argc, argv);
    }
    catch (ParseFailure<wchar_t>& ex)
    {
        wcout << ex.message() << endl;
        help(optcfg);
        return Error;
    }

    if (opt.help)
    {
        help(optcfg);
        return Command;
    }

    if (opt.list_colors)
    {
        listColors();
        return Command;
    }

    if (!validateOptions(opt))
    {
        help(optcfg);
        return Error;
    }

    return Parsed;
}
