#include "DescriptionGrabber.h"

#include <cstring>
#include <algorithm>

DescriptionGrabber::DescriptionGrabber(std::streambuf& source)
	: source(source)
{
	memset(buffer, 0, bufferSize);
}

void DescriptionGrabber::handleHyphen(char* hyphen)
{
	char *bufferEnd { buffer + bufferSize };

	if ( hyphen == bufferEnd )

		return;

	char *nonHyphen {
		std::find_if_not(hyphen + 1, bufferEnd, [] (char c) { return c == '-'; })
	};

	std::size_t additionalHyphens { nonHyphen - hyphen };

	if ( currentHyphenCount + additionalHyphens >= targetHyphens )
	{
		// Set the end of the streambuf to hyphen because we will be
		// needing to return EOF after that
		std::cerr << "We found the proper number of hyphens!\n";
		finished = true;
	}
	else if ( nonHyphen != bufferEnd )
	{
		// We have come across a series of insufficient hyphens, so we
		// will start our count anew
		std::cerr << "We didn't find enough hyphens there, so we are restarting the count\n";
		currentHyphenCount = 0;
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
	char *bufferEnd { buffer + bufferSize };

	while ( !finished )
	{
		std::streamsize charsRead { source.sgetn(buffer, bufferSize) };

		if ( charsRead == eof )
		{
			std::cerr << "We reached the end of file looking for the end of the description section!\n";
			exit(1);
		}

		char *hyphen { std::find(
			buffer, bufferEnd, '-'
		) };

		handleHyphen(hyphen);

		auto currentSize {target.size()};
		target.resize(target.size() + charsRead);
		std::memcpy(target.data() + currentSize, buffer, charsRead);
	}
}
