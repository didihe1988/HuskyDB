TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG+=c++11

SOURCES += main.cpp \
    page.cpp \
    file.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    page.h \
    record.h \
    file.h \
    file_iterator.h

