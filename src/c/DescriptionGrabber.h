#pragma once

#include <iostream>

#include "VimParsingConstants.h"

namespace DacPM::VimParsing {

class StdinWithUngetStreambuf;

// This buffer is shared with the streambuf buffering stdin
class DescriptionGrabber
{
	static constexpr std::size_t targetHyphens {3};

	StdinWithUngetStreambuf& source;
	char *buffer;
	size_t currentHyphenCount {0};
	bool finished {false};

public:
	DescriptionGrabber(StdinWithUngetStreambuf&, char *);
	void grab(std::string&);

private:
	char *handleHyphen(char* hyphen);
};
}
