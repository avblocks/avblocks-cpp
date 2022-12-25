#include <iostream>
#include <iomanip>
#include <fstream>

#include "options.h"

#include <primo/avblocks/AVBlocks.h>

#include <primo/platform/Reference++.h>
#include <primo/platform/ErrorFacility.h>
#include <primo/platform/UString.h>

using namespace std;
using namespace primo::error;
using namespace primo::codecs;
using namespace primo::avblocks;

const char* metaPictureTypeName(primo::codecs::MetaPictureType::Enum pictype)
{
    using namespace primo::codecs;

    switch (pictype)
    {
    case MetaPictureType::Other:				return "Other";
    case MetaPictureType::FileIcon:				return "FileIcon";
    case MetaPictureType::OtherFileIcon:                    return "OtherFileIcon";
    case MetaPictureType::FrontCover:			return "FrontCover";
    case MetaPictureType::BackCover:			return "BackCover";
    case MetaPictureType::LeafletPage:			return "LeafletPage";
    case MetaPictureType::Media:				return "Media";
    case MetaPictureType::LeadArtist:			return "LeadArtist";
    case MetaPictureType::Artist:				return "Artist";
    case MetaPictureType::Conductor:			return "Conductor";
    case MetaPictureType::Band:				return "Band";
    case MetaPictureType::Composer:				return "Composer";
    case MetaPictureType::TextWriter:			return "TextWriter";
    case MetaPictureType::RecordingLocation:                return "RecordingLocation";
    case MetaPictureType::DuringRecording:                  return "DuringRecording";
    case MetaPictureType::DuringPerformance:                return "DuringPerformance";
    case MetaPictureType::VideoCapture:			return "VideoCapture";
    case MetaPictureType::BrightColoredFish:                return "BrightColoredFish";
    case MetaPictureType::Illustration:			return "Illustration";
    case MetaPictureType::ArtistLogotype:                   return "ArtistLogotype";
    case MetaPictureType::PublisherLogotype:                return "PublisherLogotype";

    default: return "unknown";
    }
}

void printMetadata(Metadata* meta)
{
    if (!meta)
    {
        cout << "No metadata is found" << endl;
        return;
    }

    cout << "Metadata" << endl;
    cout << "--------" << endl;

    MetaAttributeList* attlist = meta->attributes();
    cout << attlist->count() << " attributes:" << endl;

    for (int i=0; i < attlist->count(); ++i)
    {
        MetaAttribute* attrib = attlist->at(i);
        
        cout << setw(15) << left << attrib->name() << ": " << primo::ustring(attrib->value()) << endl;
    }
    cout << endl;

    MetaPictureList* piclist = meta->pictures();
    cout << piclist->count() << " pictures:" << endl;
    for (int i=0; i < piclist->count(); ++i)
    {
        MetaPicture* pic = piclist->at(i);
        cout << "#" << (i+1) << " mime: " << pic->mimeType() << "; size: " << pic->dataSize();

        cout << "; type: " << metaPictureTypeName(pic->pictureType()) << endl;
        cout << "description: " << pic->description() << endl;
    }
    cout << endl;
}

void printError(const ErrorInfo* e)
{
    if (ErrorFacility::Success == e->facility())
    {
        cout << "Success";
        
    }
    else
    {
                if (e->message())
                {
                    cout << primo::ustring(e->message()) << " ";
                }
         
                cout << "(facility:" << e->facility() << " error:" << e->code() << ")" << endl;
    }

    cout << endl;
}

void savePicture(primo::codecs::MetaPicture* pic, const string& baseFilename)
{
    string filename(baseFilename);

        string mimeType(pic->mimeType());
        
    if (0 == mimeType.compare(primo::codecs::MimeType::Jpeg))
    {
        filename += ".jpg";
    }
    else if (0 == mimeType.compare(primo::codecs::MimeType::Png))
    {
        filename += ".png";
    }
    else if (0 == mimeType.compare(primo::codecs::MimeType::Gif))
    {
        filename += ".gif";
    }
    else if (0 == mimeType.compare(primo::codecs::MimeType::Tiff))
    {
        filename += ".tiff";
    }
    else
    {
        // unexpected picture mime type
        return;
    }

    ofstream out(filename.c_str(), ios::out | ios::binary | ios::trunc);

    if (out)
    {
        out.write((const char*)pic->data(), pic->dataSize());
    }
}

void savePictures(primo::codecs::Metadata* meta, const char* inputFile)
{
    if (!meta || meta->pictures()->count() == 0)
        return;

    string baseFilename(inputFile);
    baseFilename += ".pic";

    MetaPictureList* piclist = meta->pictures();
    for (int i=0; i < piclist->count(); ++i)
    {
        char num[8];
        sprintf(num, "%d", i+1);
        string picname (baseFilename);
        picname += num;
        savePicture(piclist->at(i), picname);
    }
}

bool metaInfo(Options& opt)
{
    auto info = primo::make_ref(Library::createMediaInfo());

    info->inputs()->at(0)->setFile(primo::ustring(opt.avfile));

    if(info->open())
    {
            Metadata* meta = NULL;
            
            if(info->outputs()->count() > 0)
                meta = info->outputs()->at(0)->metadata();

            printMetadata(meta);
            savePictures(meta, opt.avfile.c_str());
            return true;
    }
    else
    {
            printError(info->error());
            return false;
    }
}

int main(int argc, char* argv[])
{
        Options opt;
        
        switch(prepareOptions( opt, argc, argv))
        {
                case Command: return 0;
                case Error: return 1;
        }
    
        Library::initialize();

    //Library::setLicense("<license-string>");

        bool metaInfoResult = metaInfo(opt);

    Library::shutdown();

    return metaInfoResult ? 0 : 1;
}
