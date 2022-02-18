#include <iostream>
#include <vector>

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

	TicketFromVim(std::istream& istream);

	friend std::ostream& operator<<(std::ostream& output, const TicketFromVim& ticket);

private:
	std::vector<std::string> fields;
};

std::ostream& operator<<(std::ostream& output, const TicketFromVim& ticket);
