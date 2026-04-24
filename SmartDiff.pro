# SmartDiff.pro
#
# Qt Widgets diff & merge tool
# Qt 6.7.3 | C++17 | qmake

QT       += core gui widgets svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++17

TARGET   = SmartDiff
TEMPLATE = app

# ── Sources ───────────────────────────────────────────────────────────────────
SOURCES += \
    core/diffengine.cpp \
    core/mergeengine.cpp \
    main.cpp \
    models/diffmodel.cpp \
    theme/thememanager.cpp \
    icons/iconmanager.cpp \
    ui/diffhighlighter.cpp \
    ui/filepanel.cpp \
    ui/linenumgutter.cpp \
    ui/mainwindow.cpp \
    ui/scrollsynchronizer.cpp \
    utils/fileloader.cpp

# ── Headers ───────────────────────────────────────────────────────────────────
HEADERS += \
    core/diffengine.h \
    core/mergeengine.h \
    models/diffmodel.h \
    theme/thememanager.h \
    icons/iconmanager.h \
    icons/iconnames.h \
    ui/diffhighlighter.h \
    ui/filepanel.h \
    ui/linenumgutter.h \
    ui/mainwindow.h \
    ui/scrollsynchronizer.h \
    utils/fileloader.h

# ── Resources ─────────────────────────────────────────────────────────────────
RESOURCES += \
    resources.qrc


# ── Include Paths ─────────────────────────────────────────────────────────────
# $$PWD = absolute path of the project root (where this .pro file lives).
# All #include statements are written relative to project root — no "../" needed.
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/core
INCLUDEPATH += $$PWD/models
INCLUDEPATH += $$PWD/ui
INCLUDEPATH += $$PWD/utils
INCLUDEPATH += $$PWD/icons
INCLUDEPATH += $$PWD/theme

# ── Deployment ────────────────────────────────────────────────────────────────
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
