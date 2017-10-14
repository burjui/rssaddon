MGR = billmgr
PLUGIN = rssaddon
LANGS = en ru

CXXFLAGS += -I/usr/local/mgr5/include

WRAPPER += rssaddon rssfetcher

rssaddon_SOURCES = addon.cpp rssitem.cpp
rssaddon_FOLDER = addon
rssaddon_LDADD = -lmgr

rssfetcher_SOURCES = fetcher.cpp rssitem.cpp
rssfetcher_FOLDER = addon
rssfetcher_LDADD = -lmgr

BASE ?= /usr/local/mgr5
include $(BASE)/src/isp.mk
