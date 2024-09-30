#ifndef __MYWEB_H__
#define __MYWEB_H__

#include <WebServer.h>
#include <Update.h>
#include <globais.h>
#include <variaveis.h>
#include <pagesWeb.h>
#include <ArduinoJson.h>
#include <bsp.h>
#include <myEspNow.h>

extern IPAddress local_ip;
extern IPAddress gateway;
extern IPAddress subnet;
extern boolean OTA_AUTORIZADO;
extern WebServer server;

//***************************************************************************************************
// Function  : Setup das páginas WEB
// Arguments : none
// Return    : none
//***************************************************************************************************
void setupWeb(void);

//***************************************************************************************************
// Function  : Cria o AP se tiver sem wifi
// Arguments : none
// Return    : none
//***************************************************************************************************
void createAP();

//***************************************************************************************************
// Function  : Abre a gaveta 1 quando clicar no botão "Abrir gaveta 1" no site
// Arguments : none
// Return    : none
//***************************************************************************************************
void drawer1Function(void);

//***************************************************************************************************
// Function  : Abre a gaveta 2 quando clicar no botão "Abrir gaveta 2" no site
// Arguments : none
// Return    : none
//***************************************************************************************************
void drawer2Function(void);

//***************************************************************************************************
// Function  : Faz o setup do WiFi
// Arguments : None
// Return    : none
//***************************************************************************************************
void setupWifi(void);

#endif