QT += core websockets
QT -= gui

TARGET = localClient
CONFIG += console
CONFIG -= app_bundle
LIBS += -lnanomsg

TEMPLATE = app

SOURCES += main.cpp

