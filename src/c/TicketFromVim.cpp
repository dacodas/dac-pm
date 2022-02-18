#include "TicketFromVim.h"
#include <cstring>
#include <algorithm>
#include <sstream>
#include <locale>

struct CtypesFacet : std::ctype<char>
{
	static const mask *prepareTable() {
		static std::vector<mask> table(classic_table(), classic_table() + table_size);
		for ( auto& mask : table )
			mask &= ~space;

		return &table[0];
	}

	CtypesFacet() : ctype(prepareTable(), false, 0) {}
};

class FilteringStreambuf : public std::streambuf
{
	static constexpr std::size_t bufferSize {1024};
	static constexpr std::size_t targetHyphens {3};

	std::streambuf& source;
	char buffer[bufferSize];
	size_t serialHyphens {0};
	bool foundHyphens {false};
	bool lookingForHyphens {false};

public:
	FilteringStreambuf(std::streambuf&);

	int underflow();
	std::streambuf* rdbuf();
	void toggleHyphenSearching();

private:
	void handleHyphen(char* hyphen);
};


void FilteringStreambuf::toggleHyphenSearching() {
	lookingForHyphens = !lookingForHyphens;
	foundHyphens = false;
}

std::streambuf* FilteringStreambuf::rdbuf() {
	return &source;
}

FilteringStreambuf::FilteringStreambuf(std::streambuf& source)
	: source(source)
{
	memset(buffer, 0, bufferSize);
}

void FilteringStreambuf::handleHyphen(char* hyphen)
{
	char *bufferEnd { buffer + bufferSize };
	auto eof {std::char_traits<char>::eof()};

	char *nonHyphen {
		std::find_if_not(hyphen + 1, bufferEnd, [] (char c) { return c == '-'; })
	};

	std::size_t additionalHyphens { nonHyphen - hyphen };

	if ( serialHyphens + additionalHyphens >= targetHyphens )
	{
		// Set the end of the streambuf to hyphen because we will be
		// needing to return EOF after that
		std::cerr << "We found the proper number of hyphens!\n";
		foundHyphens = true;

		setg(buffer, gptr(), nonHyphen);
	}
	else if ( nonHyphen != bufferEnd )
	{
		// We have come across a series of insufficient hyphens, so we
		// will start our count anew
		std::cerr << "We didn't find enough hyphens there, so we are restarting the count\n";
		serialHyphens = 0;
	}
	else
	{
		// We don't have enough hyphens, and we are split amongst two
		// buffers
		std::cerr << "We are split amongst two buffers, and I am sad\n";
		exit(1);
	}
}

int FilteringStreambuf::underflow()
{
	auto eof {std::char_traits<char>::eof()};
	char *bufferEnd { buffer + bufferSize };

	if ( gptr() == egptr() )
	{
		std::streamsize charsRead { source.sgetn(buffer, bufferSize) };

		if ( charsRead == eof )
		{
			return eof;
		}

		setg(buffer, buffer, buffer + charsRead);
	}

	if ( lookingForHyphens and foundHyphens )
	{
		return eof;
	}

	else if ( lookingForHyphens )
	{
		char *hyphen { std::find(
			buffer, bufferEnd, '-'
		) };

		if ( hyphen != bufferEnd )
		{
			handleHyphen(hyphen);
		}
	}

	return *gptr();
}

TicketFromVim::TicketFromVim(std::istream& istream)
	: fields(FieldsCount)
{
	size_t index {0};

	FilteringStreambuf streambuf { *istream.rdbuf() };

	for ( ; index < FieldsCount - 1 ; ++index )
	{
		std::getline(istream, fields[index]);

		if ( istream.fail() )
		{
			std::cerr << "Failed to read field " << FieldNames[index] << "\n";
			exit(1);
		}
	}

	std::locale locale(std::locale(), new CtypesFacet());
	istream.rdbuf(&streambuf);
	istream.imbue(locale);

	streambuf.toggleHyphenSearching();
	istream >> fields[index];
}

std::ostream& operator<<(std::ostream& output, const TicketFromVim& ticket)
{
	for ( size_t index {0} ; index < TicketFromVim::FieldsCount ; ++index )
	{
		std::cout << TicketFromVim::FieldNames[index] << " : " << ticket.fields[index] << "\n";
	}

	return output;
}
