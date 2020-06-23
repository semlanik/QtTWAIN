TEMPLATE = app

QT += quick

CONFIG += c++11

INCLUDEPATH += src

RESOURCES += \
    qml.qrc

SOURCES += \
    main.cpp \
    scanengine.cpp

DISTFILES += \
    main.qml

HEADERS += \
    scanengine.h \
    src/dsm/qdsminterface.h \
    src/qtwain_p.h \
    src/qtwaincontext_p.h \
    src/qtwaineventfilter_p.h \
    src/qtwainglobal.h \
    src/qtwainscanner.h \
    src/qtwainscanner_p.h \
    src/qtwainscannermanager.h

SOURCES += \
    src/dsm/qdsminterface.cpp \
    src/qtwaincontext.cpp \
    src/qtwaineventfilter.cpp \
    src/qtwainscanner.cpp \
    src/qtwainscanner_p.cpp \
    src/qtwainscannermanager.cpp
