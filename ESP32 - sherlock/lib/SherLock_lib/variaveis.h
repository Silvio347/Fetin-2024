#ifndef __variaveis__
#define __variaveis__

#include <Arduino.h>

bool drawer1Opened = false;                      // Flag usada para verificar se a gaveta 1 está aberta
bool drawer2Opened = false;                      // Flag usada para verificar se a gaveta 2 está aberta
String myTimezone = "WET0WEST,M3.5.0/1,M10.5.0"; // Formato do tempo para requisitar ao servidor
uint16_t contadorAPI = 0;                        // Contador para atualizar os dados dos clientes
uint8_t contadorLCD = 0;                         // Contador para desligar o LCD quando houver inatividade
uint8_t contadorBeep = 0;                        // Contador para ligar o buzzer quando a gaveta estiver aberta
String serverPort = "8080";                      // Porta da API
String serverIP = "192.168.0.104";               // IP da API
String ssid = "BlueEngineering";                 // Nome da rede WiFi
String password = "santarita2023//";             // Senha da rede WiFi
uint8_t senderMAC[6] = {0};                      // Variável para armazenar o MAC do emissor
uint8_t statePair = 0;                           // Estado de pareamento (0 - não parado, 1 - pareado)
float distanceSensor[NUM_PRODUCTS] = {0};        // Distância de cada sensor para atualizar a página web
uint8_t keepAlive = 0;                           // Contador do keep alive
bool apActive = false;                           // Indicador de quando está como AP ou WiFi
uint16_t contadorWifi = 0;                       // Conta um tempo para ficar verificando o status de conexão do wifi

#endif