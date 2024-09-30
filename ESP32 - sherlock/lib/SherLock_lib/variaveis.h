#ifndef __VARIAVEIS_H_
#define __VARIAVEIS_H_

#include <globais.h>
#include <Arduino.h>
#include <bsp.h>

extern bool drawer1Opened;                     
extern bool drawer2Opened;                     
extern String myTimezone;
extern uint16_t contadorAPI;                      
extern uint8_t contadorLCD;                      
extern uint8_t contadorBeep;                      
extern String serverPort;                     
extern String serverIP;              
extern String ssid;                
extern String password;            
extern uint8_t senderMAC[6];                     
extern uint8_t statePair;                          
extern float distanceSensor[2];       
extern uint8_t keepAlive;                          
extern bool apActive;                          
extern uint16_t contadorWifi;                   
extern char timeString[20];
extern bool wificonnect;
extern bool foiEntregue[2];
extern bool atualizaAPI1;
extern bool atualizaAPI2;
extern bool jaConectouUmaVez;
extern bool jaSalvou;
extern bool conetado;

#endif