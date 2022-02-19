#include "TicketFromVim.h"
#include "StdinWithUngetStreambuf.h"
#include "DescriptionGrabber.h"

std::vector<TicketFromVim> readTickets(std::istream& istream) 
{
	DacPM::VimParsing::StdinWithUngetStreambuf streambuf {istream};
	std::istream bufferedIstream {&streambuf};

	std::vector<TicketFromVim> tickets {};

	for (;;)
	{
		DacPM::VimParsing::BufferedSentry sentry(streambuf);

		TicketFromVim ticket {bufferedIstream, streambuf};

		if ( !ticket )
		{
			break;
		}

		tickets.emplace_back(std::move(ticket));
	}

	for ( auto& ticket : tickets )

		std::cout << ticket;

	return tickets;
}

TicketFromVim::TicketFromVim(std::istream& bufferedIstream, DacPM::VimParsing::StdinWithUngetStreambuf& streambuf)
	: fields(FieldsCount) 
{
	size_t index {0};

	for ( ; index < FieldsCount - 1 ; ++index )
	{
		std::getline(bufferedIstream, fields[index]);

		if ( bufferedIstream.fail() )
		{
			std::cerr << "Failed to read field " << FieldNames[index] << "\n";
			return;
		}
	}

	DacPM::VimParsing::DescriptionGrabber grabber {streambuf};
	grabber.grab(fields[index]);

	ok = true;
}

TicketFromVim::operator bool() const
{
	return ok;
}

std::ostream& operator<<(std::ostream& output, const TicketFromVim& ticket)
{
	for ( size_t index {0} ; index < TicketFromVim::FieldsCount ; ++index )
	{
		std::cout << TicketFromVim::FieldNames[index] << " : " << ticket.fields[index] << "\n";
	}

	return output;
}
