#include "StdinWithUngetStreambuf.h"

#include <cstring>
#include <algorithm>

namespace DacPM::VimParsing {

using int_type = StdinWithUngetStreambuf::int_type;
using char_type = StdinWithUngetStreambuf::char_type;

void StdinWithUngetStreambuf::checkIfEnd(std::streamsize read, std::streamsize desired) 
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
	ensureGetAreaEndWithBuffer();

	if ( atEnd )
	{
		return std::char_traits<char>::eof();
	}

	if ( gptr() == egptr() )
	{
		std::streamsize readChars {source.rdbuf()->sgetn(buffer, bufferSize)};

		if ( readChars == std::char_traits<char>::eof() )

			return readChars;

		checkIfEnd(readChars, bufferSize);

		setg(buffer, buffer, buffer + readChars);
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

	if ( atEnd )
	{
		return egptr() - gptr();
	}
	else 
	{
		// Move unread stuff to the beginning of the buffer, and then read as
		// much as we need
		std::streamsize 
			unreadSize {buffer + bufferSize - gptr()},
			toReadSize {bufferSize - unreadSize},
			actualReadSize;
			
		std::memmove(buffer, gptr(), unreadSize);
		actualReadSize = source.rdbuf()->sgetn(gptr(), toReadSize);
		checkIfEnd(actualReadSize, toReadSize);

		std::streamsize newBufferSize {unreadSize + actualReadSize};

		setg(buffer, buffer, buffer + newBufferSize);

		return newBufferSize;
	}
}

StdinWithUngetStreambuf::StdinWithUngetStreambuf(std::istream& source)
	: source {source}
{
	std::streamsize readSize {source.rdbuf()->sgetn(buffer, bufferSize)};

	checkIfEnd(readSize, bufferSize);	

	setg(buffer, buffer, buffer + readSize);
}

char_type* StdinWithUngetStreambuf::eback() const { return std::streambuf::eback(); }
char_type* StdinWithUngetStreambuf::gptr() const { return std::streambuf::gptr(); }
char_type* StdinWithUngetStreambuf::egptr() const { return std::streambuf::egptr(); }

int_type StdinWithUngetStreambuf::sungetc() { 
	std::cerr << "sungetc\n";

	if ( gptr() > eback() )
	{
		--_M_in_cur;
		return std::char_traits<char>::to_int_type(*gptr());
	}
	else
		return std::char_traits<char>::eof();
}

void StdinWithUngetStreambuf::advanceTo(char *newGptr) { 
	if ( newGptr > egptr() or newGptr < eback() )
	{
		std::cerr << "You have asked me to advance somewhere nulle!\n";
		exit(1);
	}

	_M_in_cur = newGptr;
}

BufferedSentry::BufferedSentry(StdinWithUngetStreambuf& streambuf)
{
	char* nonWhitespace {std::find_if_not(
		streambuf.gptr(),
		streambuf.egptr(),
		[] (char c) { return std::isspace(c); }
	)};

	streambuf.advanceTo(nonWhitespace);
}

}

