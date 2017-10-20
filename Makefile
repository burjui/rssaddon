MGR = billmgr
PLUGIN = rssaddon
LANGS = en ru

CXXFLAGS += -I/usr/local/mgr5/include -Wall

LIB += rssaddon

rssaddon_SOURCES = list.cpp fetch.cpp rssitem.cpp httpfetcher.cpp db.cpp table.cpp
rssaddon_FOLDER = lib
rssaddon_LDADD = -lmgr -lbase

BASE ?= /usr/local/mgr5
include $(BASE)/src/isp.mk
