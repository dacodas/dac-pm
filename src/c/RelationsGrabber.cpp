#include "RelationsGrabber.h"

#include <algorithm>

#include <iostream>

static char *findParagraphStart(char *begin, char *end)
{
	char *c {begin};

	while ( c != end )
	{
		if ( *( c - 1 ) == '\n' and *( c - 2 ) == '\n' )

			return c;

		--c;
	}

	return end;
}

const RelationsGrabber::Return RelationsGrabber::InvalidGrabberReturn {
	{}, {}, Invalid
};

RelationsGrabber::RelationsGrabber(std::string& descriptionAndRelations, RelationSlugs& relationSlugs)
	: descriptionAndRelations {descriptionAndRelations}, relationSlugs {relationSlugs} {}

// The off-by-one errors have a potential to be hazardous here
RelationsGrabber::Return RelationsGrabber::grab()
{
	using enum Status;

	std::vector<RelationSlugAndIssue> relations {};

	if ( descriptionAndRelations.size() < 3 )
	{
		std::cerr << "This is too small to possibly contain a paragraph\n";
		return InvalidGrabberReturn;
	}

	char *safeEndForDereferencing {
		&*( descriptionAndRelations.begin() + 2 )
	};

	char *mostRecentParagraphBegin = findParagraphStart(&*--descriptionAndRelations.end(), safeEndForDereferencing);

	if ( mostRecentParagraphBegin == safeEndForDereferencing )
	{
		std::cerr << "Did not find a full paragraph!\n";
		return InvalidGrabberReturn;
	}

	std::string::iterator 
		mostRecentParagraphBeginAsIterator {
			descriptionAndRelations.begin() + ( mostRecentParagraphBegin - descriptionAndRelations.c_str() )
		},
		lineBegin { mostRecentParagraphBeginAsIterator },
		lineEnd {},
		descriptionEnd { descriptionAndRelations.end() }
		;

	for ( ; lineBegin < descriptionEnd ; lineBegin = lineEnd + 1 )
	{
		lineEnd = std::find(lineBegin, descriptionAndRelations.end(), '\n');

		if ( lineEnd == descriptionAndRelations.end() )
		{
			std::cerr << "Didn't find complete line in last paragraph of description\n";
			return InvalidGrabberReturn;
		}

		auto [relation, status] = relationLine({lineBegin, lineEnd});
		switch ( status )
		{
			case Invalid:
				return InvalidGrabberReturn;
			default:
				relations.push_back(relation);
				break;
		}
	}

	return {
		std::move(relations),
		mostRecentParagraphBeginAsIterator - 2,
		Okay
	};
}

RelationsGrabber::LineReturn RelationsGrabber::relationLine(std::string_view line)
{
	// Since summaries are limited to a single line, we know that Relations
	// are also limited to a single line.
	// 
	// Because of this, we are able to say that only the last "paragraph"
	// is allowed to be relations

	auto openingApostrophe {std::find(line.begin(), line.end(), '\'')};

	if ( openingApostrophe == line.end() )
	{
		std::cerr << "Did not find opening apostrophe in relations section\n";
		return InvalidLineReturn;
	}

	if ( openingApostrophe == line.begin() )
	{
		std::cerr << "There is no relation slug here!\n";
		return InvalidLineReturn;
	}

	if ( *( openingApostrophe - 1 ) != ' ' )
	{
		std::cerr << "No space before the opening apostrophe!\n";
		return InvalidLineReturn;
	}

	std::string_view relationSlug { line.begin(), openingApostrophe - 1 };
	if ( !relationSlugs.contains( relationSlug ) )
	{
		std::cerr << "Unknown relation\n";
		return InvalidLineReturn;
	}

	if ( *( line.end() - 1 ) != '\'' )
	{
		std::cerr << "Line does not end with apostrophe!\n";
		return InvalidLineReturn;
	}

	std::string_view issueSummary { openingApostrophe + 1, line.end() - 1 };

	return {
		{ relationSlug, issueSummary }, Okay
	};
}
