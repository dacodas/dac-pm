#include "ConversionBindingWrangler.h"

#include <iostream>

#include "iso-8601-duration-mock.h"

namespace DacPM::Wranglers {

auto identity = [] (const std::string& value) -> Wrangler<const std::string&>::ConversionResult {
	return {value, Status::Success};
};

auto parseDuration = [] (const std::string& string) -> Wrangler<int>::ConversionResult {
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
