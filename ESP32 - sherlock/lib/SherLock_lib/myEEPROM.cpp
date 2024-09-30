#include <myEEPROM.h>

void setupEEPROM()
{
  if (!EEPROM.begin(LENGHT_EEPROM))
  {
    // Serial.println("EEPROM não inicializou, tentando novamente...");
    myDelayMillis(500);
    return;
  }
}

void escreverStringNaEEPROM(int enderecoInicio, String str, int maxLength)
{
  int tamanho = str.length();

  // Verifica se o endereço e o tamanho são válidos
  if (enderecoInicio + maxLength > LENGHT_EEPROM)
  {
    // Serial.println("Erro: A string excede o tamanho da EEPROM.");
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
  // Serial.println("salvando " + ssid + " " + password + " " + serverIP);
  escreverStringNaEEPROM(SSID_ADDRESS, ssid, SSID_MAX_LENGTH);
  escreverStringNaEEPROM(PASSWORD_ADDRESS, password, PASSWORD_MAX_LENGTH);
  escreverStringNaEEPROM(SSID_MAX_LENGTH + PASSWORD_MAX_LENGTH, serverIP, SERVER_IP_MAX_LENGTH);
}

void lerParametrosDaEEPROM()
{
  ssid = lerStringDaEEPROM(SSID_ADDRESS, SSID_MAX_LENGTH);
  password = lerStringDaEEPROM(PASSWORD_ADDRESS, PASSWORD_MAX_LENGTH);
  serverIP = lerStringDaEEPROM(SERVER_IP_ADDRESS, SERVER_IP_MAX_LENGTH);
}
