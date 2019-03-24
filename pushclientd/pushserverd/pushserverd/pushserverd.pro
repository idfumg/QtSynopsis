QT += core websockets
QT -= gui

TARGET = pushserverd
CONFIG += console c++11
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -Werror -Wall -pedantic

TEMPLATE = app

SOURCES += main.cpp \
    pushserver.cpp

HEADERS += \
    pushserver.h

