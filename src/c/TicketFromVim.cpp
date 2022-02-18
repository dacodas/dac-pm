#include "TicketFromVim.h"
#include "StdinWithUngetStreambuf.h"
#include "DescriptionGrabber.h"

TicketFromVim::TicketFromVim(std::istream& istream)
	: fields(FieldsCount)
{
	DacPM::VimParsing::StdinWithUngetStreambuf streambuf {istream};
	std::istream bufferedIstream {&streambuf};

	size_t index {0};

	for ( ; index < FieldsCount - 1 ; ++index )
	{
		std::getline(bufferedIstream, fields[index]);

		if ( bufferedIstream.fail() )
		{
			std::cerr << "Failed to read field " << FieldNames[index] << "\n";
			exit(1);
		}
	}

	DacPM::VimParsing::DescriptionGrabber grabber {streambuf, exposeBuffer(streambuf)};
	grabber.grab(fields[index]);
}

std::ostream& operator<<(std::ostream& output, const TicketFromVim& ticket)
{
	for ( size_t index {0} ; index < TicketFromVim::FieldsCount ; ++index )
	{
		std::cout << TicketFromVim::FieldNames[index] << " : " << ticket.fields[index] << "\n";
	}

	return output;
}
