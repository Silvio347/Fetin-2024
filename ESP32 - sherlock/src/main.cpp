/*
 * Copyright (c) 2024 Sher-lock
 * https://github.com/Fiddelis/sher-lock
 */

#include <Arduino.h>
#include <myWeb.h>
#include <globais.h>
#include "esp_task_wdt.h"
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <EEPROM.h>
#include <esp_now.h>
#include <bsp.h>
#include <variaveis.h>

esp_now_peer_info_t peerInfo;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Cliente produtos[NUM_PRODUCTS];

void setup()
{
  Serial.begin(115200);

  if (!Wire.begin(SDA_PIN, SCL_PIN))
  {
    Serial.println("I2C não inicializou, tentando novamente...");
    myDelayMillis(500);
    return;
  }

  if (!EEPROM.begin(256))
  {
    Serial.println("EEPROM não inicializou, tentando novamente...");
    myDelayMillis(500);
    return;
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }
  display.display();
  myDelayMillis(2000);

  setupEspNow(); // Iniciando espnow

  // Inicializa o watchdog timer com um timeout de 10 segundos
  esp_task_wdt_init(WDT_TIMER, true);
  esp_task_wdt_add(NULL); // Adiciona o loopTask ao watchdog timer

  // Pino dos sensores, buzzer e relés
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(BEEP, OUTPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  digitalWrite(BEEP, 0);
  digitalWrite(RELAY_1, 0);
  digitalWrite(RELAY_2, 0);

  // produtos[0].getEEPROM(CLIENT_ADDRESS_1);
  // produtos[1].getEEPROM(CLIENT_ADDRESS_2);
  lerParametrosDaEEPROM(); // Obtendo os parâmetros de WiFi e API salvos na EEPROM
}

void loop()
{
  if (Serial.available() > 0)
  {
    char comando = Serial.read();
    if (comando == 'a')
    {
      produtos[0].setRAM(0, "camisa x", "oii", 0, true);
      produtos[1].setRAM(1, "camisa y", "ola", 1, true);
      Serial.println("passCode dos produtos resetados!");
    }
    else if (comando == 'b')
    {
      getAPI(1);
      getAPI(2);
    }
    else if (comando == 'c')
    {
      salvarParametrosNaEEPROM();
    }
    else if (comando == 'd')
    {
      Serial.println("enviando comando para salvar foto.");
      mySendEspNow("save;");
    }
    else if (comando == 'f')
    {
      digitalWrite(RELAY_1, !digitalRead(RELAY_1));
    }
    else if (comando == 'g')
    {
      digitalWrite(RELAY_2, !digitalRead(RELAY_2));
    }
    else if (comando == 'h')
    {
      produtos[0].setRAM(1, "camisa x", "oii", 1, true);
      produtos[0].setEEPROM(CLIENT_ADDRESS_1); // Salvando os trem na EEPROM
      Serial.println("salvo na eeprom");
    }
    else if (comando == 'i')
    {
      setupWifi(); // Configurando acesso ao WiFi
      setupWeb();  // Configurando a página Web
    }
  }

  taskUpdate();
  server.handleClient(); // Manipula clientes conectados na Web
}

void checkPassCode(String recebido)
{
  bool qrInvalido = false;
  for (uint8_t i = 0; i < NUM_PRODUCTS; i++)
  {
    if (recebido == produtos[i].getPassCode())
    {
      mySendEspNow("save;");
      updateLCD("Processando seu QRCode...", 0, 16);
      myDelayMillis(1000); // Aguardando o ESP32CAM tirar a foto
      updateLCD("Produto " + produtos[i].getNome() + " a retirar.", 0, 16);

      // produtos[i].erase();
      // eraseClientsEEPROM(produtos[i].getId());

      digitalWrite(BEEP, 1);
      myDelayMillis(200);
      digitalWrite(BEEP, 0);

      if (i == 0) // Se for o produto 1, abre a gaveta 1
      {
        digitalWrite(RELAY_1, 1);
      }
      else if (i == 1) // Se for o produto 2, abre a gaveta 2
      {
        digitalWrite(RELAY_2, 1);
      }
      qrInvalido = true;
    }
  }
  if (!qrInvalido)
    updateLCD("QR Code inválido!", 0, 16);
}

void myDelayMillis(uint16_t tempo)
{
  uint32_t startTime = millis(); // Marca o tempo atual para o delay

  while ((millis() - startTime) < tempo)
    ;
}

void setupEspNow()
{
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());

  esp_now_peer_num_t espnum;
  espnum.total_num = 1; // Definindo quantidade de dispositivos pareados

  esp_now_deinit(); // apaga o pareamento
  delay(1000);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  // esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
  // for (uint8_t i = 0; i < 16; i++)
  // {
  //   peerInfo.lmk[i] = LMK_KEY_STR[i];
  // }
  peerInfo.encrypt = false;
  peerInfo.channel = 0;

  uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Endereço MAC de broadcast
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Erro no pareamento");
    return;
  }
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len)
{
  char receivedData[len + 1]; // +1 para o caractere nulo
  memcpy(receivedData, data, len);
  receivedData[len] = '\0'; // Garantir a terminação com null

  Serial.println("chegou: " + String(receivedData));

  String receivedDataStr = String(receivedData);

  if (statePair == 0)
  {
    // Se for comando para parear
    if (receivedDataStr == PAREAR_ESP32CAM_TO_ESP32)
    {
      // Armazena o MAC do emissor
      memcpy(senderMAC, mac_addr, 6);

      memcpy(peerInfo.peer_addr, mac_addr, 6);
      peerInfo.channel = 0;

      esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
      for (uint8_t i = 0; i < 16; i++)
      {
        peerInfo.lmk[i] = LMK_KEY_STR[i];
      }
      peerInfo.encrypt = false;

      if (esp_now_add_peer(&peerInfo) != ESP_OK)
      {
        Serial.println("Failed to add peer");
        return;
      }
      Serial.println("Pareado com o ESP32CAM!");

      mySendEspNow(PAREAR_ESP32_TO_ESP32CAM);
      myDelayMillis(50);

      setupWifi(); // Configurando acesso ao WiFi
      setupWeb();  // Configurando a página Web

      statePair = 1;
    }
  }
  else // Aqui já está pareado com o ESP32-CAM
  {
    bool macMatch = true;
    for (int i = 0; i < 6; i++)
    {
      if (mac_addr[i] != senderMAC[i]) // Verificando se o MAC recebido é o mesmo que o armazenado
      {
        macMatch = false;
        break;
      }
    }

    if (macMatch)
    {
      keepAlive = 0;

      if (receivedDataStr == PAREAR_ESP32CAM_TO_ESP32)
      {
        Serial.println("tá tentando parear denovo");
        mySendEspNow(PAREAR_ESP32_TO_ESP32CAM);
        mySendEspNow(ssid + "/" + password + "#" + serverIP + "@" + serverPort + "parametros.");
        return;
      }
      else if (receivedDataStr.endsWith(PASSCODE_COMMAND))
      {
        receivedDataStr.replace(PASSCODE_COMMAND, "");
        Serial.println("Chegou o passcode: " + receivedDataStr);
        checkPassCode(receivedDataStr.c_str());
      }
      else if (receivedDataStr == KEEP_ALIVE_COMMAND)
      {
        mySendEspNow(KEEP_ALIVE_COMMAND);
      }
      else if (receivedDataStr.endsWith(SHOW_IP_COMMAND))
      {
        receivedDataStr.replace(SHOW_IP_COMMAND, "");
        String ip;
        if (WiFi.status() == WL_CONNECTED)
        {
          ip = WiFi.localIP().toString();
        }
        else
        {
          IPAddress IP = WiFi.softAPIP();
          ip = IP.toString();
        }
        updateLCD("IP ESP32: " + ip);
        updateLCD("IP ESP32CAM: " + receivedDataStr, 0, 16, false);
      }
      else if (receivedDataStr == RESET_ESP_COMMAND)
      {
        ESP.restart();
      }
      else if (receivedDataStr == SHOW_DISTANCE_SENSOR)
      {
        updateLCD("Valor do S1: " + String(distanceSensor[0], 2) + " cm");
        updateLCD("Valor do S2: " + String(distanceSensor[1], 2) + " cm", 0, 16, false);
      }
      else if (receivedDataStr.endsWith(CREATE_AP_COMMAND))
      {
        receivedDataStr.replace(CREATE_AP_COMMAND, "");
        createAP();
        updateLCD("IP do ESP32CAM: " + receivedDataStr, 0, 16, false);
        contadorWifi = 0;
      }
      else if (receivedDataStr == DELETE_AP_COMMAND)
      {
        WiFi.softAPdisconnect(true); // Desconecta o AP
        contadorWifi = 0;
      }
      else if (receivedDataStr == WIFI_BEGIN_COMMAND)
      {
        setupWifi();
        contadorWifi = 0;
      }

      else
      {
        updateLCD("QR Code invalido!", 0, 16);
      }
    }
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if (status != ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("não enviou certo (callback).");
  }
  else
  {
    // Serial.println("enviou certo (callback).");
  }
}

void mySendEspNow(String data)
{
  const char *dataStr = data.c_str();
  uint8_t *buffer = (uint8_t *)data.c_str();
  size_t sizeBuff = sizeof(buffer) * data.length();
  esp_now_send(senderMAC, buffer, sizeBuff);
}

//---------------------------------------------------LCD-------------------------------------------------------

void updateLCD(String messageLCD, uint8_t posX, uint8_t posY, bool clearScreen)
{
  Serial.println(messageLCD);

  if (clearScreen)
    display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(posX, posY);
  display.println(messageLCD);
  display.display();
  contadorLCD = 0; // Zerando contador de inatividade
}

//-------------------------------------------------EEPROM-----------------------------------------------------

void escreverStringNaEEPROM(int enderecoInicio, String str, int maxLength)
{
  int tamanho = str.length();

  // Verifica se o endereço e o tamanho são válidos
  if (enderecoInicio + maxLength > 150)
  {
    Serial.println("Erro: A string excede o tamanho da EEPROM.");
    return;
  }

  // Escreve a string na EEPROM
  for (int i = 0; i < maxLength; i++)
  {
    if (i < tamanho)
    {
      EEPROM.write(enderecoInicio + i, str[i]);
    }
    else
    {
      EEPROM.write(enderecoInicio + i, '\0'); // Adiciona um caractere nulo se não houver mais dados
    }
  }
  EEPROM.commit();
}

String lerStringDaEEPROM(int enderecoInicio, int maxLength)
{
  String str = "";

  // Lê a string da EEPROM
  for (int i = 0; i < maxLength; i++)
  {
    char c = EEPROM.read(enderecoInicio + i);
    if (c == '\0')
    {
      break; // Encerra a leitura no caractere nulo
    }
    str += c;
  }
  str.trim();
  return str;
}

void salvarParametrosNaEEPROM()
{
  escreverStringNaEEPROM(SSID_ADDRESS, ssid, SSID_MAX_LENGTH);
  escreverStringNaEEPROM(SSID_MAX_LENGTH, password, PASSWORD_MAX_LENGTH);
  escreverStringNaEEPROM(SSID_MAX_LENGTH + PASSWORD_MAX_LENGTH, serverIP, SERVER_IP_MAX_LENGTH);
}

void lerParametrosDaEEPROM()
{
  ssid = lerStringDaEEPROM(SSID_ADDRESS, SSID_MAX_LENGTH);
  password = lerStringDaEEPROM(SSID_ADDRESS, SSID_MAX_LENGTH);
  serverIP = lerStringDaEEPROM(SERVER_IP_ADDRESS, SERVER_IP_MAX_LENGTH);
}

//--------------------------------------------------WiFi------------------------------------------------------

void createAP()
{
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(SSID_ESP, PASSWORD_ESP, 0);
  IPAddress IP = WiFi.softAPIP();
  String ip = IP.toString();
  updateLCD("IP do ESP32: " + ip);
  apActive = true;
}

void setupWifi()
{
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password, 0); // inicia a conexão com o WiFi

  uint8_t contador_wifi = 0;

  while (WiFi.status() != WL_CONNECTED) // Wait for connection
  {
    contador_wifi++;
    if (contador_wifi == TIMEOUT_WIFI)
    {
      Serial.println("Sem conexão WiFi.");
      break;
    }
    myDelayMillis(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(ssid);
    String ip = WiFi.localIP().toString();
    updateLCD("IP ESP32: " + ip, 0, 0);
    mySendEspNow(ssid + "/" + password + "#" + serverIP + "@" + serverPort + "parametros.");
    myDelayMillis(200); // Aguardando ESP32CAM iniciar o WiFi também
  }
}

void drawer1Function()
{
  if (drawer1Opened)
  {
    updateLCD("Gaveta 1 já está aberta!", 0, 16);
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", "Gaveta 1 já está aberta!");
  }
  else
  {
    server.sendHeader("Connection", "close");
    // server.send(200, "text/plain", "Gaveta 1 aberta!");
    updateLCD("Gaveta 1 aberta!", 0, 16);
    digitalWrite(RELAY_1, 1);
    drawer1Opened = true;
  }
}

void drawer2Function()
{
  if (drawer2Opened)
  {
    updateLCD("Gaveta 2 já está aberta!", 0, 16);
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", "Gaveta 2 já está aberta!");
  }
  else
  {
    server.sendHeader("Connection", "close");
    // server.send(200, "text/plain", "Gaveta 2 aberta!");
    updateLCD("Gaveta 2 aberta!", 0, 16);
    drawer2Opened = true;
    digitalWrite(RELAY_2, 1);
  }
}

//----------------------------------------------Sensores------------------------------------------------------

void sensorReader(uint8_t trigPin, uint8_t echoPin, uint8_t drawerID, bool &drawerOpened)
{
  uint8_t indiceSensor = drawerID - 1; // Para começar em 0

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Mede a duração do pulso no pino Echo
  unsigned long duration = pulseIn(echoPin, HIGH);

  // Calcula a distância em centímetros
  distanceSensor[indiceSensor] = duration * 0.0344 / 2;

  Serial.print("Distancia Sensor " + String(drawerID) + " :");
  Serial.print(distanceSensor[indiceSensor]);
  Serial.println(" cm");

  // Verifica se a gaveta foi aberta ou fechada
  if ((distanceSensor[indiceSensor] >= MAX_DISTANCE_SENSOR) && !drawerOpened)
  {
    updateLCD("Gaveta " + String(drawerID) + " aberta!", 0, 16);
    drawerOpened = true;
  }
  else if ((distanceSensor[indiceSensor] <= MIN_DISTANCE_SENSOR) && drawerOpened) // Se a gaveta estava aberta
  {
    updateLCD("Gaveta " + String(drawerID) + " fechada!", 0, 16);
    drawerOpened = false;

    if (drawerID == 1)
    {
      digitalWrite(RELAY_1, 0);
    }
    else if (drawerID == 2)
    {
      digitalWrite(RELAY_2, 0);
    }
  }
}

//------------------------------------------------API--------------------------------------------------------

void getAPI(uint8_t id)
{
  HTTPClient http;
  String urlProduct = "http://" + serverIP + ":" + serverPort + "/api/product/by_locker/" + String(id);
  Serial.println(urlProduct);
  http.begin(urlProduct);
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println("Resposta da API:");
      Serial.println(payload);

      // Cria um objeto JsonDocument
      DynamicJsonDocument doc(2048); // Ajuste o tamanho conforme necessário

      // Desserializa o JSON
      DeserializationError error = deserializeJson(doc, payload);

      uint8_t indiceProduto = id - 1;

      if (!error)
      {
        produtos[indiceProduto].setRAM(doc["id"], doc["name"].as<String>(), doc["pass_code"].as<String>(), doc["drawer_id"], doc["available"]);

        Serial.println("Produto " + String(id) + " desserializado:");
        Serial.println("ID: " + String(produtos[indiceProduto].getId()));
        Serial.println("Nome: " + produtos[indiceProduto].getNome());
        Serial.println("PassCode: " + produtos[indiceProduto].getPassCode());
        Serial.println("DrawerID: " + String(produtos[indiceProduto].getDrawerID()));
        Serial.println("Available: " + String(produtos[indiceProduto].isAvailable()));

        // if (indiceProduto == 0)
        //   produtos[indiceProduto].setEEPROM(CLIENT_ADDRESS_1); // Salvando os trem na EEPROM
        // else if (indiceProduto == 1)
        //   produtos[indiceProduto].setEEPROM(CLIENT_ADDRESS_2); // Salvando os trem na EEPROM
      }
      else
      {
        Serial.print("Falha na desserialização: ");
        Serial.println(error.c_str());
      }
    }
  }
  else
  {
    Serial.print("Erro na conexão HTTP: ");
    Serial.println(httpCode);
  }

  http.end();

  // String urlEntregador = "http://" + serverIP + ":" + serverPort + "/api/entregador";
  // http.begin(urlEntregador);
  // int httpCode = http.GET();

  // if (httpCode > 0)
  // {
  //   if (httpCode == HTTP_CODE_OK)
  //   {
  //     String payload = http.getString();
  //     Serial.println("Resposta da API:");
  //     Serial.println(payload);

  //     // Cria um objeto JsonDocument
  //     DynamicJsonDocument doc(2048); // Ajuste o tamanho conforme necessário

  //     // Desserializa o JSON
  //     DeserializationError error = deserializeJson(doc, payload);

  //     if (!error)
  //     {
  //       JsonArray produtosArray = doc.as<JsonArray>();
  //       passCodeEntregador = clienteObj["pass_code"].as<String>();
  //     }
  //   }
  // }
  // else
  // {
  //   Serial.print("Erro na conexão HTTP: ");
  //   Serial.println(httpCode);
  // }

  // http.end();
}

void updateAPI(uint8_t id, const String &newPassCode, bool newAvailable)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    // Monta o URL específico para o cliente
    String url = String("http://") + serverIP + ":" + String(serverPort) + "/api/endpoint";
    http.begin(url);

    // Define o cabeçalho da requisição
    http.addHeader("Content-Type", "application/json");

    // Cria o payload JSON
    DynamicJsonDocument jsonDoc(512);
    jsonDoc["pass_code"] = newPassCode;
    jsonDoc["available"] = newAvailable;

    String payload;
    serializeJson(jsonDoc, payload);

    // Envia a requisição HTTP PUT
    int httpResponseCode = http.PUT(payload);

    // Verifica o código de resposta HTTP
    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println("Resposta da API:");
      Serial.println(response);
    }
    else
    {
      Serial.printf("Erro: %d\n", httpResponseCode);
    }

    http.end();
  }
  else
  {
    Serial.println("Falha na conexão Wi-Fi.");
  }
}

//-------------------------------------------Tasks de tempo--------------------------------------------------

void taskUpdate()
{
  static uint16_t _tic1000ms = 0, _tic500ms = 0, _tic250ms = 0;
  static uint8_t _tic10ms = 0, _tic25ms = 0, _tic50ms = 0;
  static uint32_t updateTime = 0;
  uint32_t currentMillis = millis();
  uint32_t elapsedTime = 0;

  if (updateTime == 0)
  {
    updateTime = currentMillis;
  }
  elapsedTime = currentMillis - updateTime;
  if (elapsedTime > 4) // 5ms
  {

    updateTime = currentMillis;
    task_5ms();

    _tic10ms += elapsedTime;
    _tic25ms += elapsedTime;
    _tic50ms += elapsedTime;
    _tic250ms += elapsedTime;
    _tic500ms += elapsedTime;
    _tic1000ms += elapsedTime;

    if (_tic10ms > 9)
    {
      _tic10ms -= 10;
      task_10ms();
    }
    if (_tic25ms > 24)
    {
      _tic25ms -= 25;
      task_25ms();
    }
    if (_tic50ms > 49)
    {
      _tic50ms -= 50;
      task_50ms();
    }

    if (_tic250ms > 249)
    {
      _tic250ms -= 250;
      task_250ms();
    }
    if (_tic500ms > 499)
    {
      _tic500ms -= 500;
      task_500ms();
    }
    if (_tic1000ms > 999)
    {
      _tic1000ms -= 1000;
      task_1000ms();
    }
  }
}

void task_5ms()
{
}

void task_10ms()
{
}

void task_25ms()
{
}

void task_50ms()
{
}

void task_250ms()
{
}

void task_500ms()
{
}

// WDT, Sensores, API, beep e desligar LCD
void task_1000ms()
{
  if (statePair > 0)
  {
    // keepAlive++;
    if (keepAlive >= TIMEOUT_KEEPALIVE)
    {
      Serial.println("Perda de comunicação com o ESP32-CAM, reiniciando...");
      ESP.restart();
    }
  }
  // contadorAPI++;
  // if (contadorAPI >= (TIME_UPDT_API * 60))
  // {
  //   contadorAPI = 0;
  //   getAPI(1);
  //   getAPI(2);
  // }

  contadorLCD++;
  if (contadorLCD >= TIME_TURNOFF_LCD)
  {
    display.clearDisplay();
    int keyCenterX = SCREEN_WIDTH / 2;
    int keyCenterY = SCREEN_HEIGHT / 2;
    int keyWidth = 40;
    int keyHeight = 10;
    display.fillRect(keyCenterX - keyWidth / 2, keyCenterY - keyHeight / 2, keyWidth, keyHeight, SSD1306_WHITE);
    display.fillRect(keyCenterX + keyWidth / 2 - 5, keyCenterY - keyHeight / 4, 10, keyHeight / 2, SSD1306_WHITE);
    display.drawCircle(keyCenterX - keyWidth / 2 - 5, keyCenterY, 7, SSD1306_WHITE);
    display.display();
    contadorLCD = 0;
  }

  // if (drawer1Opened || drawer2Opened)
  // {
  //   contadorBeep++;
  //   if (contadorBeep >= 10)
  //   {
  //     // ativa o buzzer
  //     // digitalWrite(BEEP, 1);
  //   }
  // }

  contadorWifi++;
  if (contadorWifi >= TIME_WIFI)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      // Se não estiver conectado e o AP não estiver ativo, cria o AP
      if (!apActive)
      {
        createAP();
      }
      // Se o AP está ativo e o tempo de duração passou, tenta conectar novamente
      else
      {
        apActive = false;
        setupWifi(); // Tenta conectar ao WiFi novamente
      }
    }
  }

  esp_task_wdt_reset(); // Reseta o watchdog timer

  sensorReader(TRIG_PIN1, ECHO_PIN1, 1, drawer1Opened);
  sensorReader(TRIG_PIN2, ECHO_PIN2, 2, drawer2Opened);
}