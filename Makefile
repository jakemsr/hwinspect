CXX = c++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -g
CPPFLAGS = -I/usr/local/include
LDFLAGS = -L/usr/local/lib
LDLIBS = -lcurl

PROG = hwinspect
TEST_USB = test_usb_parser
TEST_PCI = test_pci_parser


OBJS = main.o usb_parser.o pci_parser.o db_lookup.o
TEST_USB_OBJS = test_usb_parser.o usb_parser.o
TEST_PCI_OBJS = test_pci_parser.o pci_parser.o

all: ${PROG}

${PROG}: ${OBJS}
	${CXX} -o ${PROG} ${LDFLAGS} ${OBJS} ${LDLIBS}

${TEST_USB}: ${TEST_USB_OBJS}
	${CXX} -o ${TEST_USB} ${TEST_USB_OBJS}

${TEST_PCI}: ${TEST_PCI_OBJS}
	${CXX} -o ${TEST_PCI} ${TEST_PCI_OBJS}

main.o: main.cpp usb_parser.hpp device.hpp
	${CXX} ${CXXFLAGS} -c main.cpp

usb_parser.o: usb_parser.cpp usb_parser.hpp device.hpp
	${CXX} ${CXXFLAGS} -c usb_parser.cpp

pci_parser.o: pci_parser.cpp pci_parser.hpp device.hpp
	${CXX} ${CXXFLAGS} -c pci_parser.cpp

db_lookup.o: db_lookup.cpp db_lookup.hpp device.hpp
	${CXX} ${CXXFLAGS} ${CPPFLAGS} -c db_lookup.cpp

test_usb_parser.o: test_usb_parser.cpp usb_parser.hpp device.hpp
	${CXX} ${CXXFLAGS} -c test_usb_parser.cpp

test_pci_parser.o: test_pci_parser.cpp pci_parser.hpp device.hpp
	${CXX} ${CXXFLAGS} -c test_pci_parser.cpp



test: ${TEST_USB} ${TEST_PCI}
	./${TEST_USB}
	./${TEST_PCI}

clean:
	rm -f ${PROG} ${TEST_USB} ${TEST_PCI} *.o *.core


.PHONY:	all test clean
