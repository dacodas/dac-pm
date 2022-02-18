#include <vector>
#include <iostream>

#include "iso-8601-duration-mock.h"

struct Test {
	const char* input;
	unsigned int output;
};

// Right of the bat, I think I see an interesting problem. We only store second
// in the database, but the ISO 8601 doesn't (I think) apply an actual value to
// a month. I think you carry around the duration, and then it is contextual
// based on what you apply it to.
// 
// We will cheapen this by assuming a month is 30 days
// 
// Also, these unsigned int would be more legible if they used std::chrono or
// ::date

std::vector<Test> tests {
	{"P100H", 100 * 3600},
	{"P16H", 16 * 3600},
	{"P18H", 18 * 3600},
	{"P1M", 30 * 24 * 3600},
	{"P1W", 7 * 24 * 3600},
	{"P2H", 2 * 3600},
	{"P2M", 2 * 30 * 24 * 3600},
	{"P2W", 2 * 7 * 24 * 3600},
	{"P36H", 36 * 3600},
	{"P3M", 3 * 30 * 24 * 3600},
	{"P4H", 4 * 3600},
	{"P8H", 8 * 3600}
};

int main(int argc, char *argv[]) {
	for ( auto [input, expectedResult] : tests )
	{
		unsigned int result { parseISO8601Duration(input) };

		if ( result != expectedResult )
		{
			std::cerr << "FAIL: ";	
		}
		else 
		{
			std::cerr << "PASS: ";
		}

		std::cerr << "Received " << result << " and was expecting " << expectedResult << "\n";
	}
}
