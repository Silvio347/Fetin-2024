#ifndef __MYESPNOW_H__
#define __MYESPNOW_H__

#include <esp_now.h>
#include <Arduino.h>
#include <globais.h>
#include <variaveis.h>
#include <WiFi.h>
#include <myWeb.h>
#include <sdcard.h>

extern esp_now_peer_info_t peerInfo;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len);

//***************************************************************************************************
// Function  : Faz a configuração do ESP-NOW
// Arguments : none
// Return    : none
//***************************************************************************************************
void setupEspNow();

#endif