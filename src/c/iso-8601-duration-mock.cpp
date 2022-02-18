#include <sstream>
#include <iostream>

using pos_type = std::istringstream::pos_type;

class Suffixes {

public:
	constexpr Suffixes();
	
	unsigned int suffixes[256] {};
};

constexpr Suffixes::Suffixes() {
	suffixes['M'] = 30 * 24 * 3600;
	suffixes['W'] = 7 * 24 * 3600;
	suffixes['H'] = 3600;
}

constexpr Suffixes suffixes {};

pos_type simplePositionOrFail(std::istringstream& isstream) {
	pos_type position {isstream.tellg()};	

	if ( position == pos_type {-1} )
	{
		std::cerr << "Failed to tellg from isstream\n";
		exit(1);
	}

	return position;
}

void showCharSuffix(const char *input, std::istringstream& isstream) {
	pos_type position { simplePositionOrFail(isstream) };

	std::cerr << "Here is the char suffix: " << &input[position] << "\n";
}

unsigned int parseISO8601Duration(const char *input) {
	if ( input [0] != 'P' )
	{
		fprintf(stderr, "This duration does not start with 'P' and is thus obviously not an ISO8601 duration\n");
		fprintf(stderr, "%s\n", input);
		exit(1);
	}

	std::string inputAfterP { input + 1 };
	std::size_t stringLengthAfterP { inputAfterP.size() };
	std::istringstream isstream { std::move(inputAfterP) };
	unsigned int number {};
	char suffix {};

	isstream >> number;

	if ( isstream.fail() )
	{
		std::cerr << "Failed to read number from: " << input << "\n";
		exit(1);
	}

	isstream.get(suffix);

	if ( isstream.fail() )
	{
		std::cerr << "Failed to read char suffix from input: " << input << "\n";
		showCharSuffix(input, isstream);
		exit(1);
	}

	pos_type position {simplePositionOrFail(isstream)};

	if ( position != stringLengthAfterP )
	{
		std::cerr << "Junk at the end of string...: " << &input[1 + position] << "\n";
		std::cerr << "Ignoring junk and continuing\n";
	}

	if ( suffixes.suffixes[suffix] == 0 )
	{
		std::cerr << "Unknown suffix " << suffix << " in " << input << "\n";
	}

	return suffixes.suffixes[suffix] * number;
}
