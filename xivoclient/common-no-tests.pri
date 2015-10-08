include (common-xivoclient.pri)

# regenerate qm when needed

QMAKE_EXTRA_COMPILERS += updateqm
updateqm.input = TRANSLATIONS
updateqm.output = $$ROOT_DIR/obj/${QMAKE_FILE_BASE}.qm
updateqm.commands = lrelease -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
# no_link = not included for linking
# target_predep = will be compiled before the executable
updateqm.CONFIG += no_link target_predeps

LIBS += -L$${BIN_DIR}
unix:LIBS += -lxivoclient
win32 {
    debug:LIBS += -lxivoclientd
    release:LIBS += -lxivoclient
    LIBS += -lole32 -loleaut32 -luuid
}

QT += widgets
QT += uitools

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O1
