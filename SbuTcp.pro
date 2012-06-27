# -------------------------------------------------
# Project created by QtCreator 2012-06-21T15:18:06
# -------------------------------------------------
QT -= gui
TARGET = SbuTcp
CONFIG += console
CONFIG -= app_bundle
CONFIG -= x86_64
TEMPLATE = app
SOURCES += main.cpp \
    SbuSocket.cpp \
    SbuServerSocket.cpp \
    Socket.cpp \
    handler.cpp
HEADERS += SbuSocket.h \
    SbuServerSocket.h \
    SbuTcp.h \
    Socket.h \
    handler.h


