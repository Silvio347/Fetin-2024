#ifndef __variaveis__
#define __variaveis__

#include <Arduino.h>

uint8_t keepAlive = 0;
uint8_t receiverMAC[6] = {0}; // MAC do receptor a ser registrado
bool pareado = false;

unsigned long duration;                          // Duração do Pulso do Sensor
float distance;                                  // Distância calculada do Sensor
bool drawer1Opened = false;                      // Flag usada para verificar se a gaveta 1 está aberta
bool drawer2Opened = false;                      // Flag usada para verificar se a gaveta 2 está aberta
bool transmissaoAtiva = false;                   // Flag usada para quando apertar o botão de iniciar transmissão na pag web
uint16_t contadorAPI = 0;                        // Contador para atualizar os dados dos clientes
uint8_t contadorLCD = 0;                         // Contador para desligar o LCD quando houver inatividade
uint8_t contadorBeep = 0;                        // Contador para ligar o buzzer quando a gaveta estiver aberta
String myTimezone = "WET0WEST,M3.5.0/1,M10.5.0"; // Formato do tempo para salvar fotos
String serverPort = "8080";                      // Porta da API (por padrão, 80 para HTTP)
String serverIP = "192.168.0.104";               // IP da API
String ssid = "BlueEngineering";                 // SSID do wifi
String password = "santarita2023//";             // senha do wifi

#endif