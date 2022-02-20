# Overview

- How about issues that touch multiple applications?
  - Primary application and allow for secondaries in another table?
  - Generate all combinations? 🤣
- Consider date and duration pickers and how to represent them in the buffer
  - Consider ISO 8601 duration standard
  - Also consider [this excellent library][howard-date]
- Add a field for "time actually completed" 
- Add integrity check for some things
  - If percentage is 100%, then we need to be in one of the "Finished" status
    categories
  - Make sure "time actually completed" is properly set if percentage is 100%
  - If there is a Gantt like or parent-child relation between two issues, we
    can constrain the proper values for the start, end, duration, and effort
    values
  - Some issue types might even have these values entirely computed from their
    children
  - Same thing for percentages, some percentages should be computed from their
    children, and I am unsure how to calculate this
- Have a date for when the ticket is actually created
- What to do about this interactive case where I am adding a lot of tasks?
  - For example, I want to have relationships between tasks
    1. What format should it be in the user interface when specifying these
       tasks?
    2. How can I create links between issues that are in this same batch and
       haven't been added yet?
       - Consider having a temporary holding table for the current batch, where
	 links can be established...
	 - IDs would change as we went along, so I'm not sure how this would
	   work...
	 - We could still have a temporary table, and rely on linking via
	   regexes that uniquely identify the summary of each issue... I think
	   something like this is the most straightforward way for now
	 - On second thought, I think it is reasonable to have the verbatim
	   summary in the batch relation
    3. One thing is determining how to establish the database link between the
       two. Another is how to allow referencing others in the batch in the
       description of the issue.

       Lately I have been partial to Markdown's "reference-like" link syntax. I
       wonder if relations can be specified at the end in a reference-like
       syntax, and referred to in the descriptions to be modified before
       insertion into the database.

## Parsing the description with iostreams

- We need different adapters for the different parts of the syntax
  - Description which looks out for hyphens
  - Potentially something for relations/links
  - Single line for everything else
- The streambufs can share the same buffer, but how do we switch back and
  forth?
  - Or do we make a god object?
- I want to pivot with how I am conceptualizing this. I think we can... I think
  the idea of sending back EOFs is causing a bit of trouble and havoc
  - Instead, let's... well, let's just skip over those bits by setting the
    current char and go from there. I think that will get us pretty good...
  - Alright, we will simply try to understand the mental model of
    `std::streambuf` and implement the virtual methods faithfully

- Okay, we're very close to being finished here...
  - I need to be able to push back chars...
    - This suggests that I will have to wrap cin before even having VimTicket
      start grabbing things

- I can't help but feel that these streambuf buffers can be shared

# Notes regarding the editing experience

```
+-----+---------------------+---------------+---------+------------+----+
| cid |        name         |     type      | notnull | dflt_value | pk |
+-----+---------------------+---------------+---------+------------+----+
| 0   | id                  | INTEGER       | 0       |            | 1  |
| 1   | type                | INTEGER       | 0       |            | 0  |
| 2   | application         | INTEGER       | 0       |            | 0  |
| 3   | assignee            | INTEGER       | 0       |            | 0  |
| 4   | summary             | TEXT          | 0       |            | 0  |
| 5   | description         | TEXT          | 0       |            | 0  |
| 6   | planned_start_date  | INTEGER       | 1       | 1645776000 | 0  |
| 7   | planned_duration    | INTEGER       | 1       | 259200     | 0  |
| 8   | planned_effort      | INTEGER       | 1       | 28800      | 0  |
| 9   | percentage_complete | DECIMAL(3, 0) | 1       | 0          | 0  |
| 10  | status              | INTEGER       | 1       | 1          | 0  |
+-----+---------------------+---------------+---------+------------+----+

summary
application
type
assignee
planned_start_date
planned_duration
planned_effort
percentage_complete
status
description
```

What ought the template look like?

```
<Summary>
<Application>
<Type>
Dacoda Strack
echo $( date -d Tomorrow '+%Y-%m-%d 10:00' )
P2W
P4H
0
<Status>
<Description>
```

[howard-date]: https://github.com/HowardHinnant/date
[howard-date-durations]: https://howardhinnant.github.io/date/date.html#duration_io

[^howard-date]: Note the output of the command here

```
(dacoda@dacoda-omen ~ λ) dnf info --cacheonly date-devel.x86_64
Last metadata expiration check: 0:19:15 ago on Thu 17 Feb 2022 09:02:12 PM PST.
Available Packages
Name         : date-devel
Version      : 3.0.1^20210518git052eeba
Release      : 2.fc35
Architecture : x86_64
Size         : 48 k
Source       : date-3.0.1^20210518git052eeba-2.fc35.src.rpm
Repository   : fedora
Summary      : Date and time library based on the C++11/14/17 <chrono> header
URL          : https://github.com/HowardHinnant/date
License      : MIT
Description  : This is actually several separate C++11/C++14/C++17 libraries:
             :  - "date.h" is a header-only library which builds upon <chrono>.
             :    It adds some new duration types, and new time_point types. It
             :    also adds "field" types such as year_month_day which is a
             :    struct {year, month, day}. And it provides convenient means
             :    to convert between the "field" types and the time_point types.
             :  - "tz.h" / "tz.cpp" are a timezone library built on top of the
             :    "date.h" library. This timezone library is a complete parser
             :    of the IANA timezone database. It provides for an easy way to
             :    access all of the data in this database, using the types from
             :    "date.h" and <chrono>. The IANA database also includes data
             :    on leap seconds, and this library provides utilities to compute
             :    with that information as well.
             : Slightly modified versions of "date.h" and "tz.h" were voted into
             : the C++20 working draft.
```
