g++ -ggdb -std=c++20 -c -o build/TicketFromVim.o src/c/TicketFromVim.cpp
g++ -ggdb -std=c++20 -c -o build/DescriptionGrabber.o src/c/DescriptionGrabber.cpp
g++ -ggdb -std=c++20 -c -o build/StdinWithUngetStreambuf.o src/c/StdinWithUngetStreambuf.cpp
g++ -ggdb -std=c++20 -c -o build/iso-8601-duration-mock.o src/c/iso-8601-duration-mock.cpp
g++ -ggdb -std=c++20 -o build/dac-pm-add src/c/dac-pm-add.cpp $( pkg-config --libs sqlite3 ) build/iso-8601-duration-mock.o build/TicketFromVim.o build/DescriptionGrabber.o build/StdinWithUngetStreambuf.o
g++ -ggdb -std=c++20 -o build/dac-pm-generate-gantt src/c/dac-pm-generate-gantt.cpp $( pkg-config --libs sqlite3 )

# ./build/import-tickets < example
