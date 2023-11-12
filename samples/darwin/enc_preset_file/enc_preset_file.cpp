/*
 *  Copyright (c)  Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a MIT License
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

#include <unistd.h>

#include <iostream>
#include <string>
#include <filesystem>

#include <primo/platform/reference++.h>
#include <primo/platform/error_facility.h>
#include <primo/platform/ustring.h>

#include <primo/avblocks/avb.h>

#include "options.h"
#include "util.h"


using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

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

bool encode(const Options& opt)
{
    deleteFile(opt.output_file.c_str());
    
	auto transcoder = primo::make_ref(Library::createTranscoder());

    // In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
	transcoder->setAllowDemoMode(TRUE);

	// Set Input
	{
		auto socket = primo::make_ref(primo::avblocks::Library::createMediaSocket());
		auto pin = primo::make_ref(primo::avblocks::Library::createMediaPin());

		auto vinformat = primo::make_ref(primo::avblocks::Library::createVideoStreamInfo());
		/*
		the input frame rate determines how fast the video is played
		*/
		vinformat->setFrameRate(opt.yuv_fps);
		vinformat->setFrameWidth(opt.yuv_frame.width);
		vinformat->setFrameHeight(opt.yuv_frame.height);
		vinformat->setColorFormat(opt.yuv_color.Id);
		vinformat->setStreamType(StreamType::UncompressedVideo);
		vinformat->setScanType(ScanType::Progressive);

		pin->setStreamInfo(vinformat.get());
		socket->pins()->add(pin.get());
		socket->setFile(primo::ustring(opt.yuv_file));
		socket->setStreamType(StreamType::UncompressedVideo);
		transcoder->inputs()->add(socket.get());
	}

	// Set Output
	{
		auto socket = primo::make_ref(primo::avblocks::Library::createMediaSocket(opt.preset.name));
		socket->setFile(primo::ustring(opt.output_file));
		transcoder->outputs()->add(socket.get());
	}

	bool_t res = transcoder->open();
	printError("Open Transcoder", transcoder->error());
	if (!res)
		return false;

	res = transcoder->run();
	printError("Run Transcoder", transcoder->error());
	if (!res)
		return false;

	transcoder->close();
    
    cout << "created video: " << opt.output_file << endl;
	
	return true;
}

int main(int argc, char* argv[])
{
    Options opt;
    
    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:	return 1;
        case Parsed: break;
    }
    
	primo::avblocks::Library::initialize();
    
    bool encode_result = encode(opt);

	primo::avblocks::Library::shutdown();

    return encode_result ? 0 : 1;
}
