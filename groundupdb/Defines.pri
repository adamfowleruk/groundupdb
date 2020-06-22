
message(Including $$_FILE_ from $$IN_PWD)
INCLUDEPATH += $$IN_PWD/..

PRE_TARGETDEPS += $$OUT_PWD/../groundupdb/libgroundupdb.a
