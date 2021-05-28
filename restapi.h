#ifndef RESTAPI_H
#define RESTAPI_H


#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "barlight.h"

using namespace std;
using namespace Pistache;

class RESTServer
{
public:
	RESTServer(LightStrip* lightStrip);
	void start(int port);
	void setBrightness(const Rest::Request &request, Http::ResponseWriter response);
	void getBrightness(const Rest::Request &request, Http::ResponseWriter response);
	void getStaticHTML(const Rest::Request &request, Http::ResponseWriter response);
	void setColor(const Rest::Request &request, Http::ResponseWriter response);
private:
	LightStrip* _lightStrip;
	Rest::Router _router;
	std::string _basePath;
};

#endif // RESTAPI_H