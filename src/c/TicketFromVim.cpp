#include "TicketFromVim.h"
#include "DescriptionGrabber.h"

TicketFromVim::TicketFromVim(std::istream& istream)
	: fields(FieldsCount)
{
	size_t index {0};

	for ( ; index < FieldsCount - 1 ; ++index )
	{
		std::getline(istream, fields[index]);

		if ( istream.fail() )
		{
			std::cerr << "Failed to read field " << FieldNames[index] << "\n";
			exit(1);
		}
	}

	DescriptionGrabber grabber {*std::cin.rdbuf()};
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
