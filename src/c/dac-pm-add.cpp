// I expect this class to help us go from the std::vector<std::string> to
// something much closer to what we'll be putting in the database

// sqlite3 can already handle the ISO 8601 with:
//
// select strftime('%s', '2022-02-18 10:00');

// We already have the example parser for the durations that we improve later

// We will be using different functions to transform and bind each argument, so
// that must be accounted for

#include <cstddef>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <variant>
#include <vector>

#include <sqlite3.h>

#include "TicketFromVim.h"
#include "iso-8601-duration-mock.h"

namespace DacPM::Wranglers {

enum class Status {
	Error,
	Success
};

template <typename Value>
struct ConversionResult {
	Value converted;
	Status status;
};

template <typename Value>
using ConversionFunction = ConversionResult<Value> (*)(const std::string&);

template <typename Value>
using BindFunction = int (*)(sqlite3_stmt *, std::size_t, Value);

template <typename Value>
struct Wrangler {
	ConversionFunction<Value> conversion;
	BindFunction<Value> bind;
};

using WranglerUnion = 
	std::variant<
		Wrangler<int>,
		Wrangler<const std::string&>
	>;

auto identity = [] (const std::string& value) -> ConversionResult<const std::string&> {
	return {value, Status::Success};
};

auto parseDuration = [] (const std::string& string) -> ConversionResult<int> {
	return {parseISO8601Duration(string.data()), Status::Success};
};

auto bindString = [] (sqlite3_stmt *statement, std::size_t index, const std::string& value) -> int {
	return sqlite3_bind_text(statement, index, value.data(), value.size(), SQLITE_STATIC);
};

auto bindInt = [] (sqlite3_stmt *statement, std::size_t index, int value) -> int {
	return sqlite3_bind_int(statement, index, value);
};

std::vector<WranglerUnion> wranglers {
	Wrangler<const std::string&> { identity, bindString },
	Wrangler<const std::string&> { identity, bindString },
	Wrangler<const std::string&> { identity, bindString },
	Wrangler<const std::string&> { identity, bindString },
	Wrangler<const std::string&> { identity, bindString },
	Wrangler<int> { parseDuration, bindInt },
	Wrangler<int> { parseDuration, bindInt },
	Wrangler<const std::string&> { identity, bindString},
	Wrangler<const std::string&> { identity, bindString },
	Wrangler<const std::string&> { identity, bindString }
};

template <typename Wrangler>
int wrangle(sqlite3_stmt *statement, Wrangler& wrangler, const std::string& field, std::size_t index) 
{
	auto& [conversion, bind] = wrangler;

	auto&& [converted, status] { conversion(field) };

	if ( status != Status::Success )
	{
		std::cerr << "Failed converting field '" << field << "'\n";
		exit(1);
	}

	return bind(statement, index + 1, converted);
}

int bind(sqlite3_stmt *statement, const std::vector<std::string>& fields, std::vector<WranglerUnion>& wranglers)
{
	for ( std::size_t index {0} ; index < wranglers.size() ; ++index )
	{
		WranglerUnion& wrangler {wranglers[index]};

		auto result = std::visit([statement, index, &fields](auto& wrangler) -> int {
			return wrangle(statement, wrangler, fields[index], index);
		}, wrangler);

		if ( result != SQLITE_OK )

			return result;
	} 

	return SQLITE_OK;
}

}

// Another option to explore here to do these all in a single statement is to
// generate a proper string of `(?, ?, ?, ?, ?)` and pass them as VALUES. The
// main thing to worry about would be the column names.
const char *statementBuffer = R"sql(

INSERT INTO issue (
	summary,
	application,
	type,
	assignee,
	planned_start_date,
	planned_duration,
	planned_effort,
	percentage_complete,
	status,
	description
)
SELECT
	input.summary,
	application.id,
	type.id,
	assignee.id,
	input.planned_start_date,
	input.planned_duration,
	input.planned_effort,
	input.percentage_complete,
	status.id,
	input.description
FROM
	(
		SELECT
			? AS summary,
			? AS application,
			? AS type,
			? AS assignee,
			strftime('%s', ?) AS planned_start_date,
			? AS planned_duration,
			? AS planned_effort,
			? AS percentage_complete,
			? AS status,
			? AS description
	) AS input
INNER JOIN application AS application ON application.application = input.application
INNER JOIN issue_type AS type ON type.type = input.type
INNER JOIN person AS assignee ON assignee.name = input.assignee
INNER JOIN issue_status AS status ON status.name = input.status
;

)sql"
;

void commitBatchOfTickets(std::vector<TicketFromVim>& tickets)
{
	sqlite3* db;
	sqlite3_stmt* statement;

	sqlite3_open("database.sqlite3", &db);

	sqlite3_prepare_v3(db, statementBuffer, 2048, 0, &statement, NULL);

	sqlite3_exec(db, "BEGIN", 0, 0, 0);

	for ( auto& ticket : tickets )
	{
		int bind_result = DacPM::Wranglers::bind(statement, ticket.asFields(), DacPM::Wranglers::wranglers);
		switch ( bind_result )
		{
			case SQLITE_OK:
				std::cout << "Success binding!\n";
				break;
			default:
				std::cout << "Failure binding...\n";
				fprintf(stderr, "%s\n", sqlite3_errmsg(db));
				exit(1);
				break;
		}

		int result = sqlite3_step(statement);
		switch ( result )
		{
			case SQLITE_DONE:
			{
				sqlite3_reset(statement);
				sqlite3_clear_bindings(statement);
				break;
			}
			case SQLITE_BUSY:
			case SQLITE_MISUSE:
			case SQLITE_ERROR:
			default:
			{
				fprintf(stderr, "%s\n", sqlite3_errmsg(db));
				exit(1);
			}
		}
	}

	sqlite3_finalize(statement);

	sqlite3_exec(db, "END", 0, 0, 0);

	sqlite3_close(db);
}

int main(int argc, char* argv[])
{
	std::vector<TicketFromVim> tickets { readTickets(std::cin) }; 
	commitBatchOfTickets(tickets);

	return 0;
}
