TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -masm=intel
QMAKE_CXXFLAGS += -std=c++17

SOURCES += \
        main.cpp
