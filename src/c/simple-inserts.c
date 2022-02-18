#include <sqlite3.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

const char *statementBuffer = 
"INSERT INTO issue ( summary, application, type, assignee, description )\n"
"SELECT input.summary, application.id, type.id, assignee.id, input.description\n"
"FROM\n"
"	(\n"
"		SELECT\n"
"			? AS summary,\n"
"			? AS application,\n"
"			? AS type,\n"
"			? AS assignee,\n"
"			? AS description\n"
"	) AS input\n"
"INNER JOIN application AS application ON application.application = input.application\n"
"INNER JOIN issue_type AS type ON type.type = input.type\n"
"INNER JOIN person AS assignee ON assignee.name = input.assignee\n"
";\n"
;

int main(int argc, char* argv[])
{
	sqlite3* db;
	sqlite3_stmt* statement;

	if ( argc != 6 )
	{
		fprintf(stderr, "Something is clearly wrong, you need to specify 5 arguments\n");
		fprintf(stderr, "The SQL command is as follows:\n\n");
		fprintf(stderr, "%s", statementBuffer);
		exit(1);
	}

	sqlite3_open("database.sqlite3", &db);

	sqlite3_prepare_v3(db, statementBuffer, 2048, 0, &statement, NULL);

	for ( size_t index = 1; index < argc ; ++index )
	{
		sqlite3_bind_text(statement, index, argv[index], -1, SQLITE_STATIC);
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

	sqlite3_finalize(statement);

	sqlite3_close(db);

	return 0;
}
