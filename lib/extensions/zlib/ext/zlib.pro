QT -= core

TARGET = zlib
TEMPLATE = lib

CONFIG += staticlib warn_on

INCLUDEPATH += \
../../../../platform/shared/ruby/include\
../../../../platform/shared\
../../../../platform/shared/common

macx {
  DESTDIR = ../../../../platform/osx/bin/extensions
  OBJECTS_DIR = ../../../../platform/osx/bin/extensions/zlib
  INCLUDEPATH += ../../../../platform/shared/ruby/iphone
}
win32 {
  DESTDIR = ../../../../platform/win32/bin/extensions
  OBJECTS_DIR = ../../../../platform/win32/bin/extensions/zlib
  DEFINES += WIN32 _WINDOWS _LIB _UNICODE UNICODE
  debug {
    DEFINES += _DEBUG DEBUG
  }
  release {
    DEFINES += _NDEBUG NDEBUG
  }
  INCLUDEPATH += ../../../../platform/shared/ruby/win32
}

DEFINES += RHODES_QT_PLATFORM _XOPEN_SOURCE _DARWIN_C_SOURCE

!isEmpty(RHOSIMULATOR_BUILD) {
  DEFINES += RHODES_EMULATOR
}

!win32 {
  QMAKE_CFLAGS_WARN_ON += -Wno-extra -Wno-unused -Wno-sign-compare -Wno-format -Wno-parentheses
  QMAKE_CXXFLAGS_WARN_ON += -Wno-extra -Wno-unused -Wno-sign-compare -Wno-format -Wno-parentheses
  # QMAKE_CFLAGS += -fvisibility=hidden
  # QMAKE_CXXFLAGS += -fvisibility=hidden
}
win32 {
  QMAKE_CFLAGS_WARN_ON += /wd4996 /wd4100 /wd4005
  QMAKE_CXXFLAGS_WARN_ON += /wd4996 /wd4100 /wd4005
  QMAKE_CFLAGS_RELEASE += /O2
  QMAKE_CXXFLAGS_RELEASE += /O2
}

HEADERS += \
crc32.h\
deflate.h\
gzguts.h\
inffast.h\
inffixed.h\
inflate.h\
inftrees.h\
trees.h\
zconf.h\
zlib.h\
zutil.h

SOURCES += \
adler32.c\
compress.c\
crc32.c\
deflate.c\
gzclose.c\
gzlib.c\
gzread.c\
gzwrite.c\
infback.c\
inffast.c\
inflate.c\
inftrees.c\
trees.c\
uncompr.c\
zlib.c\
zutil.c
