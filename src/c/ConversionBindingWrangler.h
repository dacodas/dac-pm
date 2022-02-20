#pragma once

#include <variant>
#include <vector>
#include <string>
#include <sqlite3.h>

// I expect this class to help us go from the std::vector<std::string> to
// something much closer to what we'll be putting in the database

// sqlite3 can already handle the ISO 8601 with:
//
// select strftime('%s', '2022-02-18 10:00');

// We already have the example parser for the durations that we improve later

// We will be using different functions to transform and bind each argument, so
// that must be accounted for

namespace DacPM::Wranglers {

enum class Status {
	Error,
	Success
};

template <typename Value>
struct Wrangler {
	struct ConversionResult {
		Value converted;
		Status status;
	};

	using ConversionFunction = ConversionResult (*)(const std::string&);
	using BindFunction = int (*)(sqlite3_stmt *, std::size_t, Value);

	ConversionFunction conversion;
	BindFunction bind;
};

using WranglerUnion = 
	std::variant<
		Wrangler<int>,
		Wrangler<const std::string&>
	>;

extern std::vector<WranglerUnion> wranglers;

template <typename Wrangler>
int wrangle(sqlite3_stmt *statement, Wrangler& wrangler, const std::string& field, std::size_t index);

int bind(sqlite3_stmt *statement, const std::vector<std::string>& fields, std::vector<WranglerUnion>& wranglers);
}

