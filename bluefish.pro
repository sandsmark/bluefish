# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = bluefish

CONFIG += sailfishapp c++11

QT += multimedia

PKGCONFIG += gstreamer-0.10

presets.files = presets
presets.path = /usr/share/bluefish

INSTALLS = presets

# this avoids gcc 4.6 crashing on my lambdas
QMAKE_CXXFLAGS_RELEASE -= -g


LIBS += -lz

SOURCES += src/bluefish.cpp src/snapchat.cpp src/aes.c \
    src/crypto.cpp \
    src/snapmodel.cpp \
    src/camerahelper.cpp \
    src/friendsmodel.cpp

HEADERS += src/snapchat.h src/aes.h \
    src/constants.h \
    src/crypto.h \
    src/snapmodel.h \
    src/camerahelper.h \
    src/friendsmodel.h

OTHER_FILES += qml/bluefish.qml \
    qml/cover/CoverPage.qml \
    rpm/bluefish.changes.in \
    rpm/bluefish.spec \
    rpm/bluefish.yaml \
    translations/*.ts \
    bluefish.desktop \
    qml/pages/SnapList.qml \
    qml/pages/ImageView.qml \
    qml/pages/VideoView.qml \
    qml/pages/TakeSnap.qml \
    qml/pages/Settings.qml \
    qml/pages/SendSnapDialog.qml \
    qml/pages/PrepareSnapDialog.qml


# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += translations/bluefish-de.ts

