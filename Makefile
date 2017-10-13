MGR = billmgr
PLUGIN = rssaddon
LANGS = en ru

CXXFLAGS += -I/usr/local/mgr5/include

WRAPPER += rssaddon rssfetcher

rssaddon_SOURCES = addon.cpp
rssaddon_FOLDER = addon
rssaddon_LDADD = -lmgr
#rssaddon_DLIBS = mgr

rssfetcher_SOURCES = fetcher.cpp
rssfetcher_FOLDER = addon
rssfetcher_LDADD = -lmgr
#rssfetcher_DLIBS = mgr

BASE ?= /usr/local/mgr5
include $(BASE)/src/isp.mk
