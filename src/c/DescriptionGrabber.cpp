#include "DescriptionGrabber.h"
#include "StdinWithUngetStreambuf.h"

#include <cstring>
#include <algorithm>

namespace DacPM::VimParsing {

DescriptionGrabber::DescriptionGrabber(StdinWithUngetStreambuf& streambuf)
	: source {streambuf} {}

DescriptionGrabber::HyphenReturn DescriptionGrabber::handleHyphen(char* hyphen)
{
	if ( hyphen == source.egptr() )

		return {HyphenStatus::EndOfBuffer, nullptr};

	char *nonHyphen {
		std::find_if_not(hyphen + 1, source.egptr(), [] (char c) { return c == '-'; })
	};

	std::size_t additionalHyphens { nonHyphen - hyphen };

	if ( currentHyphenCount + additionalHyphens >= targetHyphens )
	{
		// Set the end of the streambuf to hyphen because we will be
		// needing to return EOF after that
		std::cerr << "We found the proper number of hyphens!\n";
		finished = true;
		return {HyphenStatus::Success, nonHyphen};
	}
	else if ( nonHyphen != source.egptr() )
	{
		// We have come across a series of insufficient hyphens, so we
		// will start our count anew
		std::cerr << "We didn't find enough hyphens there, so we are restarting the count\n";
		currentHyphenCount = 0;
		return {HyphenStatus::FalseAlarm, nonHyphen};
	}
	else
	{
		// We don't have enough hyphens, and we are split amongst two
		// buffers
		std::cerr << "We are split amongst two buffers, and I am sad\n";
		exit(1);
	}
}

void DescriptionGrabber::grab(std::string& target)
{
	auto eof {std::char_traits<char>::eof()};

	while ( !finished )
	{
		std::streamsize charsToRead { source.sgetn(nullptr, bufferSize) };

		if ( charsToRead == eof )
		{
			std::cerr << "We reached the end of file looking for the end of the description section!\n";
			exit(1);
		}

		char *hyphen { std::find(
			source.gptr(), source.egptr(), '-'
		) };

		auto [status, endOfHyphens] = handleHyphen(hyphen);
		auto currentSize {target.size()};

		char *copyEnd;

		using enum HyphenStatus;

		switch (status)
		{
		case Success:
		case FalseAlarm:
		{
			copyEnd = endOfHyphens;
			break;
		}
		case EndOfBuffer:
		{
			copyEnd = source.egptr();
			break;
		}
		}

		std::streamsize sizeToCopy { hyphen - source.gptr() };
		target.resize(currentSize + sizeToCopy);
		std::memcpy(target.data() + currentSize, source.gptr(), sizeToCopy);
		source.advanceTo( copyEnd );
	}
}
}
