#include "StdinWithUngetStreambuf.h"

#include <cstring>

namespace DacPM::VimParsing {

using int_type = StdinWithUngetStreambuf::int_type;

char *exposeBuffer(StdinWithUngetStreambuf& streambuf)
{
	return streambuf.buffer;
}

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

	if ( gptr() == egptr() )
	{
		std::streamsize readChars {source.rdbuf()->sgetn(buffer, bufferSize)};

		if ( readChars == std::char_traits<char>::eof() )

			return readChars;

		checkIfEnd(readChars, bufferSize);

		setg(buffer, buffer, buffer + readChars);
	}
}

std::streamsize StdinWithUngetStreambuf::xsgetn(char_type *buffer, std::streamsize count)
{
	if ( count > bufferSize )
	{
		// Fail if we requrestest more than we can store in our buffer
		std::cerr << "We tried to grab more than our buffered input could handle\n";
		exit(1);
	}

	if ( count < ( egptr() - gptr() ) )
	{
		
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
}
