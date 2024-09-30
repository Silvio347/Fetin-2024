#include <sdcard.h>

void setupSDCard()
{
  bool aviso = false;
  unsigned long startMillis = millis(); // Tempo inicial
  unsigned long timeout = 5000;         // Tempo limite em milissegundos (5 segundos)

  while (!SD_MMC.begin("/sdcard", false))
  {
    if (millis() - startMillis >= timeout) // Verifica se o tempo limite foi alcançado
    {
      // Serial.println("Timeout: SD Card Mount Failed");
      break; // Sai do loop após o timeout
    }

    aviso = true;
    if (aviso)
    {
      // Serial.println("SD Card Mount Failed");
      aviso = false;
    }
    delay(500); // Adiciona um atraso para evitar mensagens de erro contínuas
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE)
  {
    // Serial.println("No SD Card attached");
    return;
  }
  // Serial.println("SD Card Ok");
}

void setTimezone(String timezone)
{
  // Serial.printf("  Setting Timezone to %s\n", timezone.c_str());
  setenv("TZ", timezone.c_str(), 1); //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone)
{
  if (WiFi.status() == WL_CONNECTED) // Se tiver com internet
  {
    struct tm timeinfo;
    // Serial.println("Setting up time");
    configTime(0, 0, "pool.ntp.org"); // First connect to NTP server, with 0 TZ offset
    if (!getLocalTime(&timeinfo))
    {
      // Serial.println(" Failed to obtain time");
      return;
    }
    // Serial.println("Got the time from NTP");
    // Now we can set the real timezone
    setTimezone(timezone);
  }
}

String getPictureFilename()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    // Serial.println("Failed to obtain time");
    return "";
  }
  char timeString[20];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d_%H-%M-%S", &timeinfo);
  // Serial.println(timeString);
  String filename = "/picture_" + String(timeString) + ".jpg";
  return filename;
}
