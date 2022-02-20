#include <sqlite3.h>
#include <iostream>

const char *statementBuffer { R"foo(
SELECT
	issue.id,
	issue.summary,
	simple_issue_view.planned_start_date,
	simple_issue_view.planned_duration,
	issue.percentage_complete
FROM
simple_issue_view
INNER JOIN issue ON issue.id = simple_issue_view.id;
;
)foo"
};

class GanttHelper {
public:
	GanttHelper();
	~GanttHelper();
};

GanttHelper::GanttHelper() {
	std::cout 
		<< "@startgantt\n"
		<< "Project starts 2022-02-01\n";
}

GanttHelper::~GanttHelper() {
	std::cout << "@endgantt\n";
}

void handleRow(sqlite3_stmt *statement) 
{
	const unsigned char 
		*summary {sqlite3_column_text(statement, 1)},
		*planned_start_date {sqlite3_column_text(statement, 2)};
	int planned_duration {sqlite3_column_int(statement, 3)};

	printf("[%s] starts %s and lasts %d days\n", summary, planned_start_date, planned_duration);
}

int main()
{
	sqlite3* db;
	sqlite3_stmt* statement;

	sqlite3_open("database.sqlite3", &db);

	sqlite3_prepare_v3(db, statementBuffer, 2048, 0, &statement, NULL);

	GanttHelper g {};

	while ( true )
	{
		int result = sqlite3_step(statement);

		switch ( result )
		{
			case SQLITE_ROW:
			{
				handleRow(statement);
				break;
			}
			case SQLITE_BUSY:
			case SQLITE_MISUSE:
			case SQLITE_ERROR:
			default:
			{
				fprintf(stderr, "%s\n", sqlite3_errmsg(db));
				goto break_while;
			}
			case SQLITE_DONE:
				goto break_while;
		}

		continue;

break_while:
		break;
	}

	sqlite3_finalize(statement);

	sqlite3_close(db);
}
