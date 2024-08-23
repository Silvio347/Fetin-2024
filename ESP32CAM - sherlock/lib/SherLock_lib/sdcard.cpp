#include <sdcard.h>

void setupSDCard()
{
  bool aviso = false;
  unsigned long startMillis = millis(); // Tempo inicial
  unsigned long timeout = 5000;        // Tempo limite em milissegundos (5 segundos)

  while (!SD_MMC.begin("/sdcard", false))
  {
    if (millis() - startMillis >= timeout) // Verifica se o tempo limite foi alcançado
    {
      Serial.println("Timeout: SD Card Mount Failed");
      break; // Sai do loop após o timeout
    }

    aviso = true;
    if (aviso)
    {
      Serial.println("SD Card Mount Failed");
      aviso = false;
    }
    delay(500); // Adiciona um atraso para evitar mensagens de erro contínuas
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE)
  {
    Serial.println("No SD Card attached");
    return;
  }
  Serial.println("SD Card Ok");
}
