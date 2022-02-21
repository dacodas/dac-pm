#include <string_view>
#include <unordered_set>
#include <vector>
#include <string>

// https://stackoverflow.com/a/64101153/118269000

struct TransparentStringEqual {
    using is_transparent = std::true_type ;

    bool operator()(std::string_view l, std::string_view r) const noexcept
    {
        return l == r;
    }
};

struct TransparentStringHash {
    using is_transparent = std::true_type ;

    auto operator()(std::string_view str) const noexcept {
        return std::hash<std::string_view>()(str);
    }
};

using RelationSlugs = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

class RelationsGrabber {
public:
	enum class Status {
		Invalid,
		Okay
	};

	using enum Status;

	struct RelationSlugAndIssue {
		std::string_view RelationSlug;
		std::string_view Issue;
	};

	struct Return {
		std::vector<RelationSlugAndIssue> relations;
		std::string::iterator newEndOfIssueDescription;
		Status status;
	};

	RelationsGrabber(std::string& descriptionAndRelations, RelationSlugs& relationSlugs);

	Return grab();

private:
	std::string& descriptionAndRelations;
	RelationSlugs& relationSlugs;

	struct LineReturn {
		RelationSlugAndIssue relation;
		Status status;
	};

	constexpr static LineReturn InvalidLineReturn {
		{}, Invalid
	};

	const static Return InvalidGrabberReturn;
	LineReturn relationLine(std::string_view);
};

