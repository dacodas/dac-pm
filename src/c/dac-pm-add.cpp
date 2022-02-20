#include <cstddef>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "TicketFromVim.h"
#include "ConversionBindingWrangler.h"

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
				break;
			default:
				std::cerr << "Failure binding...\n";
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
