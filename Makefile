MGR = billmgr

CXXFLAGS += -I/usr/local/mgr5/include

SBIN += rssaddon rssfetcher

rssaddon_SOURCES = addon.cpp
rssaddon_FOLDER = addon
rssaddon_LDADD = -lmgr

rssfetcher_SOURCES = fetcher.cpp
rssfetcher_FOLDER = addon
rssfetcher_LDADD = -lmgr

BASE ?= /usr/local/mgr5
include $(BASE)/src/isp.mk
