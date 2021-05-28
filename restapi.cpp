#include "restapi.h"
#include <algorithm>

RESTServer::RESTServer(LightStrip* lightStrip) : _lightStrip(lightStrip)
{
	_basePath = "/root/src/barlight/html";

	Rest::Routes::Get(_router, "/api/bri/:bri",		Rest::Routes::bind(&RESTServer::setBrightness, this));
	Rest::Routes::Get(_router, "/api/bri",			Rest::Routes::bind(&RESTServer::getBrightness, this));
	Rest::Routes::Get(_router, "/api/col/:r/:g/:b",		Rest::Routes::bind(&RESTServer::setColor, this));
	Rest::Routes::Get(_router, "/index.html",		Rest::Routes::bind(&RESTServer::getStaticHTML, this));
	Rest::Routes::Get(_router, "/",				Rest::Routes::bind(&RESTServer::getStaticHTML, this));
	Rest::Routes::Get(_router, "/static/*",                 Rest::Routes::bind(&RESTServer::getStaticHTML, this));

}

void RESTServer::start(int port)
{
	Address addr(Ipv4::any(), Port(port));
	auto opts = Http::Endpoint::options().threads(1).flags(Tcp::Options::ReuseAddr);
	Http::Endpoint server(addr);
	server.init(opts);
	server.setHandler(_router.handler());
	server.serve();
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

void RESTServer::setBrightness(const Rest::Request& request, Http::ResponseWriter response)
{
	int bri = request.param(":bri").as<int>();
	if (bri<=100) {
		bri = (bri * 255) / 100;
	}
	std::cout << "BRI:" << bri << std::endl;

	std::string resp = std::to_string(bri);
	resp.append("\n");
	response.send(Http::Code::Ok, resp);
	if (bri==0) {
		_lightStrip->setMode("off");
	} else {
		_lightStrip->setMode("color");
	}
	_lightStrip->setBrightness(bri);
}

void RESTServer::getBrightness(const Rest::Request& request, Http::ResponseWriter response)
{
	(void) request;
	int bri = (_lightStrip->brightness() * 100) / 255;

	std::string resp = std::to_string(bri) + "\n";
	response.send(Http::Code::Ok, resp);
}


void RESTServer::setColor(const Rest::Request& request, Http::ResponseWriter response)
{
	int r = request.param(":r").as<int>();
	int g = request.param(":g").as<int>();
	int b = request.param(":b").as<int>();
	std::string resp = std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "\n";
	response.send(Http::Code::Ok, resp);

	_lightStrip->setMode("color");
	for (int i=0; i < _lightStrip->segmentCount(); i++) {
		_lightStrip->segment(i)->setColor(r,g,b);
	}
}

