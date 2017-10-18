#ifndef __HTTPFETCHER_H__
#define __HTTPFETCHER_H__

#include <ostream>
#include <curl/curl.h>

class HttpFetcher
{
public:
	HttpFetcher();
	~HttpFetcher();
	void fetch(const char *url, std::ostream &output);
	
private:
	CURL *curl;
};

#endif // __HTTPFETCHER_H__
