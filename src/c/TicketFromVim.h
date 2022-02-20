#pragma once

#include <iostream>
#include <vector>

namespace DacPM::VimParsing {
	class SharingBufferedStdinStreambuf;
}

class TicketFromVim {
public:
	enum class Fields {
		summary,
		application,
		type,
		assignee,
		planned_start_date,
		planned_duration,
		planned_effort,
		percentage_complete,
		status,
		description,
		FieldsCount
	};

	static constexpr std::size_t FieldsCount {
		static_cast<size_t>( Fields::FieldsCount )
	};

	static constexpr const char *FieldNames[FieldsCount] {
		"summary",
		"application",
		"type",
		"assignee",
		"planned_start_date",
		"planned_duration",
		"planned_effort",
		"percentage_complete",
		"status",
		"description"
	};

	TicketFromVim(std::istream& bufferedIstream, DacPM::VimParsing::SharingBufferedStdinStreambuf& streambuf);
	operator bool() const;

	friend std::ostream& operator<<(std::ostream& output, const TicketFromVim& ticket);
	const std::vector<std::string>& asFields() const;

private:
	std::vector<std::string> fields;
	bool ok {false};
};

std::ostream& operator<<(std::ostream& output, const TicketFromVim& ticket);

std::vector<TicketFromVim> readTickets(std::istream& istream);
