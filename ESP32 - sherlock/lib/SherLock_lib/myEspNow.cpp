#include <myEspNow.h>

esp_now_peer_info_t peerInfo;

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len)
{
	char receivedData[len + 1]; // +1 para o caractere nulo
	memcpy(receivedData, data, len);
	receivedData[len] = '\0'; // Garantir a terminação com null

	// Serial.println("chegou: " + String(receivedData));

	String receivedDataStr = String(receivedData);
	receivedDataStr.trim();

	if (statePair == 0)
	{
		// Se for comando para parear
		if (receivedDataStr == PAREAR_ESP32CAM_TO_ESP32)
		{
			memcpy(senderMAC, mac_addr, 6);
			memcpy(peerInfo.peer_addr, mac_addr, 6);
			peerInfo.channel = WiFi.channel();
			peerInfo.encrypt = false;

			if (esp_now_add_peer(&peerInfo) != ESP_OK)
			{
				// Serial.println("Failed to add peer");
				return;
			}

			// Serial.println("Pareado com o ESP32CAM!");
			mySendEspNow(PAREAR_ESP32_TO_ESP32CAM);

			String ip = WiFi.localIP().toString();
			mySendEspNow(ssid + "/" + password + "#" + serverIP + "@" + serverPort + PARAMETERS_COMMAND);

			myDelayMillis(1000);

			statePair = 1;
			wificonnect = true;
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

			if (receivedDataStr.endsWith(PASSCODE_COMMAND))
			{
				receivedDataStr.replace(PASSCODE_COMMAND, "");
				// Serial.println("Chegou o passcode: " + receivedDataStr);
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
				if (WiFi.isConnected())
				{
					ip = WiFi.localIP().toString();
				}
				else
				{
					IPAddress IP = WiFi.softAPIP();
					ip = IP.toString();
				}
				updateLCD("IP ESP32: " + ip, 0, 32);
				// updateLCD("IP ESP32CAM: " + receivedDataStr, 0, 32, false);
			}
			else if (receivedDataStr == RESET_ESP_COMMAND)
			{
				ESP.restart();
			}
			else if (receivedDataStr == SHOW_DISTANCE_SENSOR)
			{
				updateLCD("Valor do S1: " + String(distanceSensor[0], 2) + " cm");
				updateLCD("Valor do S2: " + String(distanceSensor[1], 2) + " cm", 0, 32, false);
			}
			else if (receivedDataStr.endsWith(CREATE_AP_COMMAND))
			{
				receivedDataStr.replace(CREATE_AP_COMMAND, "");
				createAP();
				// updateLCD("IP do ESP32CAM: " + receivedDataStr, 0, 32, false);
				contadorWifi = 0;
			}
			else if (receivedDataStr == DELETE_AP_COMMAND)
			{
				WiFi.softAPdisconnect(true); // Desconecta o AP
				contadorWifi = 0;
			}
			else if (receivedDataStr == WIFI_BEGIN_COMMAND)
			{
				mySendEspNow(ssid + "/" + password + "#" + serverIP + "@" + serverPort + PARAMETERS_COMMAND);
				setupWifi();
				contadorWifi = 0;
			}
		}
	}
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
	if (status != ESP_NOW_SEND_SUCCESS)
	{
		// Serial.println("não enviou certo (callback)");
		// mySendEspNow(PAREAR_ESP32_TO_ESP32CAM);
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
	// Serial.println("Enviando no canal: " + String(WiFi.channel()));
}

void setupEspNow()
{
	// Serial.println(WiFi.macAddress());

	if (esp_now_init() != ESP_OK)
	{
		// Serial.println("ESP-NOW initialization failed");
		return;
	}

	esp_now_register_recv_cb(OnDataRecv);
	esp_now_register_send_cb(OnDataSent);

	peerInfo.encrypt = false;
	peerInfo.channel = WiFi.channel();
}

void delPeerAndPair()
{
	esp_now_del_peer(senderMAC); // Despareando
	memcpy(peerInfo.peer_addr, senderMAC, 6); // Pareando no novo canal
	peerInfo.channel = WiFi.channel();
	peerInfo.encrypt = false;
	if (esp_now_add_peer(&peerInfo) != ESP_OK)
	{
		// Serial.println("Failed to add peer");
	}
	else
	{
		// Serial.println("Despareado e pareado com o ESP32CAM!");
	}
}