#ifndef GLOBAIS_H
#define GLOBAIS_H

#include <Arduino.h>
#include <myEEPROM.h>

// Parâmetros
#define PASSWORD_SITE "1234"                        // Senha de autorização da página WEB
#define VERSAO_FIRMWARE "02/06/2024"                // Versão do firmware para indicar no OTA
#define SSID_ESP "ESP32"                            // Nome do AP
#define PASSWORD_ESP "1234sksksk"                   // Senha para acessar o AP
#define NUM_PRODUCTS 2                              // Número de produtos/gavetas
#define SCREEN_WIDTH 128                            // OLED display width, in pixels
#define SCREEN_HEIGHT 64                            // OLED display height, in pixels
#define OLED_RESET -1                               // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C                         ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define MAX_DISTANCE_SENSOR 18                      // Máxima distância do sensor para indicar que a gaveta abriu
#define MIN_DISTANCE_SENSOR 8                       // Máxima distância do sensor para indicar que a gaveta fechou
static const char *PMK_KEY_STR = "00XXmkwei/lpPÇf"; // Criptografia do ESPNOW
static const char *LMK_KEY_STR = "00XXmkwei/lpPÇf"; // Criptografia do ESPNOW
#define LENGHT_EEPROM 512

// Time-out de contadores
#define WDT_TIMER 20         // Time-out do WDT
#define TIMEOUT_KEEPALIVE 30 // Tempo limite em segundos para aguardar mensagem do esp32cam
#define TIME_UPDT_API 5      // Tempo para atualizar os clientes na RAM e EEPROM em minutos
#define TIME_TURNOFF_LCD 15  // Tempo para desligar o LCD após inatividade em segundos
#define TIME_WIFI 15         // Tempo em segundos para tentar conectar ao wifi novamente se tiver desconectado
#define TIMEOUT_WIFI 10      // Tempo em segundos para tentar conectar ao wifi
#define TIMEOUT_BUZZER 30    // Tempo em segundos para desligar o buzzer

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
#define DATE_TIME_ADDRESS (CLIENT_ADDRESS_2 + CLIENT_LENGHT)
#define DATE_TIME_LENGHT 20

// Comandos de troca de dados entre os dois esps
#define PAREAR_ESP32CAM_TO_ESP32 "FPLFWQL,ESP32CAMTOESP32"      // Chega esse comando do ESP32CAM para parear
#define PAREAR_ESP32_TO_ESP32CAM "GJREMGLEWRMV,ESP32TOESP32CAM" // Chega esse comando do ESP32 para parear
#define SHOW_IP_COMMAND "mostreip"                              // Crie um QR Code com esse texto para exibir os IPs dos ESPs
#define PARAMETERS_COMMAND "parametros."                        // Envia esse comando com o ssid, password, ip e porta da API
#define KEEP_ALIVE_COMMAND "keepalive+"                         // Envia e recebe essa string para zerar o contador do keep alive
#define RESET_ESP_COMMAND "reinicia"                            // Envia e recebe essa string para reiniciar o ESP
#define PASSCODE_COMMAND "passcode/"                            // Recebe essa string junto com o passcode
#define SHOW_DISTANCE_SENSOR "distanceSensor"                   // Crie um QR Code com esse texto para exibir a medição dos sensores
#define CREATE_AP_COMMAND "crieAP"                              // Crie um QR Code com esse texto para criar AP nos dois esps
#define WIFI_BEGIN_COMMAND "crieWifi"                           // Crie um QR Code com esse texto para tentar conectar ao wifi dnv
#define DELETE_AP_COMMAND "deleteAP"                            // Crie um QR Code com esse texto para deletar os APs
#define SAVE_COMMAND "save;"                                    // Envia esse comando para salvar infos no cartão SD do ESP32CAM
#define UPDATE_COMMAND "update;"                                // Envia esse comando para atualizar API com infos salvas no sd do esp32cam

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
// Arguments : none
// Return    : none
//***************************************************************************************************
void getAPI();

//***************************************************************************************************
// Function  : Verifica o passCode recebido do ESP32-CAM
// Arguments : PassCode do QRCode
// Return    : none
//***************************************************************************************************
void checkPassCode(String recebido);

//***************************************************************************************************
// Function  : Envia string no ESPNOW
// Arguments : String que será enviada
// Return    : none
//***************************************************************************************************
void mySendEspNow(String data);

//***************************************************************************************************
// Function  : Atualiza o banco de dados quando um produto for retirado
// Arguments : id da gaveta
// Return    : none
//***************************************************************************************************
void updateAPI(uint8_t id);

//***************************************************************************************************
// Function  : Lê Strings da EEPROM
// Arguments : Endereço e tamanho para ler
// Return    : none
//***************************************************************************************************
String lerStringDaEEPROM(int enderecoInicio, int maxLength);

//***************************************************************************************************
// Function  : Escreve Strings na EEPROM
// Arguments : Endereço, string/mensagem, tamanho para ocupar
// Return    : none
//***************************************************************************************************
void escreverStringNaEEPROM(int enderecoInicio, String str, int maxLength);

//***************************************************************************************************
// Function  : Passa o tempo obtido para uma string
// Arguments : none
// Return    : o tempo obtido
//***************************************************************************************************
String getTime();

//***************************************************************************************************
// Function  : Obtém data e hora do servidor
// Arguments : none
// Return    : none
//***************************************************************************************************
void updateTime();

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
  // Construtor padrão
  Cliente() : id(0), nome(""), deliveryCode(""), withdrawnCode(""), drawerID(0),
              tempo_retirado("00/00/00 00:00:00"), tempo_inserido("00/00/00 00:00:00"),
              tempo_abertura("11/11/11 11:11:11"), tempo_fechamento("11/11/11 11:11:11") {}

  // Construtor com parâmetros
  Cliente(String nome, String deliveryCode, String withdrawnCode, uint8_t drawerID)
      : nome(nome), deliveryCode(deliveryCode), withdrawnCode(withdrawnCode), drawerID(drawerID),
        tempo_retirado("00/00/00 00:00:00"), tempo_inserido("00/00/00 00:00:00"),
        tempo_abertura("00/00/00 00:00:00"), tempo_fechamento("00/00/00 00:00:00") {}

  // Getters
  String getNome() const { return nome; }
  uint8_t getId() const { return id; }
  String getWithdrawnCode() const { return deliveryCode; }
  String getDeliveryCode() const { return withdrawnCode; }
  uint8_t getDrawerID() const { return drawerID; }
  String getTimeWithdrawn() const { return tempo_retirado; }
  String getTimeInserted() const { return tempo_inserido; }
  String getTimeOpening() const { return tempo_abertura; }
  String getTimeClosing() const { return tempo_fechamento; }

  // Setters
  void setNome(const String &nome) { this->nome = nome; }
  void setWithdrawnCode(const String &deliveryCode) { this->deliveryCode = deliveryCode; }
  void setDeliveryCode(const String &withdrawnCode) { this->withdrawnCode = withdrawnCode; }
  void setDrawerID(uint8_t drawerID) { this->drawerID = drawerID; }
  void setId(uint8_t id) { this->id = id; }
  void setTimeWithdrawn(const String &tempo_retirado) { this->tempo_retirado = tempo_retirado; }
  void setTimeInserted(const String &tempo_inserido) { this->tempo_inserido = tempo_inserido; }
  void setTimeOpening(const String &tempo_abertura) { this->tempo_abertura = tempo_abertura; }
  void setTimeClosing(const String &tempo_fechamento) { this->tempo_fechamento = tempo_fechamento; }

  // Função para setar os valores na RAM
  void setRAM(uint8_t id, String nameProduct, String deliveryCode, String withdrawnCode, uint8_t draweridProduct)
  {
    setId(id);
    setNome(nameProduct);
    setWithdrawnCode(deliveryCode);
    setDeliveryCode(withdrawnCode);
    setDrawerID(draweridProduct);
  }

  // Função para salvar o ID e dados na EEPROM
  void setEEPROM(int endereco)
  {
    EEPROM.write(endereco, id); // Salva o ID no primeiro byte
    escreverStringNaEEPROM(endereco + 1, nome, 32);
    escreverStringNaEEPROM(endereco + 33, deliveryCode, 32);
    escreverStringNaEEPROM(endereco + 65, withdrawnCode, 32);
    EEPROM.write(endereco + 97, drawerID); // Salva drawerID
    EEPROM.commit();
  }

  // Função para ler o ID e dados da EEPROM
  void getEEPROM(int endereco)
  {
    id = EEPROM.read(endereco); // Lê o ID do primeiro byte
    nome = lerStringDaEEPROM(endereco + 1, 32);
    deliveryCode = lerStringDaEEPROM(endereco + 33, 32);
    withdrawnCode = lerStringDaEEPROM(endereco + 65, 32);
    drawerID = EEPROM.read(endereco + 97); // Lê drawerID
  }

  // Função para apagar dados na EEPROM
  void apagarNaEEPROM(int endereco)
  {
    EEPROM.write(endereco, 0); // Apaga o ID
    escreverStringNaEEPROM(endereco + 1, "", 32);
    escreverStringNaEEPROM(endereco + 33, "", 32);
    escreverStringNaEEPROM(endereco + 65, "", 32);
    EEPROM.write(endereco + 97, 0); // Apaga drawerID
    EEPROM.commit();
  }

  // Função para limpar os dados na RAM
  void limparRAM()
  {
    id = 0;
    nome = "";
    deliveryCode = "";
    withdrawnCode = "";
    drawerID = 0;
    tempo_retirado = "";
    tempo_inserido = "";
    tempo_abertura = "";
    tempo_fechamento = "";
  }

private:
  String nome;
  String deliveryCode;
  String withdrawnCode;
  uint8_t drawerID;
  uint8_t id;
  String tempo_retirado;
  String tempo_inserido;
  String tempo_abertura;
  String tempo_fechamento;

  // Função para escrever uma string na EEPROM
  void escreverStringNaEEPROM(int endereco, String valor, int tamanhoMax)
  {
    for (int i = 0; i < tamanhoMax; i++)
    {
      if (i < valor.length())
      {
        EEPROM.write(endereco + i, valor[i]);
      }
      else
      {
        EEPROM.write(endereco + i, 0);
      }
    }
  }

  // Função para ler uma string da EEPROM
  String lerStringDaEEPROM(int endereco, int tamanhoMax)
  {
    char dados[tamanhoMax + 1];
    for (int i = 0; i < tamanhoMax; i++)
    {
      dados[i] = EEPROM.read(endereco + i);
    }
    dados[tamanhoMax] = '\0'; // Termina a string
    return String(dados);
  }
};

#endif
