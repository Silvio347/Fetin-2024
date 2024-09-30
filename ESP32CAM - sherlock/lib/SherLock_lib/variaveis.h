#ifndef __VARIAVEIS_H_
#define __VARIAVEIS_H_

#include <Arduino.h>

extern uint8_t keepAlive;
extern uint8_t receiverMAC[6]; // MAC do receptor a ser registrado
extern bool pareado;

extern unsigned long duration;    // Duração do Pulso do Sensor
extern float distance;            // Distância calculada do Sensor
extern bool drawer1Opened;        // Flag usada para verificar se a gaveta 1 está aberta
extern bool drawer2Opened;        // Flag usada para verificar se a gaveta 2 está aberta
extern bool transmissaoAtiva;     // Flag usada para quando apertar o botão de iniciar transmissão na pag web
extern uint16_t contadorAPI;      // Contador para atualizar os dados dos clientes
extern uint8_t contadorLCD;       // Contador para desligar o LCD quando houver inatividade
extern uint8_t contadorBeep;      // Contador para ligar o buzzer quando a gaveta estiver aberta
extern uint8_t contadorSendKPALV; // Contador para enviar kpalv
extern String myTimezone;         // Formato do tempo para salvar fotos
extern String serverPort;         // Porta da API (por padrão, 80 para HTTP)
extern String serverIP;           // IP da API
extern String ssid;               // SSID do wifi
extern String password;           // senha do wifi
extern bool wificonnect;

#endif