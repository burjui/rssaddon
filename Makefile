MGR = billmgr
PLUGIN = rssaddon
LANGS = en ru

CXXFLAGS += -I/usr/local/mgr5/include -Wall -Wextra

LIB += rssaddon

rssaddon_SOURCES = addon.cpp rssitem.cpp
rssaddon_FOLDER = lib
rssaddon_LDADD = -lmgr -lbase

BASE ?= /usr/local/mgr5
include $(BASE)/src/isp.mk
