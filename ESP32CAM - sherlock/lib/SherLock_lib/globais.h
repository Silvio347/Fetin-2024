#ifndef GLOBAIS_H
#define GLOBAIS_H

#include <Arduino.h>

// Comandos para troca de dados entre os dois ESPs
#define PAREAR_ESP32CAM_TO_ESP32 "FPLFWQL,ESP32CAMTOESP32"      // Para parear
#define PAREAR_ESP32_TO_ESP32CAM "GJREMGLEWRMV,ESP32TOESP32CAM" // Para parear
#define SHOW_IP_COMMAND "mostreip"                              // Crie um QR Code com esse texto para exibir os IPs dos ESPs
#define SHOW_DISTANCE_SENSOR_COMMAND "distanceSensor"           // Crie um QR Code com esse texto para exibir a medição dos sensores
#define PARAMETERS_COMMAND "parametros."                        // Chega esse comando com o ssid, password, ip e porta da API
#define PASSCODE_COMMAND "passcode/"                            // Recebe essa string junto com o passcode
#define KEEP_ALIVE_COMMAND "keepalive+"                         // Envia e recebe essa string para zerar o contador do keep alive
#define RESET_ESP_COMMAND "reinicia"                            // Envia e recebe essa string para reiniciar o ESP
#define CREATE_AP_COMMAND "crieAP"                              // Crie um QR Code com esse texto para criar AP nos dois esps
#define DELETE_AP_COMMAND "deleteAP"                            // Crie um QR Code com esse texto para deletar os APs
#define WIFI_BEGIN_COMMAND "crieWifi"                           // Crie um QR Code com esse texto para tentar conectar ao wifi dnv
#define SAVE_COMMAND "save;"                                    // Chega esse comando para atualizar os dados no cartão sd
#define UPDATE_COMMAND "update;"                                // Chega esse comando para atualizar API com infos salvas no sd do esp32cam

// Time-out dos contadores
#define TIMEOUT_KEEPALIVE 30 // Tempo limite em segundos para aguardar mensagem do esp32
#define WDT_TIMER 20         // Time-out do WDT
#define TIMEOUT_WIFI 10      // Tempo em segundos para tentar conectar ao wifi
#define TIME_UPDT_API 600    // Tempo em segundos para att a API

// Parâmetros
#define PASSWORD_SITE "1234"
#define VERSAO_FIRMWARE "02/06/2024"
#define SSID_ESP "ESP32-CAM"
#define PASSWORD_ESP "1234sksksk"
#define SSID_ADDRESS 0
#define SSID_MAX_LENGTH 32
#define PASSWORD_ADDRESS (SSID_ADDRESS + SSID_MAX_LENGTH)
#define PASSWORD_MAX_LENGTH 32
#define SERVER_IP_ADDRESS (PASSWORD_ADDRESS + PASSWORD_MAX_LENGTH)
#define SERVER_IP_MAX_LENGTH 16
static const char *PMK_KEY_STR = "00XXmkwei/lpPÇf"; // Criptografia
static const char *LMK_KEY_STR = "00XXmkwei/lpPÇf"; // Criptografa

//***************************************************************************************************
// Function  : Faz o setup da configuração WiFi
// Arguments : none
// Return    : none
//***************************************************************************************************
void setupWifi(void);

//***************************************************************************************************
// Function  : Faz a função do delay mas sem travar o ESP
// Arguments : Tempo em milisegundos para o delay
// Return    : none
//***************************************************************************************************
void myDelayMillis(uint16_t tempo);

//***************************************************************************************************
// Function  : Faz a configuração do ESP-NOW
// Arguments : none
// Return    : none
//***************************************************************************************************
void setupEspNow();

//***************************************************************************************************
// Function  : Faz o envio de strings no ESPNOW
// Arguments : A string a ser enviada
// Return    : none
//***************************************************************************************************
void mySendEspNow(String data);

//***************************************************************************************************
// Function  : Sobe as fotos no banco de dados em formato de vetor
// Arguments : none
// Return    : none
//***************************************************************************************************
void uploadDados();

// Tasks de tempo
void task_5ms(void);
void task_10ms(void);
void task_25ms(void);
void task_50ms(void);
void task_250ms(void);
void task_500ms(void);
void task_1000ms(void);
void taskUpdate(void);

#endif
