#pragma once

#include <iostream>

#include "VimParsingConstants.h"

// The buffer here is shared directly with the DescriptionGrabber.
// When DescriptionGrabber calls sgetn, he is expecting to be just have the
// next however many characters ready
namespace DacPM::VimParsing {

class StdinWithUngetStreambuf : public std::streambuf {
private:
	std::istream& source;
	char buffer[bufferSize];
	bool atEnd {false};

	void ensureGetAreaEndWithBuffer();
	void checkIfEnd(std::streamsize, std::streamsize);

	friend char *exposeBuffer(StdinWithUngetStreambuf&);

protected:
	int_type underflow();
	std::streamsize xsgetn(char_type *, std::streamsize);
	// std::streambuf* setbuf(char_type *, std::streamsize);
	
public:

	StdinWithUngetStreambuf(std::istream&);
};

char *exposeBuffer(StdinWithUngetStreambuf&);

}
