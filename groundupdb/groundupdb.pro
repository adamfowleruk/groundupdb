CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS += -O2 -fPIC

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/database.cpp \
    src/filekeyvaluestore.cpp \
    src/groundupdb.cpp \
    src/hashes.cpp \
    src/highwayhash.cpp \
    src/memorykeyvaluestore.cpp \
    src/query.cpp \
    src/types.cpp

HEADERS += \
    groundupdb.h \
    groundupdbext.h \
    include/database.h \
    include/extensions/extdatabase.h \
    include/extensions/extquery.h \
    include/extensions/highwayhash.h \
    include/groundupdb.h \
    include/hashes.h \
    include/query.h \
    include/types.h

HH = ../../highwayhash

INCLUDEPATH += include $${HH}

LIBS += -L$${HH}/lib -lhighwayhash

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Defines.pri
