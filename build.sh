g++ -ggdb -std=c++20 -c -o build/TicketFromVim.o src/c/TicketFromVim.cpp
g++ -ggdb -std=c++20 -c -o build/DescriptionGrabber.o src/c/DescriptionGrabber.cpp
g++ -ggdb -std=c++20 -c -o build/SharingBufferedStdinStreambuf.o src/c/SharingBufferedStdinStreambuf.cpp
g++ -ggdb -std=c++20 -c -o build/iso-8601-duration-mock.o src/c/iso-8601-duration-mock.cpp
g++ -ggdb -std=c++20 -c -o build/ConversionBindingWrangler.o src/c/ConversionBindingWrangler.cpp
g++ -ggdb -std=c++20 -o build/dac-pm-add src/c/dac-pm-add.cpp $( pkg-config --libs sqlite3 ) build/iso-8601-duration-mock.o build/TicketFromVim.o build/DescriptionGrabber.o build/SharingBufferedStdinStreambuf.o build/ConversionBindingWrangler.o
g++ -ggdb -std=c++20 -o build/dac-pm-generate-gantt src/c/dac-pm-generate-gantt.cpp $( pkg-config --libs sqlite3 )

# ./build/import-tickets < example
