TEMPLATE = app

QT += testlib core

CONFIG += c++11

INCLUDEPATH = $$PWD/../../../osm_elements

LIBS += -L$$PWD/../../../intermediate_libs/ -losm_elements

SOURCES += \
    test_referential_integrity.cpp
