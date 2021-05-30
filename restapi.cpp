#include "restapi.h"
#include <algorithm>

RESTServer::RESTServer(LightStrip* lightStrip) : _lightStrip(lightStrip), _server(nullptr)
{
#ifdef _GUI_
	_basePath = "/home/akw/src/barlight/html";
#else
	_basePath = "/opt/barlight/html";
#endif
	Rest::Routes::Get(_router, "/api/bri/:bri",		Rest::Routes::bind(&RESTServer::setBrightness, this));
	Rest::Routes::Get(_router, "/api/bri",			Rest::Routes::bind(&RESTServer::getBrightness, this));
	Rest::Routes::Get(_router, "/api/col/:r/:g/:b",		Rest::Routes::bind(&RESTServer::setColor, this));
	Rest::Routes::Get(_router, "/api/col",			Rest::Routes::bind(&RESTServer::getColor, this));
	Rest::Routes::Get(_router, "/api/mode/:mode",		Rest::Routes::bind(&RESTServer::setMode, this));
	Rest::Routes::Get(_router, "/api/mode",			Rest::Routes::bind(&RESTServer::getMode, this));
	Rest::Routes::Get(_router, "/index.html",		Rest::Routes::bind(&RESTServer::getStaticHTML, this));
	Rest::Routes::Get(_router, "/",				Rest::Routes::bind(&RESTServer::getStaticHTML, this));
	Rest::Routes::Get(_router, "/static/*",                 Rest::Routes::bind(&RESTServer::getStaticHTML, this));
}

RESTServer::~RESTServer()
{
	if (_server) {
		_server->shutdown();
		delete _server;
	}
}

void RESTServer::start(int port)
{
	Address addr(Ipv4::any(), Port(port));
	auto opts = Http::Endpoint::options().threads(1).flags(Tcp::Options::ReuseAddr);
	_server = new Http::Endpoint(addr);
	_server->init(opts);
	_server->setHandler(_router.handler());
	_server->serveThreaded();
}


void RESTServer::getStaticHTML(const Rest::Request& request, Http::ResponseWriter response)
{
	std::cout << "REST: " << request.resource() << std::endl;
	if (request.resource()=="/" || request.resource()=="/index.html") {
		Http::serveFile(response, _basePath + "/index.html");
		return;
	}
	if (request.resource().find("/static/", 0) == 0) {
		Http::serveFile(response, _basePath + request.resource().substr(7));
		return;
	}
	response.send(Http::Code::Not_Found);
}

void RESTServer::setMode(const Rest::Request &request, Http::ResponseWriter response)
{
	std::string mode = request.param(":mode").as<std::string>();
	std::string resp = mode + "\n";
	response.send(Http::Code::Ok, resp);
	_lightStrip->setMode(mode);
}

void RESTServer::getMode(const Rest::Request &request, Http::ResponseWriter response)
{
	(void) request;
	response.send(Http::Code::Ok, "{ \"mode\": \""+_lightStrip->mode()+"\"}\n");
}


void RESTServer::setBrightness(const Rest::Request& request, Http::ResponseWriter response)
{
	int bri = request.param(":bri").as<int>();
	std::string resp = "{ \"bri:\": " + std::to_string(bri) + " }\n";

	bri = (bri * 255) / 100;
	if (bri>255) {
		bri = 255;
	}
	response.send(Http::Code::Ok, resp);
	/*
	if (bri==0) {
		_lightStrip->setMode("off");
	} else {
		_lightStrip->setMode("color");
	}
	*/
	_lightStrip->setBrightness(bri);
}

void RESTServer::getBrightness(const Rest::Request& request, Http::ResponseWriter response)
{
	(void) request;
	int bri = (_lightStrip->brightness() * 100) / 255;

	std::string resp = "{ \"bri:\": " + std::to_string(bri) + " }\n";
	response.send(Http::Code::Ok, resp);
}


void RESTServer::setColor(const Rest::Request& request, Http::ResponseWriter response)
{
	int r = request.param(":r").as<int>();
	int g = request.param(":g").as<int>();
	int b = request.param(":b").as<int>();
	std::string resp = "{ \"col:\": \"" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "\" }\n";
	response.send(Http::Code::Ok, resp);

	_lightStrip->setMode("color");
	_lightStrip->setColor(r,g,b);
	/*
	for (int i=0; i < _lightStrip->segmentCount(); i++) {
		_lightStrip->segment(i)->setColor(r,g,b);
	}
	*/
}

void RESTServer::getColor(const Rest::Request& request, Http::ResponseWriter response)
{
	(void) request;
	int color = _lightStrip->lastColor();
        uint8_t r = (color >> 16) & 0xff;
        uint8_t g = (color >>  8) & 0xff;
        uint8_t b = (color >>  0) & 0xff;

	std::string resp = "{ \"col:\": \"" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "\" }\n";
	response.send(Http::Code::Ok, resp);
}
