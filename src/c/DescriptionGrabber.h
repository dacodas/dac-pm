#pragma once

#include <iostream>

#include "VimParsingConstants.h"

namespace DacPM::VimParsing {

class SharingBufferedStdinStreambuf;

// This buffer is shared with the streambuf buffering stdin
class DescriptionGrabber
{
	static constexpr std::size_t targetHyphens {3};

	SharingBufferedStdinStreambuf& source;
	size_t currentHyphenCount {0};
	bool finished {false};

public:
	DescriptionGrabber(SharingBufferedStdinStreambuf&);
	void grab(std::string&);

private:
	// There are a few scenarios here:
	//
	// 1. We have found the end
	// 2. We have been reset and need to go again
	// 3. We are at the end of the buffer and need to go again
	enum class HyphenStatus {
		Success,
		FalseAlarm,
		EndOfBuffer
	};

	struct HyphenReturn {
		HyphenStatus status;
		char *location;
	};

	HyphenReturn handleHyphen(char* hyphen);

	void copyAndAdvance(std::string&, char *, char *);
};
}
