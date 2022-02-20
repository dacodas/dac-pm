#include "StdinWithUngetStreambuf.h"

#include <cstring>
#include <algorithm>

namespace DacPM::VimParsing {

using int_type = StdinWithUngetStreambuf::int_type;
using char_type = StdinWithUngetStreambuf::char_type;

BufferedSentry::BufferedSentry(StdinWithUngetStreambuf& streambuf)
{
	char* nonWhitespace {std::find_if_not(
		streambuf.gptr(),
		streambuf.egptr(),
		[] (char c) { return std::isspace(c); }
	)};

	streambuf.advanceTo(nonWhitespace);
}

StdinWithUngetStreambuf::StdinWithUngetStreambuf(std::istream& source)
	: source {source}
{
	std::streamsize readSize {source.rdbuf()->sgetn(buffer, bufferSize)};

	checkIfExhaustedUnderlyingBuffer(readSize, bufferSize);

	setg(buffer, buffer, buffer + readSize);
}

char_type* StdinWithUngetStreambuf::eback() const { return std::streambuf::eback(); }
char_type* StdinWithUngetStreambuf::gptr() const { return std::streambuf::gptr(); }
char_type* StdinWithUngetStreambuf::egptr() const { return std::streambuf::egptr(); }

void StdinWithUngetStreambuf::advanceTo(char *newGptr) {
	if ( newGptr > egptr() or newGptr < eback() )
	{
		std::cerr << "You have asked me to advance somewhere nulle!\n";
		exit(1);
	}

	_M_in_cur = newGptr;
}

void StdinWithUngetStreambuf::checkIfExhaustedUnderlyingBuffer(std::streamsize read, std::streamsize desired) 
{
	if ( read < desired )
	{
		atEnd = true;
	}
}

void StdinWithUngetStreambuf::ensureGetAreaEndWithBuffer() 
{
	if ( atEnd )
		return;

	if ( egptr() != ( buffer + bufferSize ) )
	{
		std::cerr << "Why is the get area pointer not the end of the buffer?\n";
		exit(1);
	}
}

int_type StdinWithUngetStreambuf::underflow()
{
	auto eof {std::char_traits<char>::eof()};

	ensureGetAreaEndWithBuffer();

	if ( atEnd )
	{
		return eof;
	}

	if ( gptr() == egptr() )
	{
		std::streamsize readChars {source.rdbuf()->sgetn(buffer, bufferSize)};

		if ( readChars == eof )

			return eof;

		setg(buffer, buffer, buffer + readChars);

		checkIfExhaustedUnderlyingBuffer(readChars, bufferSize);
	}
}

std::streamsize StdinWithUngetStreambuf::xsgetn(char_type *nonceBuffer, std::streamsize count)
{
	if ( nonceBuffer != nullptr )
	{
		std::cerr << "Then pay with your blood!\n";
		std::cerr << "This class is meant specifically for use with the DescriptionGrabber, and you are using it wrong\n";
		exit(1);
	}

	if ( count > bufferSize )
	{
		std::cerr << "We tried to grab more than our buffered input could handle\n";
		exit(1);
	}

	underflow();

	if ( !atEnd )
	{
		// Move unread stuff to the beginning of the buffer, and then read as
		// much as we need
		std::streamsize 
			unreadSize {buffer + bufferSize - gptr()},
			toReadSize {bufferSize - unreadSize},
			actualReadSize;
			
		std::memmove(buffer, gptr(), unreadSize);
		actualReadSize = source.rdbuf()->sgetn(gptr(), toReadSize);

		setg(buffer, buffer, buffer + unreadSize + actualReadSize);

		checkIfExhaustedUnderlyingBuffer(actualReadSize, toReadSize);
	}

	return egptr() - gptr();
}

}

