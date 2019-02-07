#HashTable Tests
check_PROGRAMS = WCHT_tests

#Tests sources
WCHT_tests_SOURCES = \
    tests/tests.c

#Tests CFlags

#include folder for HashTable header and check all warnings
WCHT_tests_CFLAGS = -I./src/ -Wall -Wextra

#Link tests to Hash table
WCHT_tests_LDADD = libWC_HashTable.la

#HashTable Library
lib_LTLIBRARIES = libWC_HashTable.la

#HashTable Sources
libWC_HashTable_la_SOURCES = \
    src/WC_HashTable.h \
    src/WC_HashTable.c

#HashTable CFlags

libWC_HashTable_la_CFLAGS = -Wall -Wextra -lm

#Linkedlist Version
libWC_HashTable_la_LDFLAGS = -version-info 1:0:0 -no-undefined

#Install linked list headers
include_HEADERS = src/WC_HashTable.h
