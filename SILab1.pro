TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=gnu++1z

SOURCES += \
        main.cpp \
    matrix.cpp \
    error.cpp \
    factoryproblem.cpp \
    randomsearch.cpp \
    greedysearch.cpp

DISTFILES += \
    had12.dat \
    had14.dat \
    had16.dat \
    had18.dat \
    had20.dat

HEADERS += \
    geneticalgorithm.h \
    matrix.h\
    factoryproblem.h \
    generics.h \
    randomservice.h \
    randomsearch.h \
    greedysearch.h

