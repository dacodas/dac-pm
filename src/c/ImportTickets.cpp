#include "TicketFromVim.h"

int main(int argc, char *argv[])
{
	TicketFromVim ticket(std::cin);

	std::cout << ticket << "\n";

	return 0;
}
