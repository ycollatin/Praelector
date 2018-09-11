VERSION = "0.0" 

TEMPLATE = app
TARGET = praelector
DEPENDPATH += $$PWD/src
INCLUDEPATH += $$PWD/src 

QT += widgets
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -fstack-protector-strong
QMAKE_CPPFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2

CONFIG += release_binary
#CONFIG += debug

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc

HEADERS  += src/dialogue.h \
			src/flexfr.h \
            src/ch.h \
            src/handicap.h \
            src/irregs.h \
            src/lemmatiseur.h \
            src/lemme.h \
            src/mainwindow.h \
            src/modele.h \
            src/mot.h \
            src/motflechi.h \
            src/phrase.h \
            src/regle.h \
            src/requete.h

SOURCES  += src/dialogue.cpp \
			src/flexfr.cpp \ 
            src/ch.cpp \
            src/handicap.cpp \
            src/irregs.cpp \
            src/lemmatiseur.cpp \ 
            src/lemme.cpp \
            src/main.cpp \
            src/mainwindow.cpp \
            src/modele.cpp \
            src/mot.cpp \
            src/motflechi.cpp \
            src/phrase.cpp \
            src/regle.cpp \
            src/requete.cpp

