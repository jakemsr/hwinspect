CXX = c++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -g
CPPFLAGS = -I/usr/local/include
LDFLAGS = -L/usr/local/lib
LDLIBS = -lcurl

PROG = hwinspect
TEST = test_usb_parser

OBJS = main.o usb_parser.o db_lookup.o
TEST_OBJS = test_usb_parser.o usb_parser.o

all: ${PROG}

${PROG}: ${OBJS}
	${CXX} -o ${PROG} ${LDFLAGS} ${OBJS} ${LDLIBS}

${TEST}: ${TEST_OBJS}
	${CXX} -o ${TEST} ${TEST_OBJS}

main.o: main.cpp usb_parser.hpp device.hpp
	${CXX} ${CXXFLAGS} -c main.cpp

usb_parser.o: usb_parser.cpp usb_parser.hpp device.hpp
	${CXX} ${CXXFLAGS} ${CPPFLAGS} -c usb_parser.cpp

db_lookup.o: db_lookup.cpp db_lookup.hpp
	${CXX} ${CXXFLAGS} ${CPPFLAGS} -c db_lookup.cpp


test: ${TEST}
	./${TEST}

clean:
	rm -f ${PROG} ${TEST} *.o


.PHONY:	all test clean
