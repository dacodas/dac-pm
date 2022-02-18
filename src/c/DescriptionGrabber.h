#include <iostream>

class DescriptionGrabber
{
	static constexpr std::size_t bufferSize {10 * 1024};
	static constexpr std::size_t targetHyphens {3};

	std::streambuf& source;
	char buffer[bufferSize];
	size_t currentHyphenCount {0};
	bool finished {false};

public:
	DescriptionGrabber(std::streambuf&);
	void grab(std::string&);

private:
	char *handleHyphen(char* hyphen);
};

