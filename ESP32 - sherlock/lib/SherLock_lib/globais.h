#ifndef GLOBAIS_H
#define GLOBAIS_H

#include <Arduino.h>
#include <EEPROM.h>

// Parâmetros
#define PASSWORD_SITE "1234"                        // Senha de autorização da página WEB
#define VERSAO_FIRMWARE "02/06/2024"                // Versão do firmware para indicar no OTA
#define SSID_ESP "ESP32"                            // Nome do AP
#define PASSWORD_ESP "1234sksksk"                   // Senha para acessar o AP
#define NUM_PRODUCTS 2                              // Número de produtos/gavetas
#define SCREEN_WIDTH 128                            // OLED display width, in pixels
#define SCREEN_HEIGHT 32                            // OLED display height, in pixels
#define OLED_RESET -1                               // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C                         ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define MAX_DISTANCE_SENSOR 50                      // Máxima distância do sensor para indicar que a gaveta abriu
#define MIN_DISTANCE_SENSOR 5                       // Máxima distância do sensor para indicar que a gaveta fechou
static const char *PMK_KEY_STR = "00XXmkwei/lpPÇf"; // Criptografia do ESPNOW
static const char *LMK_KEY_STR = "00XXmkwei/lpPÇf"; // Criptografia do ESPNOW

// Time-out de contadores
#define WDT_TIMER 10         // Time-out do WDT
#define TIMEOUT_KEEPALIVE 10 // Tempo limite em segundos para aguardar mensagem do esp32cam
#define TIME_UPDT_API 5      // Tempo para atualizar os clientes na RAM e EEPROM em minutos
#define TIME_TURNOFF_LCD 10  // Tempo para desligar o LCD após inatividade em segundos
#define TIME_WIFI 1800       // Tempo em segundos para tentar conectar ao wifi novamente se tiver desconectado
#define TIMEOUT_WIFI 4       // Tempo em segundos para tentar conectar ao wifi

// Endereços e seus tamanhos para salvar na EEPROM
#define SSID_ADDRESS 0
#define SSID_MAX_LENGTH 32
#define PASSWORD_ADDRESS (SSID_ADDRESS + SSID_MAX_LENGTH)
#define PASSWORD_MAX_LENGTH 32
#define SERVER_IP_ADDRESS (PASSWORD_ADDRESS + PASSWORD_MAX_LENGTH)
#define SERVER_IP_MAX_LENGTH 16
#define CLIENT_ADDRESS_1 81
#define CLIENT_LENGHT 68
#define CLIENT_ADDRESS_2 (CLIENT_ADDRESS_1 + CLIENT_LENGHT)

// Comandos de troca de dados entre os dois esps
#define PAREAR_ESP32CAM_TO_ESP32 "FPLFWQL,ESP32CAMTOESP32"      // Chega esse comando do ESP32CAM para parear
#define PAREAR_ESP32_TO_ESP32CAM "GJREMGLEWRMV,ESP32TOESP32CAM" // Chega esse comando do ESP32 para parear
#define SHOW_IP_COMMAND "mostreip"                              // Crie um QR Code com esse texto para exibir os IPs dos ESPs
#define SHOW_IP_ESP32CAM_COMMAND "ipesp32cam"                   // Recebe esse comando junto com o ip do ESP32CAM para exibir no display
#define KEEP_ALIVE_COMMAND "keepalive+"                         // Envia e recebe essa string para zerar o contador do keep alive
#define RESET_ESP_COMMAND "reinicia"                            // Envia e recebe essa string para reiniciar o ESP
#define PASSCODE_COMMAND "passcode/"                            // Recebe essa string junto com o passcode
#define SHOW_DISTANCE_SENSOR "distanceSensor"                   // Crie um QR Code com esse texto para exibir a medição dos sensores
#define CREATE_AP_COMMAND "crieAP"                              // Crie um QR Code com esse texto para criar AP nos dois esps
#define WIFI_BEGIN_COMMAND "crieWifi"                           // Crie um QR Code com esse texto para tentar conectar ao wifi dnv
#define DELETE_AP_COMMAND "deleteAP"                            // Crie um QR Code com esse texto para deletar os APs

//***************************************************************************************************
// Function  : Faz o setup do WiFi
// Arguments : None
// Return    : none
//***************************************************************************************************
void setupWifi(void);

//***************************************************************************************************
// Function  : Faz a leitura da distância do Sensor desejado, linkando com a gaveta
// Arguments : Pino de trig, pino de echo, ID da gaveta e flag respectiva à gaveta
// Return    : none
//***************************************************************************************************
void sensorReader(uint8_t trigPin, uint8_t echoPin, uint8_t drawerID, bool &drawerOpened);

//***************************************************************************************************
// Function  : Atualiza o LCD
// Arguments : Mensagem, posição no eixo X, posição no eixo Y, se limpa a tela antes de exibir algo
// Return    : none
//***************************************************************************************************
void updateLCD(String messageLCD, uint8_t posX = 0, uint8_t posY = 0, bool clearScreen = true);

//***************************************************************************************************
// Function  : Faz a função do delay mas sem travar o ESP
// Arguments : Tempo em milisegundos para o delay
// Return    : none
//***************************************************************************************************
void myDelayMillis(uint16_t tempo);

//***************************************************************************************************
// Function  : Atualiza infos dos produtos conforme API
// Arguments : ID do produto
// Return    : none
//***************************************************************************************************
void getAPI(uint8_t id);

//***************************************************************************************************
// Function  : Escreve Strings na EEPROM
// Arguments : Endereço, string/mensagem, tamanho para ocupar
// Return    : none
//***************************************************************************************************
void escreverStringNaEEPROM(int enderecoInicio, String str, int maxLength);

//***************************************************************************************************
// Function  : Salva os parâmetros de WI-FI e IP da API
// Arguments : none
// Return    : none
//***************************************************************************************************
void salvarParametrosNaEEPROM();

//***************************************************************************************************
// Function  : Lê Strings da EEPROM
// Arguments : Endereço e tamanho para ler
// Return    : none
//***************************************************************************************************
String lerStringDaEEPROM(int enderecoInicio, int maxLength);

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
// Function  : Verifica o passCode recebido do ESP32-CAM
// Arguments : PassCode do QRCode
// Return    : none
//***************************************************************************************************
void checkPassCode(String recebido);

//***************************************************************************************************
// Function  : Faz a configuração do ESP-NOW
// Arguments : none
// Return    : none
//***************************************************************************************************
void setupEspNow();

//***************************************************************************************************
// Function  : Envia string no ESPNOW
// Arguments : String que será enviada
// Return    : none
//***************************************************************************************************
void mySendEspNow(String data);

//***************************************************************************************************
// Function  : Obtém o SSID, PASSWORD e IP da API salvos na EEPROM
// Arguments : none
// Return    : none
//***************************************************************************************************
void lerParametrosDaEEPROM();

//***************************************************************************************************
// Function  : Cria o AP se tiver sem wifi
// Arguments : none
// Return    : none
//***************************************************************************************************
void createAP();

// Tasks de tempo
void task_5ms(void);
void task_10ms(void);
void task_25ms(void);
void task_50ms(void);
void task_250ms(void);
void task_500ms(void);
void task_1000ms(void);
void taskUpdate(void);

class Cliente
{
public:
  Cliente() : id(0), nome(""), passCode(""), drawerID(0), available(false) {}
  Cliente(uint8_t id, String nome, String passCode, uint8_t drawerID, bool available)
      : id(id), nome(nome), passCode(passCode), drawerID(drawerID), available(available) {}

  uint8_t getId() const { return id; }
  String getNome() const { return nome; }
  String getPassCode() const { return passCode; }
  uint8_t getDrawerID() const { return drawerID; }
  bool isAvailable() const { return available; }

  void setId(uint8_t id) { this->id = id; }
  void setNome(const String &nome) { this->nome = nome; }
  void setPassCode(const String &passCode) { this->passCode = passCode; }
  void setDrawerID(uint8_t drawerID) { this->drawerID = drawerID; }
  void setAvailable(bool available) { this->available = available; }

  void setRAM(uint8_t idProduct, String nameProduct, String passCodeProduct, uint8_t draweridProduct, bool availableProduct)
  {
    setId(idProduct);
    setNome(nameProduct);
    setPassCode(passCodeProduct);
    setDrawerID(draweridProduct);
    setAvailable(availableProduct);
  }

  void setEEPROM(int endereco)
  {
    EEPROM.write(endereco, id);
    escreverStringNaEEPROM(endereco + sizeof(id), nome, 32);
    escreverStringNaEEPROM(endereco + sizeof(id) + 32, passCode, 32);
    EEPROM.write(endereco + sizeof(id) + 64, drawerID);
    EEPROM.write(endereco + sizeof(id) + 65, available ? 1 : 0);
    EEPROM.commit();
  }

  void getEEPROM(int endereco)
  {
    id = EEPROM.read(endereco);
    nome = lerStringDaEEPROM(sizeof(id), 32);
    passCode = lerStringDaEEPROM(sizeof(id) + 32, 32);
    drawerID = EEPROM.read(sizeof(id) + 64);
    available = EEPROM.read(sizeof(id) + 65) == 1;
  }

  void apagarNaEEPROM(int endereco)
  {
    EEPROM.write(endereco, 0);
    escreverStringNaEEPROM(endereco + sizeof(id), "", 32);
    escreverStringNaEEPROM(endereco + sizeof(id) + 32, "", 32);
    EEPROM.write(endereco + sizeof(id) + 64, 0);
    EEPROM.write(endereco + sizeof(id) + 65, 0);
    EEPROM.commit();
  }

  void limparRAM()
  {
    id = 0;
    nome = "";
    passCode = "";
    drawerID = 0;
    available = false;
  }

private:
  uint8_t id;
  String nome;
  String passCode;
  uint8_t drawerID;
  bool available;
};

#endif
