#ifndef MYEEPROM_H
#define MYEEPROM_H

#include <EEPROM.h>
#include <Arduino.h>
#include <variaveis.h>
#include <globais.h>

//***************************************************************************************************
// Function  : Begin na EEPROM
// Arguments : none
// Return    : none
//***************************************************************************************************
void setupEEPROM();

//***************************************************************************************************
// Function  : Salva os parâmetros de WI-FI e IP da API
// Arguments : none
// Return    : none
//***************************************************************************************************
void salvarParametrosNaEEPROM();

//***************************************************************************************************
// Function  : Obtém o SSID, PASSWORD e IP da API salvos na EEPROM
// Arguments : none
// Return    : none
//***************************************************************************************************
void lerParametrosDaEEPROM();

#endif