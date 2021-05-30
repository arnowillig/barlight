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
	~RESTServer();
	void start(int port);
	void incBeatCounter(const Rest::Request &request, Http::ResponseWriter response);
	void setMode(const Rest::Request &request, Http::ResponseWriter response);
	void getMode(const Rest::Request &request, Http::ResponseWriter response);
	void setBrightness(const Rest::Request &request, Http::ResponseWriter response);
	void getBrightness(const Rest::Request &request, Http::ResponseWriter response);
	void getStaticHTML(const Rest::Request &request, Http::ResponseWriter response);
	void setColor(const Rest::Request &request, Http::ResponseWriter response);
	void getColor(const Rest::Request &request, Http::ResponseWriter response);
private:
	LightStrip* _lightStrip;
	Rest::Router _router;
	std::string _basePath;
	Http::Endpoint* _server;
};

#endif // RESTAPI_H
