#include <myWeb.h>

void setupWifi()
{
  WiFi.begin(ssid, password); // inicia a conexão com o WiFi

  uint8_t contador_wifi = 0; // Contador do time-out

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    contador_wifi++;
    if (contador_wifi >= TIMEOUT_WIFI)
    {
      // Serial.println("Sem conexao WiFi.");
      WiFi.disconnect();
      WiFi.channel(1);
      myDelayMillis(50);
      contador_wifi = 0;
      break;
    }
    myDelayMillis(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    String ip = WiFi.localIP().toString();
    // Serial.print("Conectado em ");
    // Serial.println(ssid);
    // Serial.print("Servidor em: ");
    // Serial.println(ip);
    mySendEspNow(ip + SHOW_IP_COMMAND);
    esp_now_del_peer(receiverMAC); // Despareando

    memcpy(peerInfo.peer_addr, receiverMAC, 6); // Pareando no novo canal
    peerInfo.channel = WiFi.channel();
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      // Serial.println("Failed to add peer");
    }
  }
}

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
    if (_tic1000ms > 999)
    {
      _tic1000ms -= 1000;
      task_1000ms();
    }
  }
}