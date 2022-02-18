#include <sqlite3.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	sqlite3* db;
	sqlite3_stmt* statement;

	char statementBuffer[256];
	const char* table = argv[1];
	snprintf(statementBuffer, 256, "INSERT INTO confluence_%s (%s) VALUES (?);", table, table);

	sqlite3_open("test.db", &db);

	sqlite3_prepare_v3(db, statementBuffer, 256, 0, &statement, NULL);

	for ( size_t index = 2; index < argc ; ++index )
	{
		sqlite3_bind_text(statement, 1, argv[index], -1, SQLITE_STATIC);
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

	sqlite3_close(db);

	return 0;
}
