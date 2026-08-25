#include <Arduino.h>             // Biblioteca base do ecossistema Arduino/ESP32
#include <Wire.h>                // Biblioteca I2C para acionamento físico dos chips PCF8574 na KC868-A16
#include <WiFi.h>                // Biblioteca nativa para controle de rádio Wi-Fi
#include <ETH.h>                 // Biblioteca nativa para controle da interface Ethernet física LAN8720
#include <WiFi.h>                // Biblioteca nativa para controle de rádio Wi-Fi
#include <WebServer.h>           // Biblioteca nativa para Servidor HTTP síncrono na porta 80
#include <LittleFS.h>            // Sistema de arquivos moderno na memória Flash SPI do ESP32
#include "config.h"              // Arquivo de cabeçalho contendo a estrutura de configuração do sistema
#include "default_web_assets.h"  // Assets Web PROGMEM para auto-recuperação de arquivos no LittleFS



// -----------------------------------------------------------------------------------------
// DEFINIÇÕES DE HARDWARE DA PLACA KC868-A16 (PHY ETHERNET LAN8720)
// -----------------------------------------------------------------------------------------
//#define ETH_PHY_TYPE      ETH_PHY_LAN8720       // Tipo do chip transceptor Ethernet (LAN8720A)
//#define ETH_PHY_ADDR      1                     // Endereço I2C/SMI do PHY no barramento interno (Geralmente 0)
//#define ETH_PHY_MDC       23                    // Pino GPIO do clock de gerenciamento MDC
//#define ETH_PHY_MDIO      18                    // Pino GPIO de dados de gerenciamento MDIO
//#define ETH_PHY_POWER     16                    // Pino de controle de energia do PHY (16 =  utilizado)
//#define ETH_CLK_MODE      ETH_CLOCK_GPIO0_IN  // Modo de clock do oscilador de 50MHz via GPIO0


#define ETH_PHY_TYPE  ETH_PHY_LAN8720 // Tipo do chip transceptor Ethernet (LAN8720A)
#define ETH_PHY_ADDR  1               // Endereço I2C/SMI do PHY no barramento interno (Geralmente 0)
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_PHY_POWER 16
#define ETH_CLK_MODE  ETH_CLOCK_GPIO0_IN


// -----------------------------------------------------------------------------------------
// CONTROLE DE ATUALIZAÇÃO E FORMATAÇÃO AUTOMÁTICA DO LITTLEFS NO BOOT
// true  = Formata o LittleFS no boot e regrava os arquivos mais recentes de default_web_assets.h
// false = Mantém os arquivos existentes e apenas cria os que estiverem ausentes
// -----------------------------------------------------------------------------------------

#define FORMATAR_LITTLEFS_NO_BOOT true



// -----------------------------------------------------------------------------------------
// INSTÂNCIAS DE SERVIÇOS DE REDE E SERVIDORES
// -----------------------------------------------------------------------------------------
WebServer server(80);                           // Instancia o servidor Web HTTP na porta padrão 80


// -----------------------------------------------------------------------------------------
// PROTOTIPOS DE FUNÇÕES ESSENCIAIS 
// -----------------------------------------------------------------------------------------
void registrarLog(String msg);
void verificarEGravarArquivosLittleFS(bool forcarFormatacao = FORMATAR_LITTLEFS_NO_BOOT);
void onEthEvent(WiFiEvent_t event);
void configurarRotasHttp();

// -----------------------------------------------------------------------------------------
// CONFIGURAÇÕES DE SETUP
//
// Configurações de Serial 115200
// Configurações de Sistema de Arquivos LittleFS
// Configurações de Inicialização da Interface Ethernet Rede Cabeada
// Configurações de Inicialização da Wi-Fi Station
// Configurações de Inicialização de Ativa o Ponto de Acesso (SoftAP) de Manutenção
// -----------------------------------------------------------------------------------------

void setup() {
 
  // 1. Inicializa de Serial 115200
  Serial.begin(115200);                         // Inicializa a porta serial em 115200 bps
  delay(2000);                                  // Aguarda estabilização da tensão objetivo dar tempo para inicialização do  Serial.begin
  Serial.println("\n=== INICIALIZANDO SISTEMA IOT ESP32 ===");


  // 2. Inicializa o Sistema de Arquivos LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("Erro critico ao montar o sistema de arquivos LittleFS! Formatando...");
    LittleFS.format();
    LittleFS.begin(true);
  } else {
   
    Serial.println("LittleFS montado com sucesso.");
  }

  // 3. Formata o LittleFS (se habilitado) e regrava todos os arquivos Web de default_web_assets.h
  verificarEGravarArquivosLittleFS(FORMATAR_LITTLEFS_NO_BOOT);


// 4. Carrega as configurações persistidas da Flash (incluindo nomes e flags)
  carregarConfiguracao();

salvarConfiguracao();

  // 7. Configura e Inicializa a Interface Ethernet LAN8720 (KC868-A16)
 


  if (deviceConfig.eth.active) {
    WiFi.onEvent(onEthEvent);                  // Registra o callback de eventos de rede
    ETH.begin(ETH_PHY_TYPE, ETH_PHY_ADDR, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_POWER, ETH_CLK_MODE);
    
    // Configura IP Estático na Ethernet caso o DHCP esteja desligado
    if (!deviceConfig.eth.dhcp) {
      IPAddress ip, gw, sn, dns;
      ip.fromString(deviceConfig.eth.ip);
      gw.fromString(deviceConfig.eth.gw);
      sn.fromString(deviceConfig.eth.sn);
      dns.fromString(deviceConfig.eth.dns);
      ETH.config(ip, gw, sn, dns);             // Aplica os parâmetros IP à interface física
    }
  }

  // 6. Configura e Inicializa o Wi-Fi Station
  if (deviceConfig.station.active && deviceConfig.station.ssid != "") {
    WiFi.mode(deviceConfig.ap.enabled ? WIFI_AP_STA : WIFI_STA); // Habilita modo híbrido se AP estiver ativo
    
    // Configura IP Estático no Wi-Fi se DHCP estiver desligado
    if (!deviceConfig.station.dhcp) {
      IPAddress ip, gw, sn, dns;
      ip.fromString(deviceConfig.station.ip);
      gw.fromString(deviceConfig.station.gw);
      sn.fromString(deviceConfig.station.sn);
      dns.fromString(deviceConfig.station.dns);
      WiFi.config(ip, gw, sn, dns);
    }
    WiFi.begin(deviceConfig.station.ssid.c_str(), deviceConfig.station.pass.c_str());
    registrarLog("Conectando ao Wi-Fi: " + deviceConfig.station.ssid);
  } else if (deviceConfig.ap.enabled) {
    WiFi.mode(WIFI_AP);                        // Habilita apenas modo Ponto de Acesso
  }

  // 7. Configura e Ativa o Ponto de Acesso (SoftAP) de Manutenção
  if (deviceConfig.ap.enabled) {
    IPAddress apIp, apGw, apSn;
    apIp.fromString(deviceConfig.ap.ip);
    apGw.fromString(deviceConfig.ap.gw);
    apSn.fromString(deviceConfig.ap.sn);
    WiFi.softAPConfig(apIp, apGw, apSn);       // Aplica faixa IP do AP
    WiFi.softAP(deviceConfig.ap.ssid.c_str(), deviceConfig.ap.pass.c_str()); // Sobe a rede sem fio
    registrarLog("Ponto de Acesso ativado: " + deviceConfig.ap.ssid + " | IP: " + deviceConfig.ap.ip);
  }




configurarRotasHttp();
server.begin();                              // Inicia o servidor HTTP na porta 80

}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();                       // Processa requisições HTTP pendentes
   // Monitora e reconecta automaticamente o Wi-Fi se perder a conexão
  if (deviceConfig.station.active && WiFi.status() != WL_CONNECTED && !ethConectada) {
    static unsigned long tempoUltimaTentativaWifi = 0;
    if (millis() - tempoUltimaTentativaWifi > 10000) {
      tempoUltimaTentativaWifi = millis();
      registrarLog("Reconectando ao Wi-Fi...");
      WiFi.reconnect();
    }
  }

}





// -----------------------------------------------------------------------------------------
// ROTAS DO SERVIDOR WEB HTTP (API REST E SERVIÇO DE ARQUIVOS LITTLEFS / PROGMEM)
// -----------------------------------------------------------------------------------------
void configurarRotasHttp() {
  server.enableCORS(true);                     // Habilita CORS para acesso por aplicações externas

  // Handler para a página principal (HTML) com fallback imediato para PROGMEM
  auto handlerIndex = []() {
    if (deviceConfig.security.enabled) {       // Se autenticação estiver ativa
      if (!server.authenticate(deviceConfig.security.user.c_str(), deviceConfig.security.pass.c_str())) {
        return server.requestAuthentication(); // Exige login HTTP
      }
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "0");
    if (LittleFS.exists("/index.html")) {       // Se o arquivo existir na Flash LittleFS
      File f = LittleFS.open("/index.html", "r"); // Abre em modo leitura
      server.streamFile(f, "text/html");       // Envia com streaming otimizado
      f.close();                               // Fecha o arquivo
    } else {
      server.send_P(200, "text/html", DEFAULT_INDEX_HTML); // Fallback instantâneo da Flash ROM
    }
  };

  server.on("/", HTTP_GET, handlerIndex);
  server.on("/index.html", HTTP_GET, handlerIndex);

  // Rota dedicada para CSS com fallback garantido
  server.on("/style.css", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "0");
    if (LittleFS.exists("/style.css")) {
      File f = LittleFS.open("/style.css", "r");
      server.streamFile(f, "text/css");
      f.close();
    } else {
      server.send_P(200, "text/css", DEFAULT_STYLE_CSS);
    }
  });

  // Rota dedicada para JavaScript com fallback garantido
  server.on("/script.js", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "0");
    if (LittleFS.exists("/script.js")) {
      File f = LittleFS.open("/script.js", "r");
      server.streamFile(f, "application/javascript");
      f.close();
    } else {
      server.send_P(200, "application/javascript", DEFAULT_SCRIPT_JS);
    }
  });

 


  // Serve arquivos estáticos (CSS, JS, Imagens) a partir da raiz do LittleFS
  server.serveStatic("/", LittleFS, "/");
}



// -----------------------------------------------------------------------------------------
// GERENCIADOR DE EVENTOS DA INTERFACE ETHERNET LAN8720
// -----------------------------------------------------------------------------------------
void onEthEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      registrarLog("Ethernet LAN8720 inicializada no hardware."); // Evento de subida do driver
      ETH.setHostname("esp32-kc868-industrial");                  // Define o nome de host da placa na rede
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      registrarLog("Cabo de rede Ethernet conectado!");           // Evento de link físico UP
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      registrarLog("IP Ethernet obtido: " + ETH.localIP().toString()); // Evento de endereço IP atribuído
      ethConectada = true;                                        // Marca a interface cabeada como online
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      registrarLog("Cabo Ethernet desconectado.");                // Evento de link físico DOWN
      ethConectada = false;                                       // Marca a interface cabeada como offline
      break;
    case ARDUINO_EVENT_ETH_STOP:
      registrarLog("Ethernet parada.");                           // Evento de desligamento do driver
      ethConectada = false;                                       // Atualiza a flag
      break;
    default:
      break;
  }
}



// -----------------------------------------------------------------------------------------
// FUNÇÃO DE ATUALIZAÇÃO & AUTO-RECUPERAÇÃO: FORMATA E GRAVA ASSETS DO DEFAULT_WEB_ASSETS.H
// -----------------------------------------------------------------------------------------
// Formata o LittleFS no boot para carregar as versões mais recentes dos arquivos Web
// (index.html, style.css, script.js, config.json) armazenados na Flash PROGMEM.
void verificarEGravarArquivosLittleFS(bool forcarFormatacao) {
  if (forcarFormatacao) {
    registrarLog("[LittleFS] Formatando particao LittleFS para atualizacao completa dos assets Web...");
    bool formatOk = LittleFS.format();
    if (formatOk) {
      registrarLog("[LittleFS OK] Particao LittleFS formatada com sucesso.");
    } else {
      registrarLog("[LittleFS AVISO] Formatação retornou falso ou particao ja estava formatada.");
    }
  } else {
    registrarLog("Verificando presenca e integridade dos arquivos Web no LittleFS...");
  }

  // Verifica se o index.html existente no LittleFS é uma versão anterior (ex: sem o painel de sincronismo de inputs)
  bool precisaAtualizarWeb = forcarFormatacao;
  if (!precisaAtualizarWeb && LittleFS.exists("/index.html")) {
    File checkF = LittleFS.open("/index.html", "r");
    if (checkF) {
      String snippet = checkF.readString();
      checkF.close();
      if (snippet.indexOf("inputs-panel") == -1) {
        registrarLog("[LittleFS Auto-Upgrade] Detectada versao anterior de index.html sem painel de inputs. Atualizando assets PROGMEM...");
        precisaAtualizarWeb = true;
      }
    }
  }

  // 1. Grava /config.json
  if (forcarFormatacao || !LittleFS.exists("/config.json")) {
    registrarLog("[Flash Web] Gravando /config.json a partir de default_web_assets.h...");
    File f = LittleFS.open("/config.json", "w");
    if (f) {
      f.print(DEFAULT_CONFIG_JSON);
      f.close();
      registrarLog("[Flash Web OK] /config.json gravado com sucesso.");
    } else {
      registrarLog("[Flash Web ERRO] Falha ao criar /config.json no LittleFS.");
    }
  } else {
    registrarLog("[LittleFS OK] /config.json encontrado.");
  }

  // 2. Grava /index.html
  if (precisaAtualizarWeb || !LittleFS.exists("/index.html")) {
    registrarLog("[Flash Web] Gravando /index.html a partir de default_web_assets.h...");
    File f = LittleFS.open("/index.html", "w");
    if (f) {
      f.print(DEFAULT_INDEX_HTML);
      f.close();
      registrarLog("[Flash Web OK] /index.html gravado com sucesso.");
    } else {
      registrarLog("[Flash Web ERRO] Falha ao criar /index.html no LittleFS.");
    }
  } else {
    registrarLog("[LittleFS OK] /index.html encontrado.");
  }

  // 3. Grava /style.css
  if (precisaAtualizarWeb || !LittleFS.exists("/style.css")) {
    registrarLog("[Flash Web] Gravando /style.css a partir de default_web_assets.h...");
    File f = LittleFS.open("/style.css", "w");
    if (f) {
      f.print(DEFAULT_STYLE_CSS);
      f.close();
      registrarLog("[Flash Web OK] /style.css gravado com sucesso.");
    } else {
      registrarLog("[Flash Web ERRO] Falha ao criar /style.css no LittleFS.");
    }
  } else {
    registrarLog("[LittleFS OK] /style.css encontrado.");
  }

  // 4. Grava /script.js
  if (precisaAtualizarWeb || !LittleFS.exists("/script.js")) {
    registrarLog("[Flash Web] Gravando /script.js a partir de default_web_assets.h...");
    File f = LittleFS.open("/script.js", "w");
    if (f) {
      f.print(DEFAULT_SCRIPT_JS);
      f.close();
      registrarLog("[Flash Web OK] /script.js gravado com sucesso.");
    } else {
      registrarLog("[Flash Web ERRO] Falha ao criar /script.js no LittleFS.");
    }
  } else {
    registrarLog("[LittleFS OK] /script.js encontrado.");
  }

  registrarLog("[LittleFS] Todos os assets Web sincronizados e prontos para o servidor HTTP!");
}





// -----------------------------------------------------------------------------------------
// REGISTRO DE LOGS COM TIMESTAMP E PROTEÇÃO DE MEMÓRIA RAM (HEAP)
// -----------------------------------------------------------------------------------------
void registrarLog(String msg) {
  unsigned long segundos = millis() / 1000;     // Converte o tempo do ESP32 para segundos desde o boot
  String linha = "[" + String(segundos) + "s] " + msg; // Monta a linha com o timestamp formatado
  Serial.println(linha);                        // Envia a mensagem para a porta serial de depuração
  
  // Limita o buffer em 4000 caracteres para impedir estouro de memória (Heap Overflow)
  if (systemLogs.length() > 4000) {
    systemLogs = systemLogs.substring(2000);    // Descarta a metade mais antiga mantendo os dados recentes
  }
  systemLogs += linha + "\n";                   // Adiciona a nova linha de evento ao buffer
}

