/*
 * Stellarium Remote Control plugin
 * Copyright (C) 2015 Florian Schaukowitsch
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include "RequestHandler.hpp"
#include "httpserver/staticfilecontroller.h"

#include "APIController.hpp"
#include "LocationService.hpp"
#include "MainService.hpp"
#include "ObjectService.hpp"
#include "ScriptService.hpp"
#include "SimbadService.hpp"
#include "StelActionService.hpp"
#include "ViewService.hpp"

#include "StelUtils.hpp"
#include "StelFileMgr.hpp"

#include <QDir>

const QByteArray RequestHandler::AUTH_REALM = "Basic realm=\"Stellarium remote control\"";

RequestHandler::RequestHandler(const StaticFileControllerSettings& settings, QObject* parent) : HttpRequestHandler(parent), usePassword(false)
{
	apiController = new APIController(QByteArray("/api/").size(),this);

	//register the services
	//they "live" in the main thread in the QObject sense, but their service methods are actually
	//executed in the HTTP handler threads
	apiController->registerService(new MainService("main",apiController));
	apiController->registerService(new ObjectService("objects",apiController));
	apiController->registerService(new ScriptService("scripts",apiController));
	apiController->registerService(new SimbadService("simbad",apiController));
	apiController->registerService(new StelActionService("stelaction",apiController));
	apiController->registerService(new LocationService("location",apiController));
	apiController->registerService(new ViewService("view",apiController));

	staticFiles = new StaticFileController(settings,this);
}

RequestHandler::~RequestHandler()
{

}

void RequestHandler::update(double deltaTime)
{
	apiController->update(deltaTime);
}

void RequestHandler::setUsePassword(bool v)
{
	usePassword = v;
}

void RequestHandler::setPassword(const QString &pw)
{
	password = pw;

	//pre-create the expected response string
	QByteArray arr = password.toUtf8();
	arr.prepend(':');
	passwordReply = "Basic " + arr.toBase64();
}

void RequestHandler::service(HttpRequest &request, HttpResponse &response)
{

#define SERVER_HEADER "Stellarium RemoteControl " REMOTECONTROL_VERSION
	response.setHeader("Server",SERVER_HEADER);

	//try to support keep-alive connections
	if(QString::compare(request.getHeader("Connection"),"keep-alive",Qt::CaseInsensitive)==0)
		response.setHeader("Connection","keep-alive");
	else
		response.setHeader("Connection","close");

	if(usePassword)
	{
		//Check if the browser provided correct password, else reject request
		if(request.getHeader("Authorization") != passwordReply)
		{
			response.setStatus(401,"Not Authorized");
			response.setHeader("WWW-Authenticate",AUTH_REALM);
			response.write("HTTP 401 Not Authorized",true);
			return;
		}
	}

	QByteArray rawPath = request.getRawPath();
	QByteArray path = request.getPath();
	qDebug()<<"Request path:"<<rawPath<<" decoded:"<<path;

	if(request.getPath().startsWith("/api/"))
		apiController->service(request,response);
	else
		staticFiles->service(request,response);
}
