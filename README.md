- Consider date and duration pickers and how to represent them in the buffer
  - Consider ISO 8601 duration standard
  - Also consider [this excellent library][howard-date]
- Add a field for "time actually completed" 
- Add integrity check for some things
  - If percentage is 100%, then we need to be in one of the "Finished" status
    categories
  - Make sure "time actually completed" is properly set if percentage is 100%

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
