//*******************************************************************
//
// Future Camp Project
//
// Copyright(C) 2019 Sergey Denisov.
//
// Written by Sergey Denisov aka LittleBuster(DenisovS21@gmail.com)
// Github:  https://github.com/LittleBuster
//          https://github.com/futcamp
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public Licence 3
// as published by the Free Software Foundation; either version 3
// of the Licence, or(at your option) any later version.
//
//*******************************************************************

#include <Arduino.h>
#include <functional>
#include <ESP8266WiFi.h>

#include "webserver.h"
#include "configs.h"


WebServer::WebServer(const std::shared_ptr<ESP8266WebServer> &server,
                     const std::shared_ptr<IHdk> &hdk):
                     server_(std::move(server)),
                     hdk_(std::move(hdk))
{
}

void WebServer::start()
{
    server_->on("/", std::bind(&WebServer::indexHandler, this));
    server_->on("/mih", std::bind(&WebServer::mihHandler, this));
    server_->onNotFound(std::bind(&WebServer::notFoundHandler, this));
    server_->begin();
}

void WebServer::process()
{
    server_->handleClient();
}

/*
 * Private functions
 */

String WebServer::getDevId()
{
    String id = "";

    for (auto c : WiFi.macAddress())
        if (c != ':')
            id += c;
    id.toLowerCase();

    return id;
}

void WebServer::indexHandler()
{
    String response;

    response = "{\"module\":\"mih\",\"id\":\"" + getDevId() +
               "\",\"board\":\"" + CFG_BOARD_NAME + "\"}";

    server_->send(WEB_SERVER_RESPONSE_OK, "application/json", response);
}

void WebServer::mihHandler()
{
    String  response;

    if (server_->arg("radio") == "true")
    {
        hdk_->switchPinMode(CFG_RADIO_PIN, HDK_PIN_MODE_OUT);
    }
    if (server_->arg("radio") == "false")
    {
        hdk_->switchPinMode(CFG_RADIO_PIN, HDK_PIN_MODE_IN);
    }
    if (server_->arg("lamp") == "true")
    {
        hdk_->switchPinMode(CFG_LAMP_PIN, HDK_PIN_MODE_OUT);
    }
    if (server_->arg("lamp") == "false")
    {
        hdk_->switchPinMode(CFG_LAMP_PIN, HDK_PIN_MODE_IN);
    }

    server_->send(WEB_SERVER_RESPONSE_OK, "application/json", "{\"result\":true}");
}

void WebServer::notFoundHandler()
{
    String response;

    response = "{\"module\":\"mih\",\"error\":\"Not found\"}";

    server_->send(WEB_SERVER_RESPONSE_OK, "application/json", response);
}
