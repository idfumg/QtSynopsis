QT += core websockets
QT -= gui

TARGET = pushclientd
CONFIG += console c++11
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -Wall -Werror -pedantic
LIBS += -lnanomsg

TEMPLATE = app

SOURCES += main.cpp \
    push_client.cpp \
    token_issues.cpp \
    push_client_local_server.cpp \
    push_client_method_get_token.cpp \
    push_client_method_notifications.cpp \
    push_client_method_register_phone.cpp

HEADERS += \
    push_client.h \
    token_issues.h \
    push_client_local_server.h \
    push_client_method_register_phone.h \
    push_client_method_notifications.h \
    push_client_method_get_token.h

