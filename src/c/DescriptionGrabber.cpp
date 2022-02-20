#include "DescriptionGrabber.h"
#include "SharingBufferedStdinStreambuf.h"

#include <cstring>
#include <algorithm>

namespace DacPM::VimParsing {

DescriptionGrabber::DescriptionGrabber(SharingBufferedStdinStreambuf& streambuf)
	: source {streambuf} {}

DescriptionGrabber::HyphenReturn DescriptionGrabber::handleHyphen(char* hyphen)
{
	if ( hyphen == source.egptr() )

		return {HyphenStatus::EndOfBuffer, nullptr};

	char *nonHyphen {
		std::find_if_not(hyphen + 1, source.egptr(), [] (char c) { return c == '-'; })
	};

	std::size_t hyphensFound { nonHyphen - hyphen };

	if ( currentHyphenCount + hyphensFound >= targetHyphens )
	{
		finished = true;
		return {HyphenStatus::Success, nonHyphen};
	}
	else if ( nonHyphen != source.egptr() )
	{
		// We didn't find enough hyphens there, so we are restarting
		// the count
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

void DescriptionGrabber::copyAndAdvance(std::string& destination, char *hyphen, char *copyEnd)
{
	auto destinationSize {destination.size()};

	std::streamsize sizeToCopy { hyphen - source.gptr() };
	destination.resize(destinationSize + sizeToCopy);
	std::memcpy(destination.data() + destinationSize, source.gptr(), sizeToCopy);
	source.advanceTo( copyEnd );
}

void DescriptionGrabber::grab(std::string& destination)
{
	auto eof {std::char_traits<char>::eof()};

	while ( !finished )
	{
		std::streamsize charsRead { source.sgetn(nullptr, bufferSize) };

		if ( charsRead == eof )
		{
			std::cerr << "We reached the end of file looking for the end of the description section!\n";
			exit(1);
		}

		using enum HyphenStatus;

		char
			*copyEnd,
			*hyphen { std::find(
				source.gptr(), source.egptr(), '-'
			) };

		auto [status, endOfHyphens] = handleHyphen(hyphen);

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

		copyAndAdvance(destination, hyphen, copyEnd);
	}
}
}
