// =========================================================================================
// ARQUIVO: config.h - DEFINIÇÃO DAS ESTRUTURAS DE CONFIGURAÇÃO E PERSISTÊNCIA EM MEMÓRIA FLASH
// =========================================================================================
// Este arquivo define as estruturas de dados necessárias para salvar e carregar os parâmetros
// de rede (Wi-Fi Station, Ethernet LAN8720, Ponto de Acesso AP), Modbus TCP e autenticação.
// Todas as configurações são persistidas na partição LittleFS em formato JSON (/config.json).

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define CONFIG_FILE "/config.json"             // Caminho do arquivo de configuração na Flash

bool ethConectada = false;                     // Flag indicando se o cabo Ethernet LAN está plugado e ativo
String systemLogs = "";                        // Buffer circular de logs em memória RAM para diagnóstico
// Estrutura hierárquica contendo todas as variáveis de configuração do sistema
struct Config {
  struct {
    bool active;                               // Flag para ativar ou desativar o Wi-Fi Station
    String ssid;                               // Nome da rede Wi-Fi do roteador
    String pass;                               // Senha da rede Wi-Fi
    bool dhcp;                                 // true para DHCP automático, false para IP estático
    String ip;                                 // Endereço IP estático da estação Wi-Fi
    String gw;                                 // Endereço IP do Gateway (Roteador)
    String sn;                                 // Máscara de sub-rede (Netmask)
    String dns;                                // Servidor DNS Primário
  } station;

  struct {
    bool active;                               // Flag para ativar ou desativar a porta Ethernet LAN8720
    bool dhcp;                                 // true para DHCP automático, false para IP estático
    String ip;                                 // Endereço IP estático da porta cabeada
    String gw;                                 // Gateway da rede cabeada
    String sn;                                 // Máscara de sub-rede
    String dns;                                // Servidor DNS
    int webPort;                               // Porta do serviço Webserver (Padrão 80)
    int alexaPort;                             // Porta para emulação do serviço Alexa (Padrão 81)
  } eth;

  struct {
    bool enabled;                              // Flag para ativar ou desativar o Ponto de Acesso
    String ssid;                               // Nome da rede Wi-Fi gerada pelo ESP32 (SoftAP)
    String pass;                               // Senha da rede de acesso (mínimo 8 caracteres)
    String ip;                                 // IP próprio do ESP32 no modo AP
    String gw;                                 // Gateway do AP
    String sn;                                 // Máscara de sub-rede do AP
  } ap;

  struct {
    int webPort;                               // Porta do servidor Web HTTP
    int alexaPort;                             // Porta de escuta da Alexa
    int googlePort;                            // Porta de serviço Google Home
  } system;

  struct {
    bool enabled;                              // Flag para ativar autenticação HTTP por senha
    String user;                               // Nome de usuário administrativo (ex: "admin")
    String pass;                               // Senha de acesso do painel
  } security;

  struct {
    bool enabled;                              // Flag para ativar polling e controle Modbus TCP
    String ip;                                 // Endereço IP do módulo Modbus ETD8A12
    int port;                                  // Porta do protocolo Modbus TCP (Padrão 502)
    int slaveId;                               // ID do escravo / Unit ID (Padrão 1)
    int timeout;                               // Tempo limite de resposta em milissegundos
    String gw;                                 // Gateway do barramento
    String sn;                                 // Máscara
  } modbus;

  struct {
    bool restoreStateOnBoot;                   // Salva e restaura o estado dos relés ao ligar/desligar a placa
    String names[16];                          // Nomes customizados para os 16 canais de relé
  } relays;

  struct {
    int mode;                                  // Modo padrão de operação (compatibilidade retroativa)
    int modes[16];                             // Modo individual de cada um dos 16 inputs (0=Three-Way/Paralelo, 1=Seguidor de Borda, 2=Pulsador)
    int debounceMs;                            // Tempo de debounce / filtro de ruído I2C em milissegundos (padrão 25ms)
  } inputs;
};

extern Config deviceConfig;                    // Declaração da variável global de configuração

// Restaura todos os parâmetros de fábrica pré-definidos
void resetToDefaults() {
  deviceConfig.inputs.mode = 0;                // Padrão: Modo Toggle Paralelo (Three-Way)
  for (int i = 0; i < 16; i++) {
    deviceConfig.inputs.modes[i] = 0;          // Padrão individual: 0 (Three-Way / Paralelo)
  }
  deviceConfig.inputs.debounceMs = 25;
  deviceConfig.station.active = true;
  deviceConfig.station.ssid = "GSH9126";
  deviceConfig.station.pass = "0148483886";
  deviceConfig.station.dhcp = false;
  deviceConfig.station.ip = "192.168.18.222";
  deviceConfig.station.gw = "192.168.18.1";
  deviceConfig.station.sn = "255.255.255.0";
  deviceConfig.station.dns = "8.8.8.8";

  deviceConfig.eth.active = true;
  deviceConfig.eth.dhcp = false;
  deviceConfig.eth.ip = "192.168.18.232";
  deviceConfig.eth.gw = "192.168.18.1";
  deviceConfig.eth.sn = "255.255.255.0";
  deviceConfig.eth.dns = "8.8.8.8";
  deviceConfig.eth.webPort = 80;
  deviceConfig.eth.alexaPort = 81;

  deviceConfig.ap.enabled = true;
  deviceConfig.ap.ssid = "KC868_A16_PRO";
  deviceConfig.ap.pass = "admin";
  deviceConfig.ap.ip = "192.168.4.1";
  deviceConfig.ap.gw = "192.168.4.1";
  deviceConfig.ap.sn = "255.255.255.0";

  deviceConfig.system.webPort = 80;
  deviceConfig.system.alexaPort = 81;
  deviceConfig.system.googlePort = 8008;

  deviceConfig.security.enabled = true;
  deviceConfig.security.user = "admin";
  deviceConfig.security.pass = "admin";

  deviceConfig.modbus.enabled = true;
  deviceConfig.modbus.ip = "192.168.18.21";
  deviceConfig.modbus.port = 502;
  deviceConfig.modbus.slaveId = 1;
  deviceConfig.modbus.timeout = 2000;

  deviceConfig.relays.restoreStateOnBoot = true;
  for (int i = 0; i < 16; i++) {
    if (i == 2) deviceConfig.relays.names[i] = "SALA";
    else deviceConfig.relays.names[i] = "RELÉ " + String(i + 1);
  }
}

// Carrega as configurações a partir do arquivo JSON no LittleFS
void carregarConfiguracao() {
  if (!LittleFS.exists(CONFIG_FILE)) {          // Se o arquivo não existir na Flash
    resetToDefaults();                         // Restaura os padrões
    return;
  }
  
  File file = LittleFS.open(CONFIG_FILE, "r"); // Abre o arquivo em modo leitura
  DynamicJsonDocument doc(4096);               // Aloca buffer para desserialização
  DeserializationError err = deserializeJson(doc, file); // Lê e decodifica o JSON
  file.close();                                // Fecha o arquivo
  
  if (err) {                                   // Se o arquivo estiver corrompido
    resetToDefaults();                         // Restaura padrões
    return;
  }

  // Atribui os campos lidos às variáveis da estrutura com valores de fallback seguros
  deviceConfig.station.active = doc["station"]["active"] | true;
  deviceConfig.station.ssid = doc["station"]["ssid"] | "GSH9126";
  deviceConfig.station.pass = doc["station"]["pass"] | "0148483886";
  deviceConfig.station.dhcp = doc["station"]["dhcp"] | false;
  deviceConfig.station.ip = doc["station"]["ip"] | "192.168.18.222";
  deviceConfig.station.gw = doc["station"]["gw"] | "192.168.18.1";
  deviceConfig.station.sn = doc["station"]["sn"] | "255.255.255.0";
  deviceConfig.station.dns = doc["station"]["dns"] | "8.8.8.8";

  deviceConfig.eth.active = doc["eth"]["active"] | true;
  deviceConfig.eth.dhcp = doc["eth"]["dhcp"] | false;
  deviceConfig.eth.ip = doc["eth"]["ip"] | "192.168.18.232";
  deviceConfig.eth.gw = doc["eth"]["gw"] | "192.168.18.1";
  deviceConfig.eth.sn = doc["eth"]["sn"] | "255.255.255.0";
  deviceConfig.eth.dns = doc["eth"]["dns"] | "8.8.8.8";

  deviceConfig.ap.enabled = doc["ap"]["enabled"] | true;
  deviceConfig.ap.ssid = doc["ap"]["ssid"] | "KC868_A16_PRO";
  deviceConfig.ap.pass = doc["ap"]["pass"] | "admin";
  deviceConfig.ap.ip = doc["ap"]["ip"] | "192.168.4.1";
  deviceConfig.ap.gw = doc["ap"]["gw"] | "192.168.4.1";
  deviceConfig.ap.sn = doc["ap"]["sn"] | "255.255.255.0";

  deviceConfig.security.enabled = doc["security"]["enabled"] | true;
  deviceConfig.security.user = doc["security"]["user"] | "admin";
  deviceConfig.security.pass = doc["security"]["pass"] | "admin";

  deviceConfig.modbus.enabled = doc["modbus"]["enabled"] | true;
  deviceConfig.modbus.ip = doc["modbus"]["ip"] | "192.168.18.21";
  deviceConfig.modbus.port = doc["modbus"]["port"] | 502;
  deviceConfig.modbus.slaveId = doc["modbus"]["slaveId"] | 1;
  deviceConfig.modbus.timeout = doc["modbus"]["timeout"] | 2000;

  deviceConfig.relays.restoreStateOnBoot = doc["relays"]["restoreStateOnBoot"] | true;
  if (doc["relays"].containsKey("names")) {
    JsonArray namesArr = doc["relays"]["names"];
    for (int i = 0; i < 16 && i < (int)namesArr.size(); i++) {
      deviceConfig.relays.names[i] = namesArr[i].as<String>();
    }
  } else {
    for (int i = 0; i < 16; i++) {
      if (i == 2) deviceConfig.relays.names[i] = "SALA";
      else deviceConfig.relays.names[i] = "RELÉ " + String(i + 1);
    }
  }

  deviceConfig.inputs.mode = doc["inputs"]["mode"] | 0;
  if (doc["inputs"].containsKey("modes")) {
    JsonArray modesArr = doc["inputs"]["modes"];
    for (int i = 0; i < 16 && i < (int)modesArr.size(); i++) {
      deviceConfig.inputs.modes[i] = modesArr[i].as<int>();
    }
  } else {
    for (int i = 0; i < 16; i++) {
      deviceConfig.inputs.modes[i] = deviceConfig.inputs.mode;
    }
  }
  deviceConfig.inputs.debounceMs = doc["inputs"]["debounceMs"] | 25;
}

// Salva as configurações atuais no arquivo JSON do LittleFS
bool salvarConfiguracao() {
  DynamicJsonDocument doc(4096);
  
  JsonObject sta = doc.createNestedObject("station");
  sta["active"] = deviceConfig.station.active;
  sta["ssid"] = deviceConfig.station.ssid;
  sta["pass"] = deviceConfig.station.pass;
  sta["dhcp"] = deviceConfig.station.dhcp;
  sta["ip"] = deviceConfig.station.ip;
  sta["gw"] = deviceConfig.station.gw;
  sta["sn"] = deviceConfig.station.sn;
  sta["dns"] = deviceConfig.station.dns;

  JsonObject eth = doc.createNestedObject("eth");
  eth["active"] = deviceConfig.eth.active;
  eth["dhcp"] = deviceConfig.eth.dhcp;
  eth["ip"] = deviceConfig.eth.ip;
  eth["gw"] = deviceConfig.eth.gw;
  eth["sn"] = deviceConfig.eth.sn;
  eth["dns"] = deviceConfig.eth.dns;
  eth["webPort"] = deviceConfig.eth.webPort;
  eth["alexaPort"] = deviceConfig.eth.alexaPort;

  JsonObject ap = doc.createNestedObject("ap");
  ap["enabled"] = deviceConfig.ap.enabled;
  ap["ssid"] = deviceConfig.ap.ssid;
  ap["pass"] = deviceConfig.ap.pass;
  ap["ip"] = deviceConfig.ap.ip;
  ap["gw"] = deviceConfig.ap.gw;
  ap["sn"] = deviceConfig.ap.sn;

  JsonObject sec = doc.createNestedObject("security");
  sec["enabled"] = deviceConfig.security.enabled;
  sec["user"] = deviceConfig.security.user;
  sec["pass"] = deviceConfig.security.pass;

  JsonObject mb = doc.createNestedObject("modbus");
  mb["enabled"] = deviceConfig.modbus.enabled;
  mb["ip"] = deviceConfig.modbus.ip;
  mb["port"] = deviceConfig.modbus.port;
  mb["slaveId"] = deviceConfig.modbus.slaveId;
  mb["timeout"] = deviceConfig.modbus.timeout;

  JsonObject rls = doc.createNestedObject("relays");
  rls["restoreStateOnBoot"] = deviceConfig.relays.restoreStateOnBoot;
  JsonArray names = rls.createNestedArray("names");
  for (int i = 0; i < 16; i++) {
    names.add(deviceConfig.relays.names[i]);
  }

  JsonObject inps = doc.createNestedObject("inputs");
  inps["mode"] = deviceConfig.inputs.modes[0];
  JsonArray modesArr = inps.createNestedArray("modes");
  for (int i = 0; i < 16; i++) {
    modesArr.add(deviceConfig.inputs.modes[i]);
  }
  inps["debounceMs"] = deviceConfig.inputs.debounceMs;

  File f = LittleFS.open(CONFIG_FILE, "w");
  if (!f) return false;
  serializeJson(doc, f);
  f.close();
  return true;
}

Config deviceConfig;                           // Instanciação da estrutura de configuração

#endif                                         // Fim do bloco CONFIG_H
