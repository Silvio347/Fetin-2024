#include <myEspNow.h>

esp_now_peer_info_t peerInfo;

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len)
{
	char receivedData[len + 1]; // +1 para o caractere nulo
	memcpy(receivedData, data, len);
	receivedData[len] = '\0'; // Garantir a terminação com null
	String receivedDataStr = String(receivedData);
	receivedDataStr.trim();

	// Serial.println("Chegou: " + receivedDataStr);

	// Se chegar o comando certo para parear
	if ((receivedDataStr == PAREAR_ESP32_TO_ESP32CAM) && !pareado)
	{
		// Armazena o MAC do receptor
		memcpy(receiverMAC, mac_addr, 6);
		memcpy(peerInfo.peer_addr, mac_addr, 6); // Para parear
		peerInfo.channel = WiFi.channel();

		peerInfo.encrypt = false; // Define se criptografa ou n

		if (esp_now_add_peer(&peerInfo) != ESP_OK) // Pareando
		{
			// Serial.println("Failed to add peer");
			return;
		}
		// Serial.println("Pareado com o ESP32!");
		pareado = true;
		return;
	}

	if (pareado) // Se tiver pareado
	{
		if (receivedDataStr == KEEP_ALIVE_COMMAND) // Zera o keep alive
		{
			keepAlive = 0;
		}
		else if (receivedDataStr.endsWith(PARAMETERS_COMMAND)) // Obtém o ssid para  o wifi
		{
			receivedDataStr.replace(PARAMETERS_COMMAND, "");

			int slashIndex = receivedDataStr.indexOf('/');
			int hashIndex = receivedDataStr.indexOf('#');
			int atIndex = receivedDataStr.indexOf('@');

			ssid = receivedDataStr.substring(0, slashIndex);
			password = receivedDataStr.substring(slashIndex + 1, hashIndex);
			serverIP = receivedDataStr.substring(hashIndex + 1, atIndex);
			serverPort = receivedDataStr.substring(atIndex + 1);
			ssid.trim();
			password.trim();
			serverIP.trim();
			serverPort.trim();

			// Serial.println("ssid - " + ssid);
			// Serial.println("password - " + password);
			wificonnect = true;
		}
		else if (receivedDataStr == SAVE_COMMAND) // Tira e salva foto no cartão SD
		{
			receivedDataStr.replace(PARAMETERS_COMMAND, "");
			receivedDataStr.trim();
			SaveSDCard(receivedDataStr);
		}
		else if (receivedDataStr == RESET_ESP_COMMAND)
		{
			ESP.restart();
		}
		else if (receivedDataStr == UPDATE_COMMAND)
		{
			uploadDados();
		}
		else if (receivedDataStr == "tentaConectar")
		{
			wificonnect = true;
		}
		else if (receivedDataStr == CREATE_AP_COMMAND)
		{
			esp_now_del_peer(receiverMAC); // Despareando

			memcpy(peerInfo.peer_addr, receiverMAC, 6); // Pareando no novo canal
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
	}
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
	if (status != ESP_NOW_SEND_SUCCESS)
	{
		// Serial.println("não enviou certo (callback).");
	}
}

void mySendEspNow(String data)
{
	const char *dataStr = data.c_str();
	uint8_t *buffer = (uint8_t *)data.c_str();
	size_t sizeBuff = sizeof(buffer) * data.length();
	// Serial.println("enviando no canal " + String(WiFi.channel()));
	esp_now_send(receiverMAC, buffer, sizeBuff);
}

void setupEspNow()
{
	// Serial.println(WiFi.macAddress()); // Imprime o MAC desse ESP

	if (esp_now_init() != ESP_OK) // Inicia o ESP-NOW
	{
		// Serial.println("ESP-NOW initialization failed");
		return;
	}

	esp_now_register_recv_cb(OnDataRecv); // Função de callback
	esp_now_register_send_cb(OnDataSent); // Função de callback

	uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Endereço MAC de broadcast

	memcpy(peerInfo.peer_addr, broadcastAddress, 6);
	memcpy(receiverMAC, broadcastAddress, 6);
	peerInfo.channel = WiFi.channel();
	peerInfo.encrypt = false;

	// Add peer
	if (esp_now_add_peer(&peerInfo) != ESP_OK)
	{
		// Serial.println("Erro no pareamento");
		return;
	}
}