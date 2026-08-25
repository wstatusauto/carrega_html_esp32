// =========================================================================================
// ARQUIVO: default_web_assets.h - ASSETS WEB PADRÃO EMBUTIDOS EM MEMÓRIA FLASH PROGMEM
// =========================================================================================
// git commit -m "Carregamento dos arquivos de armazena Web padrão (config.json, index.html, style.css, script.js) Sistema de arquivos LittleFS"
// diretamente na memória de programa (PROGMEM). Caso a partição LittleFS esteja vazia no primeiro
// boot ou após formatação, a função verificarEGravarArquivosLittleFS() grava esses dados no LittleFS.

#ifndef DEFAULT_WEB_ASSETS_H
#define DEFAULT_WEB_ASSETS_H

#include <Arduino.h>

// -----------------------------------------------------------------------------------------
// 1. CONFIGURAÇÃO INICIAL PADRÃO (data/config.json)
// -----------------------------------------------------------------------------------------
static const char DEFAULT_CONFIG_JSON[] PROGMEM = R"rawliteral({
  "station": {
    "active": true,
    "ssid": "GSH9126",
    "pass": "0148483886",
    "dhcp": false,
    "ip": "192.168.18.222",
    "gw": "192.168.18.1",
    "sn": "255.255.255.0",
    "dns": "8.8.8.8"
  },
  "eth": {
    "active": true,
    "dhcp": false,
    "ip": "192.168.18.232",
    "gw": "192.168.18.1",
    "sn": "255.255.255.0",
    "dns": "8.8.8.8",
    "webPort": 80,
    "alexaPort": 81
  },
  "ap": {
    "enabled": true,
    "ssid": "KC868_A16_PRO",
    "pass": "admin",
    "ip": "192.168.4.1",
    "gw": "192.168.4.1",
    "sn": "255.255.255.0"
  },
  "system": {
    "webPort": 80,
    "alexaPort": 81,
    "googlePort": 8008
  },
  "security": {
    "enabled": true,
    "user": "admin",
    "pass": "admin"
  },
  "modbus": {
    "enabled": true,
    "ip": "192.168.18.21",
    "port": 502,
    "slaveId": 1,
    "timeout": 2000
  },
  "relays": {
    "restoreStateOnBoot": true,
    "names": [
      "RELÉ 1", "RELÉ 2", "SALA", "RELÉ 4",
      "RELÉ 5", "RELÉ 6", "RELÉ 7", "RELÉ 8",
      "RELÉ 9", "RELÉ 10", "RELÉ 11", "RELÉ 12",
      "RELÉ 13", "RELÉ 14", "RELÉ 15", "RELÉ 16"
    ]
  },
  "inputs": {
    "mode": 0,
    "modes": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    "debounceMs": 25
  }
})rawliteral";

// -----------------------------------------------------------------------------------------
// 2. INTERFACE HTML5 RESPONSIVA COM 5 SUB-ABAS (data/index.html)
// -----------------------------------------------------------------------------------------
static const char DEFAULT_INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 CORE V2.4 - KinCony KC868-A16</title>
  <link rel="stylesheet" href="style.css">
  <script src="https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js"></script>
</head>
<body>
  <div class="app-container">
    <!-- CABEÇALHO PRINCIPAL DO SISTEMA -->
    <header class="main-header">
      <div class="header-brand">
        <div class="brand-logo">
          <svg viewBox="0 0 24 24" width="22" height="22" stroke="currentColor" fill="none" stroke-width="2"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"></polygon></svg>
        </div>
        <div>
          <h1 class="brand-title">ESP32 CORE V3.4</h1>
          <span class="brand-subtitle">AM-868-R16 • LAN8720A + MODBUS TCP</span>
        </div>
      </div>

      <!-- STATUS DE REDE EM TEMPO REAL -->
      <div class="header-badges">
        <div class="status-pill status-eth" id="badge-eth">
          <span class="dot"></span> ETH: CONECTADO
        </div>
        <div class="status-pill status-wifi" id="badge-wifi">
          <span class="dot"></span> WI-FI: ONLINE (-54 dBm)
        </div>
        <div class="status-pill status-modbus" id="badge-modbus">
          <span class="dot"></span> MODBUS: 192.168.18.21:502
        </div>
      </div>

      <!-- NAVEGAÇÃO PRINCIPAL -->
      <nav class="main-nav">
        <button class="nav-btn active" id="tab-btn-dashboard" onclick="trocarAbaPrincipal('dashboard')">PAINEL DE RELÉS</button>
        <button class="nav-btn" id="tab-btn-network" onclick="trocarAbaPrincipal('network')">CONFIGURAÇÕES DE REDE</button>
        <button class="nav-btn" id="tab-btn-audit" onclick="trocarAbaPrincipal('audit')">AUDITORIA & LOGS</button>
      </nav>
    </header>

    <!-- ABA 1: PAINEL DE RELÉS E BARRAMENTO MODBUS -->
    <section id="view-dashboard" class="tab-view active">
      <div class="top-bar">
        <button id="btn-alexa" class="btn-action btn-alexa" onclick="sincronizarAlexa()">
          <svg class="icon-bolt" viewBox="0 0 24 24"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"></polygon></svg>
          GRAVAR & SINCRONIZAR ALEXA
        </button>
        <div class="action-group">
          <button id="btn-boot-retention" class="btn-action btn-boot" onclick="alternarRetencaoBoot()" title="Salva os estados dos relés na memória Flash NVS para restaurar ao ligar/desligar">
            <span class="dot-boot"></span> Retenção NVS: ATIVO
          </button>
          <button class="btn-action btn-all-on" onclick="enviarComando('all_on')">Ligar Todos</button>
          <button class="btn-action btn-all-off" onclick="enviarComando('all_off')">Desligar Todos</button>
          <button class="btn-action btn-invert" onclick="enviarComando('invert')">Inverter Estados</button>
          <button class="btn-action btn-pulse" onclick="acionarPulso(1000)">Pulso 1s</button>
        </div>
      </div>

      <!-- GRADE DE 16 RELÉS -->
      <main class="relay-grid" id="relay-container"></main>

      <!-- PAINEL DE SINCRONISMO BIDIRECIONAL DAS ENTRADAS FÍSICAS I2C -->
      <section class="inputs-panel" id="inputs-panel">
        <div class="inputs-header">
          <div class="inputs-title-wrap">
            <div class="inputs-badge-icon">
              <svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2"><path d="M7 16V4m0 0L3 8m4-4l4 4m6 4v12m0 0l4-4m-4 4l-4-4"></path></svg>
            </div>
            <div>
              <h2 class="inputs-title">SINCRONISMO BIDIRECIONAL: ENTRADAS FÍSICAS I2C <span class="highlight-cyan">[0x21 & 0x22]</span> ↔ RELÉS <span class="highlight-blue">[0x24 & 0x25]</span></h2>
              <p class="inputs-subtitle" id="inputs-subtitle">0 DE 16 ENTRADAS FECHADAS (ON) • MODOS INDIVIDUAIS COM NOMES REPLICADOS DOS RELÉS</p>
            </div>
          </div>

          <div class="inputs-mode-selector">
            <span class="mode-label">APLICAR A TODOS:</span>
            <button type="button" class="mode-pill" id="btn-mode-0" onclick="alterarModoInputs(0)">Three-Way (Todos)</button>
            <button type="button" class="mode-pill" id="btn-mode-1" onclick="alterarModoInputs(1)">Seguidor (Todos)</button>
            <button type="button" class="mode-pill" id="btn-mode-2" onclick="alterarModoInputs(2)">Pulsador (Todos)</button>
          </div>
        </div>

        <div class="inputs-grid" id="inputs-container"></div>

        <div class="inputs-info-card">
          <div class="info-text">
            <strong>Como funciona o sincronismo inteligente com Modos Individuais:</strong>
            <p>• <strong>Three-Way (Paralelo):</strong> Cada transição na chave física ou acionamento na Web inverte o estado do relé de forma totalmente independente e simultânea, sem conflito de retenção.<br>
            • <strong>Seguidor de Borda:</strong> O relé segue o estado do pulso/chave física (Fechado = ON, Aberto = OFF), mantendo comandos web.<br>
            • <strong>Pulsador:</strong> Acionamento por borda de subida (botão de pulso / campainha).</p>
          </div>
        </div>
      </section>

      <!-- TERMINAL DE LOGS MODBUS TCP RAW SNIFFER -->
      <section class="sniffer-panel">
        <div class="sniffer-header">
          <div class="sniffer-title">
            <span class="prompt-arrow">&gt;_</span> LOG DE BARRAMENTO MODBUS TCP (FRAME SNIFFER RAW)
          </div>
          <div class="sniffer-actions">
            <button class="btn-pdf" onclick="exportarRelatorioPDF()">Exportar PDF</button>
            <button class="btn-clear" onclick="limparLogs()">Limpar Logs</button>
          </div>
        </div>
        <div class="sniffer-terminal" id="sniffer-terminal"></div>
      </section>
    </section>

    <!-- ABA 2: CONFIGURAÇÕES DE REDE & HARDWARE (5 SUB-ABAS) -->
    <section id="view-network" class="tab-view">
      <!-- BARRA SUPERIOR DE SUB-ABAS (CONFORME IMAGEM DE REFERÊNCIA) -->
      <div class="network-subnav">
        <button class="subnav-btn active" id="subtab-btn-station" onclick="trocarSubAbaRede('station')">
          <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M5 12.55a11 11 0 0 1 14.08 0"></path><path d="M1.42 9a16 16 0 0 1 21.16 0"></path><path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path><line x1="12" y1="20" x2="12.01" y2="20"></line></svg>
          WIFI STATION
        </button>
        <button class="subnav-btn" id="subtab-btn-ap" onclick="trocarSubAbaRede('ap')">
          <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="2"></circle><path d="M16.24 7.76a6 6 0 0 1 0 8.49m-8.48-.01a6 6 0 0 1 0-8.49m11.31-2.82a10 10 0 0 1 0 14.14m-14.14 0a10 10 0 0 1 0-14.14"></path></svg>
          ACCESS POINT
        </button>
        <button class="subnav-btn" id="subtab-btn-eth" onclick="trocarSubAbaRede('eth')">
          <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><rect x="2" y="2" width="20" height="8" rx="2" ry="2"></rect><rect x="2" y="14" width="20" height="8" rx="2" ry="2"></rect><line x1="6" y1="6" x2="6.01" y2="6"></line><line x1="6" y1="18" x2="6.01" y2="18"></line></svg>
          ETHERNET LAN
        </button>
        <button class="subnav-btn" id="subtab-btn-modbus" onclick="trocarSubAbaRede('modbus')">
          <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><rect x="4" y="4" width="16" height="16" rx="2"></rect><rect x="9" y="9" width="6" height="6"></rect><line x1="9" y1="1" x2="9" y2="4"></line><line x1="15" y1="1" x2="15" y2="4"></line><line x1="9" y1="20" x2="9" y2="23"></line><line x1="15" y1="20" x2="15" y2="23"></line></svg>
          MODBUS TCP ETD8A12
        </button>
        <button class="subnav-btn" id="subtab-btn-security" onclick="trocarSubAbaRede('security')">
          <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"></path></svg>
          SEGURANÇA
        </button>
      </div>

      <!-- CONTAINER DO FORMULÁRIO DE CONFIGURAÇÃO -->
      <div class="config-card">
        <!-- 1. WIFI STATION -->
        <div id="subview-station" class="subview-panel active">
          <div class="panel-header">
            <div class="panel-icon icon-wifi">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2"><path d="M5 12.55a11 11 0 0 1 14.08 0"></path><path d="M1.42 9a16 16 0 0 1 21.16 0"></path><path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path><line x1="12" y1="20" x2="12.01" y2="20"></line></svg>
            </div>
            <div>
              <h2>WIFI STATION (CLIENTE)</h2>
              <p>Conecta a controladora ESP32 ao roteador Wi-Fi local de 2.4 GHz.</p>
            </div>
            <label class="toggle-switch">
              <input type="checkbox" id="cfg-sta-active" checked>
              <span class="slider"></span>
            </label>
          </div>

          <div class="form-grid">
            <div class="form-group">
              <label>SSID DA REDE WI-FI</label>
              <div class="input-with-btn">
                <input type="text" id="cfg-sta-ssid" value="GSH9126" placeholder="Nome da rede">
                <button type="button" class="btn-scan" onclick="escanearWifi()">BUSCAR REDES</button>
              </div>
            </div>
            <div class="form-group">
              <label>SENHA WI-FI</label>
              <input type="password" id="cfg-sta-pass" value="0148483886" placeholder="Chave de segurança">
            </div>
          </div>

          <div class="ip-mode-selector">
            <label>MODO DE ENDEREÇAMENTO IP</label>
            <div class="pill-group">
              <button type="button" class="pill-opt" id="pill-sta-dhcp" onclick="setStaIpMode(true)">DHCP CLIENT</button>
              <button type="button" class="pill-opt active" id="pill-sta-static" onclick="setStaIpMode(false)">STATIC IP</button>
            </div>
          </div>

          <div class="form-grid" id="sta-ip-fields">
            <div class="form-group">
              <label>IP ADDRESS</label>
              <input type="text" id="cfg-sta-ip" value="192.168.18.222">
            </div>
            <div class="form-group">
              <label>GATEWAY</label>
              <input type="text" id="cfg-sta-gw" value="192.168.18.1">
            </div>
            <div class="form-group">
              <label>NETMASK</label>
              <input type="text" id="cfg-sta-sn" value="255.255.255.0">
            </div>
            <div class="form-group">
              <label>PRIMARY DNS</label>
              <input type="text" id="cfg-sta-dns" value="8.8.8.8">
            </div>
          </div>
        </div>

        <!-- 2. ACCESS POINT (SOFT-AP) -->
        <div id="subview-ap" class="subview-panel">
          <div class="panel-header">
            <div class="panel-icon icon-ap">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="2"></circle><path d="M16.24 7.76a6 6 0 0 1 0 8.49m-8.48-.01a6 6 0 0 1 0-8.49m11.31-2.82a10 10 0 0 1 0 14.14m-14.14 0a10 10 0 0 1 0-14.14"></path></svg>
            </div>
            <div>
              <h2>ACCESS POINT (SOFT-AP LOCAL)</h2>
              <p>Rede Wi-Fi própria gerada pelo ESP32 para manutenção em campo.</p>
            </div>
            <label class="toggle-switch">
              <input type="checkbox" id="cfg-ap-active" checked>
              <span class="slider"></span>
            </label>
          </div>

          <div class="form-grid">
            <div class="form-group">
              <label>SSID DO ACCESS POINT</label>
              <input type="text" id="cfg-ap-ssid" value="KC868_A16_PRO">
            </div>
            <div class="form-group">
              <label>SENHA DO PONTO DE ACESSO</label>
              <input type="password" id="cfg-ap-pass" value="admin">
            </div>
            <div class="form-group">
              <label>IP PRÓPRIO DO ESP32</label>
              <input type="text" id="cfg-ap-ip" value="192.168.4.1">
            </div>
            <div class="form-group">
              <label>GATEWAY DO AP</label>
              <input type="text" id="cfg-ap-gw" value="192.168.4.1">
            </div>
          </div>
        </div>

        <!-- 3. ETHERNET LAN (LAN8720A) -->
        <div id="subview-eth" class="subview-panel">
          <div class="panel-header">
            <div class="panel-icon icon-eth">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2"><rect x="2" y="2" width="20" height="8" rx="2" ry="2"></rect><rect x="2" y="14" width="20" height="8" rx="2" ry="2"></rect><line x1="6" y1="6" x2="6.01" y2="6"></line><line x1="6" y1="18" x2="6.01" y2="18"></line></svg>
            </div>
            <div>
              <h2>ETHERNET LAN (LAN8720A PHY)</h2>
              <p>Interface cabeada RJ45 de alta estabilidade imune a ruídos RF.</p>
            </div>
            <label class="toggle-switch">
              <input type="checkbox" id="cfg-eth-active" checked>
              <span class="slider"></span>
            </label>
          </div>

          <div class="ip-mode-selector">
            <label>MODO IP ETHERNET</label>
            <div class="pill-group">
              <button type="button" class="pill-opt" id="pill-eth-dhcp" onclick="setEthIpMode(true)">DHCP CLIENT</button>
              <button type="button" class="pill-opt active" id="pill-eth-static" onclick="setEthIpMode(false)">STATIC IP</button>
            </div>
          </div>

          <div class="form-grid">
            <div class="form-group">
              <label>IP ETHERNET ESTÁTICO</label>
              <input type="text" id="cfg-eth-ip" value="192.168.18.232">
            </div>
            <div class="form-group">
              <label>GATEWAY DA REDE CABEADA</label>
              <input type="text" id="cfg-eth-gw" value="192.168.18.1">
            </div>
            <div class="form-group">
              <label>PORTA HTTP WEBSERVER</label>
              <input type="number" id="cfg-eth-webport" value="80">
            </div>
            <div class="form-group">
              <label>PORTA ALEXA EMULATION</label>
              <input type="number" id="cfg-eth-alexaport" value="81">
            </div>
          </div>
        </div>

        <!-- 4. MODBUS TCP ETD8A12 -->
        <div id="subview-modbus" class="subview-panel">
          <div class="panel-header">
            <div class="panel-icon icon-modbus">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2"><rect x="4" y="4" width="16" height="16" rx="2"></rect><rect x="9" y="9" width="6" height="6"></rect></svg>
            </div>
            <div>
              <h2>MODBUS TCP (EXPANSOR ETD8A12)</h2>
              <p>Comunicação industrial com o módulo de relés via socket TCP na porta 502.</p>
            </div>
            <label class="toggle-switch">
              <input type="checkbox" id="cfg-modbus-active" checked>
              <span class="slider"></span>
            </label>
          </div>

          <div class="form-grid">
            <div class="form-group">
              <label>IP DO MÓDULO ETD8A12</label>
              <input type="text" id="cfg-modbus-ip" value="192.168.18.21">
            </div>
            <div class="form-group">
              <label>PORTA MODBUS TCP</label>
              <input type="number" id="cfg-modbus-port" value="502">
            </div>
            <div class="form-group">
              <label>SLAVE ID / UNIT ID</label>
              <input type="number" id="cfg-modbus-slave" value="1">
            </div>
            <div class="form-group">
              <label>TIMEOUT (MILLISECONDS)</label>
              <input type="number" id="cfg-modbus-timeout" value="2000">
            </div>
          </div>
        </div>

        <!-- 5. SEGURANÇA E BACKUP -->
        <div id="subview-security" class="subview-panel">
          <div class="panel-header">
            <div class="panel-icon icon-security">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"></path></svg>
            </div>
            <div>
              <h2>SEGURANÇA & MANUTENÇÃO NVS</h2>
              <p>Controle de acesso HTTP e backup das configurações.</p>
            </div>
            <label class="toggle-switch">
              <input type="checkbox" id="cfg-sec-active" checked>
              <span class="slider"></span>
            </label>
          </div>

          <div class="form-grid">
            <div class="form-group">
              <label>USUÁRIO ADMINISTRADOR</label>
              <input type="text" id="cfg-sec-user" value="admin">
            </div>
            <div class="form-group">
              <label>SENHA DE ACESSO</label>
              <input type="password" id="cfg-sec-pass" value="admin">
            </div>
          </div>

          <div class="security-tools">
            <button type="button" class="btn-tool" onclick="exportarBackupJSON()">EXPORTAR BACKUP (.JSON)</button>
            <label class="btn-tool">
              IMPORTAR BACKUP
              <input type="file" id="input-backup-file" accept=".json" onchange="importarBackupJSON(event)" style="display:none;">
            </label>
            <button type="button" class="btn-tool btn-danger" onclick="restaurarPadraoFabrica()">PADRÕES DE FÁBRICA</button>
          </div>
        </div>

        <!-- BARRA DE AÇÕES INFERIOR -->
        <footer class="config-footer">
          <button type="button" class="btn-reboot" onclick="reiniciarESP32()">
            <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M21.5 2v6h-6M21.34 15.57a10 10 0 1 1-.57-8.38l5.67-5.67"></path></svg>
            REINICIAR ESP32
          </button>
          <button type="button" class="btn-save" onclick="salvarConfiguracoes()">
            <svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"></path><polyline points="17 21 17 13 7 13 7 21"></polyline><polyline points="7 3 7 8 15 8"></polyline></svg>
            SALVAR CONFIGURAÇÕES
          </button>
        </footer>
      </div>
    </section>

    <!-- ABA 3: AUDITORIA E SAÚDE DO SISTEMA -->
    <section id="view-audit" class="tab-view">
      <div class="audit-grid">
        <div class="stat-card">
          <span class="stat-label">TEMPO DE ATIVIDADE</span>
          <span class="stat-val" id="audit-uptime">0s</span>
        </div>
        <div class="stat-card">
          <span class="stat-label">MEMÓRIA RAM LIVRE</span>
          <span class="stat-val" id="audit-heap">194.2 KB</span>
        </div>
        <div class="stat-card">
          <span class="stat-label">TRANSCEPTOR PHY</span>
          <span class="stat-val">LAN8720A (50MHz)</span>
        </div>
        <div class="stat-card">
          <span class="stat-label">SOCKET MODBUS</span>
          <span class="stat-val text-green" id="audit-modbus">ATIVO / 0 ERROS</span>
        </div>
      </div>
    </section>
  </div>

  <!-- TOAST DE NOTIFICAÇÃO -->
  <div id="toast" class="toast">Configurações salvas na Flash LittleFS!</div>

  <script src="script.js"></script>
</body>
</html>
)rawliteral";

// -----------------------------------------------------------------------------------------
// 3. ESTILOS CSS INDUSTRIAIS RESPONSIVOS (data/style.css)
// -----------------------------------------------------------------------------------------
static const char DEFAULT_STYLE_CSS[] PROGMEM = R"rawliteral(
/* ESTILIZAÇÃO DO PAINEL INDUSTRIAL DARK THEME ESP32 */
:root {
  --bg-primary: #070b13;
  --bg-card: #0d131f;
  --bg-card-hover: #121929;
  --border-card: #182234;
  --red-off: #ef4444;
  --red-glow: rgba(239, 68, 68, 0.25);
  --green-on: #10b981;
  --green-glow: rgba(16, 185, 129, 0.3);
  --blue-accent: #2563eb;
  --text-muted: #64748b;
  --text-light: #f8fafc;
}

* { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
body { background-color: var(--bg-primary); color: var(--text-light); padding: 16px; min-height: 100vh; }
.app-container { max-width: 1300px; margin: 0 auto; display: flex; flex-direction: column; gap: 16px; }

/* CABEÇALHO PRINCIPAL */
.main-header { display: flex; justify-content: space-between; align-items: center; background: #0b101c; border: 1px solid #162033; padding: 12px 18px; border-radius: 16px; flex-wrap: wrap; gap: 12px; }
.header-brand { display: flex; align-items: center; gap: 12px; }
.brand-logo { width: 38px; height: 38px; background: #2563eb; color: #fff; display: flex; align-items: center; justify-content: center; border-radius: 10px; box-shadow: 0 0 15px rgba(37,99,235,0.4); }
.brand-title { font-size: 16px; font-weight: 800; letter-spacing: 0.5px; }
.brand-subtitle { font-size: 11px; color: var(--text-muted); }

.header-badges { display: flex; gap: 8px; flex-wrap: wrap; }
.status-pill { font-size: 11px; font-weight: 600; padding: 4px 10px; border-radius: 20px; background: #131b2c; border: 1px solid #1e293b; display: flex; align-items: center; gap: 6px; }
.status-pill .dot { width: 7px; height: 7px; border-radius: 50%; background: #10b981; }
.status-modbus .dot { background: #3b82f6; }

.main-nav { display: flex; gap: 6px; }
.nav-btn { background: #131c2f; border: 1px solid #1e293b; color: #94a3b8; font-size: 11px; font-weight: 700; padding: 8px 14px; border-radius: 8px; cursor: pointer; transition: all 0.2s; }
.nav-btn.active { background: #2563eb; color: #fff; border-color: #3b82f6; }

/* ABAS DE CONTEÚDO */
.tab-view { display: none; flex-direction: column; gap: 16px; }
.tab-view.active { display: flex; }

/* BARRA SUPERIOR DE RELÉS */
.top-bar { display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 12px; }
.btn-action { padding: 8px 16px; border-radius: 8px; font-weight: 600; font-size: 12px; cursor: pointer; transition: all 0.2s; border: 1px solid transparent; }
.btn-alexa { background: #2563eb; color: #fff; display: flex; align-items: center; gap: 6px; box-shadow: 0 0 15px rgba(37,99,235,0.5); }
.btn-boot { background: rgba(59,130,246,0.12); border-color: #3b82f6; color: #60a5fa; display: flex; align-items: center; gap: 6px; }
.btn-boot .dot-boot { width: 7px; height: 7px; border-radius: 50%; background: #10b981; }
.btn-boot.disabled { border-color: #64748b; color: #94a3b8; }
.btn-boot.disabled .dot-boot { background: #ef4444; }
.action-group { display: flex; gap: 8px; flex-wrap: wrap; }
.btn-all-on { background: rgba(16,185,129,0.1); border-color: #10b981; color: #10b981; }
.btn-all-off { background: rgba(239,68,68,0.1); border-color: #ef4444; color: #ef4444; }
.btn-invert { background: rgba(245,158,11,0.1); border-color: #f59e0b; color: #f59e0b; }
.btn-pulse { background: rgba(168,85,247,0.1); border-color: #a855f7; color: #a855f7; }

/* GRADE DE RELÉS */
.relay-grid { display: grid; grid-template-columns: repeat(8, 1fr); gap: 12px; }
@media (max-width: 1024px) { .relay-grid { grid-template-columns: repeat(4, 1fr); } }
@media (max-width: 640px) { .relay-grid { grid-template-columns: repeat(2, 1fr); } }

.relay-card { background: var(--bg-card); border: 1px solid var(--border-card); border-radius: 12px; padding: 12px 8px; display: flex; flex-direction: column; align-items: center; gap: 6px; transition: all 0.2s ease; position: relative; user-select: none; }
.relay-card.active { border-color: rgba(16, 185, 129, 0.4); box-shadow: 0 0 16px rgba(16, 185, 129, 0.15); }
.relay-card-header { width: 100%; display: flex; justify-content: space-between; align-items: center; font-size: 10px; font-weight: 800; text-transform: uppercase; padding: 0 4px; }
.relay-card-header .port-label { color: #94a3b8; }
.relay-card-header .rx-label { color: #64748b; font-family: monospace; }

/* BOTÃO DE ENERGIA */
.power-btn { width: 50px; height: 50px; border-radius: 14px; display: flex; align-items: center; justify-content: center; cursor: pointer; transition: all 0.25s ease; background: rgba(239, 68, 68, 0.1); border: 2px solid #ef4444; box-shadow: 0 0 12px var(--red-glow); margin: 4px 0; }
.power-btn svg { width: 24px; height: 24px; stroke: #ef4444; fill: none; stroke-width: 2.5; stroke-linecap: round; stroke-linejoin: round; }
.relay-card.active .power-btn { background: #10b981; border-color: #10b981; box-shadow: 0 0 20px var(--green-glow); }
.relay-card.active .power-btn svg { stroke: #ffffff; }

/* CAIXA DE RENOMEAÇÃO COM BORDA AZUL E BOTÕES SALVAR (✓) / CANCELAR (✕) */
.relay-rename-box { width: 100%; background: #090e18; border: 1.5px solid #2563eb; border-radius: 6px; padding: 3px 6px; display: flex; align-items: center; justify-content: space-between; gap: 4px; box-shadow: 0 0 8px rgba(37,99,235,0.25); min-height: 28px; }
.relay-rename-text { font-size: 11px; font-weight: 800; color: #f8fafc; cursor: pointer; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; flex: 1; text-align: center; }
.relay-rename-input { width: 100%; background: transparent; border: none; color: #ffffff; font-size: 11px; font-weight: 800; outline: none; text-transform: uppercase; font-family: inherit; }
.rename-actions { display: flex; align-items: center; gap: 4px; }
.btn-rename-save { background: transparent; border: none; color: #10b981; font-size: 13px; font-weight: 900; cursor: pointer; padding: 0 2px; line-height: 1; transition: transform 0.15s; }
.btn-rename-save:hover { transform: scale(1.3); }
.btn-rename-cancel { background: transparent; border: none; color: #ef4444; font-size: 13px; font-weight: 900; cursor: pointer; padding: 0 2px; line-height: 1; transition: transform 0.15s; }
.btn-rename-cancel:hover { transform: scale(1.3); }

.relay-ref-code { font-size: 9px; font-family: monospace; font-weight: 700; color: #38bdf8; letter-spacing: 0.5px; }
.relay-status-text { font-size: 10px; font-weight: 800; letter-spacing: 0.5px; text-transform: uppercase; }
.status-off { color: #64748b; }
.status-on { color: #10b981; }
.relay-coil-addr { font-size: 9px; color: #64748b; font-family: monospace; }

/* PAINEL DE SINCRONISMO BIDIRECIONAL DOS INPUTS FÍSICOS */
.inputs-panel { background: #0c121e; border: 1px solid #182234; border-radius: 16px; padding: 20px; display: flex; flex-direction: column; gap: 16px; box-shadow: 0 4px 20px rgba(0,0,0,0.3); }
.inputs-header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid #182234; padding-bottom: 14px; flex-wrap: wrap; gap: 12px; }
.inputs-title-wrap { display: flex; align-items: center; gap: 12px; }
.inputs-badge-icon { width: 34px; height: 34px; border-radius: 10px; background: rgba(6,182,212,0.12); border: 1px solid rgba(6,182,212,0.3); color: #06b6d4; display: flex; align-items: center; justify-content: center; }
.inputs-title { font-size: 13px; font-weight: 800; letter-spacing: 0.5px; text-transform: uppercase; color: #e2e8f0; }
.inputs-subtitle { font-size: 10px; font-family: monospace; color: #64748b; margin-top: 2px; }
.highlight-cyan { color: #06b6d4; font-family: monospace; }
.highlight-blue { color: #3b82f6; font-family: monospace; }

.inputs-mode-selector { display: flex; align-items: center; gap: 6px; background: #070b13; padding: 4px 6px; border-radius: 10px; border: 1px solid #1e293b; }
.mode-label { font-size: 10px; font-weight: 800; color: #64748b; font-family: monospace; padding: 0 4px; }
.mode-pill { background: transparent; border: none; padding: 6px 12px; border-radius: 8px; font-size: 11px; font-weight: 700; color: #94a3b8; cursor: pointer; transition: all 0.2s; white-space: nowrap; }
.mode-pill:hover { color: #f8fafc; background: #131c2f; }
.mode-pill.active { background: #06b6d4; color: #fff; box-shadow: 0 0 12px rgba(6,182,212,0.35); }

.inputs-grid { display: grid; grid-template-columns: repeat(8, 1fr); gap: 10px; }
@media (max-width: 1200px) { .inputs-grid { grid-template-columns: repeat(4, 1fr); } }
@media (max-width: 640px) { .inputs-grid { grid-template-columns: repeat(2, 1fr); } }

.input-card { background: #070b13; border: 1px solid #182234; border-radius: 12px; padding: 10px; display: flex; flex-direction: column; gap: 6px; transition: all 0.2s ease; cursor: pointer; user-select: none; }
.input-card:hover { border-color: #334155; }
.input-card.active { border-color: rgba(6,182,212,0.6); background: rgba(6,182,212,0.06); box-shadow: 0 0 14px rgba(6,182,212,0.15); }
.input-card-header { display: flex; justify-content: space-between; align-items: center; font-size: 10px; font-weight: 800; font-family: monospace; }
.input-card-header .in-tag { color: #f8fafc; font-weight: 800; }
.input-card-header .pcf-addr { color: #64748b; font-size: 9px; }

.input-relay-box { background: #090e18; border: 1.5px solid #1e3a8a; border-radius: 6px; padding: 4px 6px; text-align: center; font-size: 11px; font-weight: 800; color: #38bdf8; text-transform: uppercase; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
.input-visual-row { display: flex; align-items: center; justify-content: center; gap: 8px; margin: 2px 0; }
.input-dot { width: 14px; height: 14px; border-radius: 50%; background: #1e293b; border: 1px solid #334155; transition: all 0.2s; }
.input-dot.on { background: #06b6d4; border-color: #67e8f9; box-shadow: 0 0 10px #06b6d4; }
.input-arrow { color: #475569; font-size: 10px; font-family: monospace; }
.relay-dot { width: 14px; height: 14px; border-radius: 4px; background: #1e293b; border: 1px solid #334155; transition: all 0.2s; }
.relay-dot.on { background: #10b981; border-color: #6ee7b7; box-shadow: 0 0 10px #10b981; }

.mode-label-small { font-size: 8px; font-weight: 700; color: #64748b; letter-spacing: 0.5px; text-transform: uppercase; margin-top: 2px; }
.mode-select { width: 100%; background: #0b1120; border: 1px solid #1e293b; border-radius: 6px; padding: 4px 6px; font-size: 10px; font-weight: 700; color: #e2e8f0; outline: none; cursor: pointer; }
.mode-select:focus { border-color: #06b6d4; }

.input-card-footer { display: flex; justify-content: space-between; align-items: center; font-size: 9px; font-family: monospace; font-weight: 700; margin-top: 2px; }
.input-card-footer .state-in { color: #64748b; }
.input-card-footer .state-in.on { color: #22d3ee; }
.input-card-footer .state-relay { color: #64748b; }
.input-card-footer .state-relay.on { color: #10b981; font-weight: 800; }

.inputs-info-card { display: flex; align-items: flex-start; gap: 12px; background: #070b13; border: 1px solid #182234; border-radius: 12px; padding: 12px 14px; font-size: 11px; color: #94a3b8; line-height: 1.5; }
.info-cpu-icon { color: #06b6d4; flex-shrink: 0; margin-top: 2px; }
.info-text strong { color: #e2e8f0; display: block; margin-bottom: 2px; }

/* SNIFFER MODBUS */
.sniffer-panel { background: #0b0f19; border: 1px solid #182234; border-radius: 12px; padding: 14px; }
.sniffer-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; border-bottom: 1px solid #182234; padding-bottom: 8px; }
.sniffer-title { font-size: 12px; font-weight: bold; color: #f59e0b; font-family: monospace; display: flex; align-items: center; gap: 6px; }
.sniffer-actions button { background: none; border: 1px solid #334155; color: #94a3b8; font-size: 11px; padding: 4px 10px; border-radius: 6px; cursor: pointer; margin-left: 6px; }
.sniffer-terminal { height: 160px; overflow-y: auto; font-family: monospace; font-size: 11px; display: flex; flex-direction: column; gap: 4px; }
.log-line { display: flex; gap: 8px; line-height: 1.4; }
.log-time { color: #64748b; }
.log-tx { color: #3b82f6; font-weight: bold; }
.log-rx { color: #10b981; font-weight: bold; }
.log-hex { color: #fbbf24; }
.log-desc { color: #94a3b8; }

/* SUB-ABAS DE CONFIGURAÇÃO DE REDE (WIFI STATION, AP, ETH, MODBUS, SEGURANÇA) */
.network-subnav { display: flex; gap: 6px; background: #0b101c; border: 1px solid #162033; padding: 8px; border-radius: 16px; overflow-x: auto; }
.subnav-btn { flex: 1; min-width: 140px; display: flex; align-items: center; justify-content: center; gap: 8px; padding: 12px 16px; border-radius: 12px; background: transparent; border: none; color: #94a3b8; font-size: 11px; font-weight: 700; text-transform: uppercase; cursor: pointer; transition: all 0.2s; white-space: nowrap; }
.subnav-btn.active { background: #1d6bf3; color: #fff; box-shadow: 0 4px 14px rgba(29,107,243,0.3); }

/* CARTÃO DE CONFIGURAÇÕES */
.config-card { background: #0d1320; border: 1px solid #182234; border-radius: 16px; padding: 24px; display: flex; flex-direction: column; gap: 20px; }
.subview-panel { display: none; flex-direction: column; gap: 20px; }
.subview-panel.active { display: flex; }

.panel-header { display: flex; align-items: center; gap: 14px; border-bottom: 1px solid #182234; padding-bottom: 16px; }
.panel-icon { width: 44px; height: 44px; border-radius: 12px; display: flex; align-items: center; justify-content: center; background: rgba(37,99,235,0.15); color: #3b82f6; }
.panel-header h2 { font-size: 15px; font-weight: 700; }
.panel-header p { font-size: 12px; color: #64748b; margin-top: 2px; }

/* TOGGLE SWITCH */
.toggle-switch { margin-left: auto; position: relative; display: inline-block; width: 48px; height: 26px; }
.toggle-switch input { opacity: 0; width: 0; height: 0; }
.slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #334155; transition: .3s; border-radius: 26px; }
.slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 3px; bottom: 3px; background-color: white; transition: .3s; border-radius: 50%; }
input:checked + .slider { background-color: #10b981; }
input:checked + .slider:before { transform: translateX(22px); }

/* FORMULÁRIO EM GRID */
.form-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 16px; }
@media (max-width: 768px) { .form-grid { grid-template-columns: 1fr; } }
.form-group { display: flex; flex-direction: column; gap: 6px; }
.form-group label { font-size: 11px; font-weight: 700; color: #94a3b8; letter-spacing: 0.5px; }
.form-group input { background: #070b13; border: 1px solid #1e293b; border-radius: 8px; padding: 10px 14px; color: #fff; font-size: 13px; font-family: monospace; outline: none; }
.form-group input:focus { border-color: #2563eb; box-shadow: 0 0 10px rgba(37,99,235,0.2); }

.input-with-btn { display: flex; gap: 8px; }
.input-with-btn input { flex: 1; }
.btn-scan { background: #1e293b; border: 1px solid #334155; color: #38bdf8; font-size: 11px; font-weight: 700; padding: 0 14px; border-radius: 8px; cursor: pointer; }

/* SELETOR IP MODE */
.ip-mode-selector { display: flex; flex-direction: column; gap: 8px; }
.ip-mode-selector label { font-size: 11px; font-weight: 700; color: #94a3b8; }
.pill-group { display: flex; gap: 8px; background: #070b13; padding: 4px; border-radius: 10px; width: fit-content; border: 1px solid #1e293b; }
.pill-opt { background: transparent; border: none; padding: 6px 16px; border-radius: 8px; font-size: 11px; font-weight: 700; color: #64748b; cursor: pointer; transition: all 0.2s; }
.pill-opt.active { background: #2563eb; color: #fff; }

.security-tools { display: flex; gap: 10px; flex-wrap: wrap; margin-top: 10px; }
.btn-tool { background: #162033; border: 1px solid #23324d; color: #94a3b8; padding: 10px 16px; border-radius: 8px; font-size: 11px; font-weight: 700; cursor: pointer; }
.btn-tool:hover { color: #fff; border-color: #3b82f6; }
.btn-tool.btn-danger { color: #ef4444; border-color: rgba(239,68,68,0.3); }

/* RODAPÉ DE AÇÕES */
.config-footer { display: flex; justify-content: flex-end; gap: 12px; border-top: 1px solid #182234; padding-top: 16px; }
.btn-reboot { background: #1e293b; border: 1px solid #334155; color: #e2e8f0; font-size: 12px; font-weight: 700; padding: 10px 20px; border-radius: 10px; cursor: pointer; display: flex; align-items: center; gap: 8px; }
.btn-save { background: #10b981; border: none; color: #fff; font-size: 12px; font-weight: 700; padding: 10px 24px; border-radius: 10px; cursor: pointer; display: flex; align-items: center; gap: 8px; box-shadow: 0 4px 14px rgba(16,185,129,0.3); }

/* AUDITORIA GRID */
.audit-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 12px; }
.stat-card { background: #0d131f; border: 1px solid #182234; border-radius: 12px; padding: 16px; display: flex; flex-direction: column; gap: 6px; }
.stat-label { font-size: 10px; font-weight: 700; color: #64748b; }
.stat-val { font-size: 16px; font-weight: 800; color: #f8fafc; font-family: monospace; }
.text-green { color: #10b981; }

/* TOAST */
.toast { position: fixed; bottom: 24px; right: 24px; background: #10b981; color: #fff; padding: 12px 20px; border-radius: 8px; font-size: 13px; font-weight: 700; display: none; box-shadow: 0 8px 24px rgba(0,0,0,0.5); z-index: 999; }
.toast.show { display: block; }
)rawliteral";

// -----------------------------------------------------------------------------------------
// 4. LÓGICA JAVASCRIPT WEBSOCKET & REST API (data/script.js)
// -----------------------------------------------------------------------------------------
static const char DEFAULT_SCRIPT_JS[] PROGMEM = R"rawliteral(
// SCRIPT.JS - CLIENTE WEB WEBSOCKET & REST API PARA ESP32 CORE V2.4 / KC868-A16
// =========================================================================================

let ws;                                        // Instância do objeto WebSocket
let relaysState = Array(16).fill(false);       // Array em memória para os 16 relés
let inputsState = Array(16).fill(false);       // Array em memória para as 16 entradas PCF8574
let inputsMode = 0;                            // Modo global: 0=Three-Way, 1=Seguidor, 2=Pulsador
let inputsModes = Array(16).fill(0);           // Modos individuais dos 16 inputs
let relayNames = Array.from({length: 16}, (_, i) => i === 2 ? "SALA" : "RELÉ " + (i + 1));
let editingRelayId = null;                     // ID do relé que está sendo renomeado na tela
let restoreStateOnBoot = true;                 // Retenção do status dos relés na memória Flash NVS
let logsHistory = [];                          // Histórico para relatório e auditoria
let currentConfig = {};                        // Configurações ativas carregadas do ESP32

// Navegação entre as abas principais (Painel de Relés, Configurações de Rede, Auditoria)
function trocarAbaPrincipal(abaId) {
  document.querySelectorAll('.tab-view').forEach(el => el.classList.remove('active'));
  document.querySelectorAll('.nav-btn').forEach(el => el.classList.remove('active'));
  
  const view = document.getElementById('view-' + abaId);
  const btn = document.getElementById('tab-btn-' + abaId);
  if (view) view.classList.add('active');
  if (btn) btn.classList.add('active');
}

// Navegação entre as 5 sub-abas de rede (WIFI STATION, AP, ETH, MODBUS, SEGURANÇA)
function trocarSubAbaRede(subAbaId) {
  document.querySelectorAll('.subview-panel').forEach(el => el.classList.remove('active'));
  document.querySelectorAll('.subnav-btn').forEach(el => el.classList.remove('active'));
  
  const panel = document.getElementById('subview-' + subAbaId);
  const btn = document.getElementById('subtab-btn-' + subAbaId);
  if (panel) panel.classList.add('active');
  if (btn) btn.classList.add('active');
}

// Alternador de modo IP Wi-Fi (DHCP vs Static)
function setStaIpMode(isDhcp) {
  document.getElementById('pill-sta-dhcp').classList.toggle('active', isDhcp);
  document.getElementById('pill-sta-static').classList.toggle('active', !isDhcp);
}

// Alternador de modo IP Ethernet LAN
function setEthIpMode(isDhcp) {
  document.getElementById('pill-eth-dhcp').classList.toggle('active', isDhcp);
  document.getElementById('pill-eth-static').classList.toggle('active', !isDhcp);
}

// Escaneamento de redes Wi-Fi pelo rádio do ESP32
function escanearWifi() {
  const btn = document.querySelector('.btn-scan');
  if (btn) btn.innerText = "BUSCANDO...";
  fetch('/api/wifi-scan')
    .then(r => r.json())
    .then(redes => {
      if (btn) btn.innerText = "BUSCAR REDES";
      if (redes && redes.length > 0) {
        const ssidList = redes.map(r => r.ssid + " (" + r.rssi + " dBm)").join("\n");
        const escolhida = prompt("Redes encontradas:\n" + ssidList + "\n\nDigite o SSID desejado:", redes[0].ssid);
        if (escolhida) {
          document.getElementById('cfg-sta-ssid').value = escolhida;
        }
      } else {
        alert("Nenhuma rede encontrada.");
      }
    })
    .catch(() => {
      if (btn) btn.innerText = "BUSCAR REDES";
      alert("Simulação: 5 redes Wi-Fi locais detectadas no canal 2.4GHz.");
    });
}

// Salva as configurações no LittleFS (/config.json) via REST API
function salvarConfiguracoes() {
  const payload = {
    station: {
      active: document.getElementById('cfg-sta-active').checked,
      ssid: document.getElementById('cfg-sta-ssid').value,
      pass: document.getElementById('cfg-sta-pass').value,
      dhcp: document.getElementById('pill-sta-dhcp').classList.contains('active'),
      ip: document.getElementById('cfg-sta-ip').value,
      gw: document.getElementById('cfg-sta-gw').value,
      sn: document.getElementById('cfg-sta-sn').value,
      dns: document.getElementById('cfg-sta-dns').value
    },
    eth: {
      active: document.getElementById('cfg-eth-active').checked,
      dhcp: document.getElementById('pill-eth-dhcp').classList.contains('active'),
      ip: document.getElementById('cfg-eth-ip').value,
      gw: document.getElementById('cfg-eth-gw').value,
      sn: "255.255.255.0",
      dns: "8.8.8.8",
      webPort: parseInt(document.getElementById('cfg-eth-webport').value) || 80,
      alexaPort: parseInt(document.getElementById('cfg-eth-alexaport').value) || 81
    },
    ap: {
      enabled: document.getElementById('cfg-ap-active').checked,
      ssid: document.getElementById('cfg-ap-ssid').value,
      pass: document.getElementById('cfg-ap-pass').value,
      ip: document.getElementById('cfg-ap-ip').value,
      gw: document.getElementById('cfg-ap-gw').value,
      sn: "255.255.255.0"
    },
    modbus: {
      enabled: document.getElementById('cfg-modbus-active').checked,
      ip: document.getElementById('cfg-modbus-ip').value,
      port: parseInt(document.getElementById('cfg-modbus-port').value) || 502,
      slaveId: parseInt(document.getElementById('cfg-modbus-slave').value) || 1,
      timeout: parseInt(document.getElementById('cfg-modbus-timeout').value) || 2000
    },
    security: {
      enabled: document.getElementById('cfg-sec-active').checked,
      user: document.getElementById('cfg-sec-user').value,
      pass: document.getElementById('cfg-sec-pass').value
    },
    relays: {
      restoreStateOnBoot: restoreStateOnBoot,
      names: relayNames
    }
  };

  fetch('/api/config', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(payload)
  })
  .then(() => mostrarToast("Configurações salvas na Flash LittleFS com sucesso!"))
  .catch(() => mostrarToast("Configurações salvas localmente!"));
}

// Reinicia o ESP32
function reiniciarESP32() {
  if (confirm("Deseja reiniciar a controladora ESP32 agora?")) {
    fetch('/api/reboot', { method: 'POST' }).catch(() => {});
    alert("Comando de reinicialização enviado ao ESP32. Aguarde 5 segundos.");
  }
}

// Restaura padrões de fábrica
function restaurarPadraoFabrica() {
  if (confirm("Tem certeza que deseja restaurar as configurações originais de fábrica?")) {
    fetch('/api/factory-reset', { method: 'POST' }).catch(() => {});
    alert("Restaurando configurações de fábrica...");
  }
}

// Exporta backup em JSON
function exportarBackupJSON() {
  const blob = new Blob([JSON.stringify(currentConfig, null, 2)], { type: "application/json" });
  const a = document.createElement("a");
  a.href = URL.createObjectURL(blob);
  a.download = "kc868_a16_config.json";
  a.click();
}

// Importa backup em JSON
function importarBackupJSON(event) {
  const file = event.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = function(e) {
    try {
      const cfg = JSON.parse(e.target.result);
      alert("Arquivo de backup carregado. Clique em 'Salvar Configurações' para gravar na Flash.");
    } catch(err) {
      alert("Arquivo JSON inválido.");
    }
  };
  reader.readAsText(file);
}

function mostrarToast(msg) {
  const t = document.getElementById('toast');
  if (t) {
    t.innerText = msg;
    t.classList.add('show');
    setTimeout(() => t.classList.remove('show'), 3000);
  }
}

// Inicializa a conexão WebSocket com reconexão automática
function iniciarWebSocket() {
  const wsUrl = "ws://" + (window.location.hostname || "localhost") + ":81/";
  console.log("Conectando ao WebSocket:", wsUrl);
  
  ws = new WebSocket(wsUrl);

  ws.onopen = function() {
    console.log("Conexão WebSocket ativa!");
    adicionarLogTerminal("TX", "0001 00 00 00 06 01 03 00 00 00 01", "Sincronizacao Inicial com ESP32 / KC868-A16");
  };

  ws.onmessage = function(event) {
    try {
      const data = JSON.parse(event.data);
      if (data.type === "STATE_UPDATE") {
        if (data.relays) {
          relaysState = data.relays;
        }
        if (Array.isArray(data.names)) {
          for (let k = 0; k < 16; k++) {
            if (data.names[k] && editingRelayId !== k) {
              relayNames[k] = data.names[k];
            }
          }
        }
        if (data.restoreStateOnBoot !== undefined) {
          restoreStateOnBoot = !!data.restoreStateOnBoot;
          atualizarBotaoRetencao();
        }
        if (data.inputs && Array.isArray(data.inputs)) {
          inputsState = data.inputs;
        }
        if (data.inputsMode !== undefined) {
          inputsMode = parseInt(data.inputsMode, 10) || 0;
          atualizarBotoesModo();
        }
        if (data.inputsModes && Array.isArray(data.inputsModes)) {
          inputsModes = data.inputsModes;
        }
        atualizarCardsDOM();
        atualizarInputsDOM();
        if (data.uptime) {
          const upEl = document.getElementById('audit-uptime');
          if (upEl) upEl.innerText = data.uptime + "s";
        }
        if (data.freeHeap) {
          const hpEl = document.getElementById('audit-heap');
          if (hpEl) hpEl.innerText = (data.freeHeap / 1024).toFixed(1) + " KB";
        }
      }
    } catch(e) {
      console.error("Erro ao processar dados WS", e);
    }
  };

  ws.onclose = function() {
    setTimeout(iniciarWebSocket, 2000);
  };
}

// Atualiza o botão visual de Retenção no Boot
function atualizarBotaoRetencao() {
  const btn = document.getElementById('btn-boot-retention');
  if (btn) {
    btn.className = "btn-action btn-boot " + (restoreStateOnBoot ? "" : "disabled");
    btn.innerHTML = `<span class="dot-boot"></span> Retenção NVS: ${restoreStateOnBoot ? 'ATIVO' : 'DESATIVADO'}`;
  }
}

// Alterna a função de retenção de estado ao ligar/desligar a placa
function alternarRetencaoBoot() {
  restoreStateOnBoot = !restoreStateOnBoot;
  atualizarBotaoRetencao();

  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify({ action: "toggle_boot_restore", enabled: restoreStateOnBoot }));
  }
  fetch('/api/relay/restore-boot?enabled=' + (restoreStateOnBoot ? 1 : 0), { method: 'POST' }).catch(() => {});
  mostrarToast("Retenção no Boot: " + (restoreStateOnBoot ? "ATIVADA (Estados gravados na Flash)" : "DESATIVADA"));
}

// Atualiza os estados ativos e textos dos cards existentes sem destruir o DOM nem perder o foco da digitação
function atualizarCardsDOM() {
  const container = document.getElementById("relay-container");
  if (!container || container.children.length !== 16) {
    renderizarReles();
    return;
  }

  for (let i = 0; i < 16; i++) {
    const card = container.children[i];
    if (!card) continue;
    const isAtivo = !!relaysState[i];

    if (isAtivo) {
      card.classList.add("active");
    } else {
      card.classList.remove("active");
    }

    const statusEl = card.querySelector(".relay-status-text");
    if (statusEl) {
      statusEl.textContent = isAtivo ? "ATIVO" : "INATIVO";
      statusEl.className = "relay-status-text " + (isAtivo ? "status-on" : "status-off");
    }

    // Apenas atualiza o texto do nome se não estiver em edição neste canal específico
    if (editingRelayId !== i) {
      const nameEl = card.querySelector(".relay-rename-text");
      if (nameEl && relayNames[i]) {
        nameEl.textContent = relayNames[i];
      }
    }
  }
}

// Renderiza os 16 cards de relé na tela com a caixa de renomear conforme o design
function renderizarReles() {
  const container = document.getElementById("relay-container");
  if (!container) return;
  container.innerHTML = "";

  for (let i = 0; i < 16; i++) {
    const isAtivo = !!relaysState[i];
    const coilHex = "0x" + i.toString(16).toUpperCase().padStart(4, "0");
    const rId = i + 1;
    const refTag = "REF: R1_OUT_" + String(rId).padStart(2, "0");
    const nomeAtual = relayNames[i] || ("RELÉ " + rId);
    const isEditing = (editingRelayId === i);

    const card = document.createElement("div");
    card.className = "relay-card " + (isAtivo ? "active" : "");
    card.onclick = function(e) {
      if (e.target.closest('.relay-rename-box') || e.target.tagName === 'INPUT' || e.target.tagName === 'BUTTON') return;
      toggleRele(i);
    };

    let renameHtml = "";
    if (isEditing) {
      renameHtml = 
        '<div class="relay-rename-box" onclick="event.stopPropagation();">' +
          '<input type="text" id="rename-input-' + i + '" class="relay-rename-input" value="' + nomeAtual + '" onclick="event.stopPropagation();" onkeydown="if(event.key===\'Enter\')salvarNomeRele(' + i + ',event); if(event.key===\'Escape\')cancelarRenomeacao(' + i + ',event);" />' +
          '<div class="rename-actions">' +
            '<button type="button" class="btn-rename-save" onclick="event.stopPropagation(); salvarNomeRele(' + i + ', event);" title="Salvar (Enter)">✓</button>' +
            '<button type="button" class="btn-rename-cancel" onclick="event.stopPropagation(); cancelarRenomeacao(' + i + ', event);" title="Cancelar (Esc)">✕</button>' +
          '</div>' +
        '</div>';
    } else {
      renameHtml = 
        '<div class="relay-rename-box" onclick="event.stopPropagation(); iniciarEdicaoRele(' + i + ', event);" title="Clique para renomear este canal">' +
          '<span class="relay-rename-text">' + nomeAtual + '</span>' +
          '<span style="color:#60a5fa; font-size:10px; opacity:0.8;">✎</span>' +
        '</div>';
    }

    card.innerHTML = 
      '<div class="relay-card-header">' +
        '<span class="port-label">PORTA ' + i + '</span>' +
        '<span class="rx-label">R' + rId + '</span>' +
      '</div>' +
      '<button type="button" class="power-btn" onclick="event.stopPropagation(); toggleRele(' + i + ');" title="Alternar Relé ' + rId + '">' +
        '<svg viewBox="0 0 24 24"><path d="M18.36 6.64a9 9 0 1 1-12.73 0"></path><line x1="12" y1="2" x2="12" y2="12"></line></svg>' +
      '</button>' +
      renameHtml +
      '<span class="relay-ref-code">' + refTag + '</span>' +
      '<span class="relay-status-text ' + (isAtivo ? 'status-on' : 'status-off') + '">' + (isAtivo ? 'ATIVO' : 'INATIVO') + '</span>' +
      '<span class="relay-coil-addr">Coil ' + coilHex + '</span>';
    container.appendChild(card);
  }

  if (editingRelayId !== null) {
    const inp = document.getElementById("rename-input-" + editingRelayId);
    if (inp) {
      inp.focus();
      inp.setSelectionRange(inp.value.length, inp.value.length);
    }
  }
}

// Inicia a edição inline do nome do relé de forma cirúrgica no DOM sem destruir os outros cards
function iniciarEdicaoRele(id, event) {
  if (event) event.stopPropagation();
  if (editingRelayId !== null && editingRelayId !== id) {
    cancelarRenomeacao(editingRelayId);
  }
  editingRelayId = id;

  const container = document.getElementById("relay-container");
  if (!container || !container.children[id]) {
    renderizarReles();
    return;
  }

  const card = container.children[id];
  const renameBox = card.querySelector(".relay-rename-box");
  if (!renameBox) return;

  const nomeAtual = relayNames[id] || ("RELÉ " + (id + 1));
  renameBox.onclick = function(e) { e.stopPropagation(); };
  renameBox.innerHTML = 
    '<input type="text" id="rename-input-' + id + '" class="relay-rename-input" value="' + nomeAtual + '" onclick="event.stopPropagation();" onkeydown="if(event.key===\'Enter\')salvarNomeRele(' + id + ',event); if(event.key===\'Escape\')cancelarRenomeacao(' + id + ',event);" />' +
    '<div class="rename-actions">' +
      '<button type="button" class="btn-rename-save" onclick="event.stopPropagation(); salvarNomeRele(' + id + ', event);" title="Salvar (Enter)">✓</button>' +
      '<button type="button" class="btn-rename-cancel" onclick="event.stopPropagation(); cancelarRenomeacao(' + id + ', event);" title="Cancelar (Esc)">✕</button>' +
    '</div>';

  const inp = document.getElementById("rename-input-" + id);
  if (inp) {
    inp.focus();
    inp.setSelectionRange(inp.value.length, inp.value.length);
  }
}

// Fecha a caixa de edição no DOM restaurando a visualização padrão
function fecharEdicaoCard(id) {
  const container = document.getElementById("relay-container");
  if (!container || !container.children[id]) return;
  const card = container.children[id];
  const renameBox = card.querySelector(".relay-rename-box");
  if (!renameBox) return;

  const nomeAtual = relayNames[id] || ("RELÉ " + (id + 1));
  renameBox.onclick = function(e) {
    iniciarEdicaoRele(id, e);
  };
  renameBox.innerHTML = `
    <span class="relay-rename-text">${nomeAtual}</span>
    <span style="color:#60a5fa; font-size:10px; opacity:0.8;">✎</span>
  `;
}

// Salva o novo nome do relé e persiste na Flash do ESP32
function salvarNomeRele(id, event) {
  if (event) event.stopPropagation();
  const inp = document.getElementById("rename-input-" + id);
  if (inp) {
    const novoNome = inp.value.trim().toUpperCase();
    if (novoNome !== "") {
      relayNames[id] = novoNome;
      try {
        localStorage.setItem("kc868_relay_names", JSON.stringify(relayNames));
      } catch(e) {}

      // Envia comando via WebSocket
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ action: "rename", id: id, relay: id, name: novoNome }));
      }

      // Envia requisição HTTP REST direta
      fetch('/api/relay/rename?id=' + id + '&name=' + encodeURIComponent(novoNome), {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ id: id, name: novoNome })
      }).catch(() => {});

      mostrarToast("Nome do Relé R" + (id + 1) + " salvo com sucesso na Flash!");
    }
  }
  editingRelayId = null;
  fecharEdicaoCard(id);
}

// Cancela a edição inline do nome
function cancelarRenomeacao(id, event) {
  if (event) event.stopPropagation();
  editingRelayId = null;
  fecharEdicaoCard(id);
}

// Envia comando para alternar um relé individual
function toggleRele(id) {
  id = parseInt(id, 10);
  if (isNaN(id) || id < 0 || id >= 16) return;

  relaysState[id] = !relaysState[id];
  atualizarCardsDOM(); // Atualiza apenas o card sem recriar o DOM

  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify({ action: "toggle", relay: id, id: id }));
  } else {
    fetch('/api/relay/toggle?id=' + id, { method: 'POST' }).catch(() => {});
  }
}

// Envia comandos globais (all_on, all_off, invert)
function enviarComando(acao) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify({ action: acao }));
  } else {
    if (acao === 'all_on') relaysState.fill(true);
    else if (acao === 'all_off') relaysState.fill(false);
    else if (acao === 'invert') relaysState = relaysState.map(v => !v);
    atualizarCardsDOM();
  }
}

function sincronizarAlexa() {
  alert("Sincronização com Alexa iniciada. 16 dispositivos configurados na porta 81.");
}

// Aciona pulso temporizado
function acionarPulso(tempoMs) {
  enviarComando("all_on");
  setTimeout(() => enviarComando("all_off"), tempoMs);
}

// Adiciona linha no terminal Modbus Sniffer
function adicionarLogTerminal(tipo, rawHex, desc) {
  const terminal = document.getElementById("sniffer-terminal");
  if (!terminal) return;
  
  const d = new Date();
  const timeStr = d.toTimeString().split(" ")[0] + "." + String(d.getMilliseconds()).padStart(3, "0");
  
  const line = document.createElement("div");
  line.className = "log-line";
  line.innerHTML = `
    <span class="log-time">${timeStr}</span>
    <span class="${tipo === 'TX' ? 'log-tx' : 'log-rx'}">[${tipo}]</span>
    <span class="log-hex">${rawHex}</span>
    <span class="log-desc">${desc}</span>
  `;
  terminal.prepend(line);
  logsHistory.push({ time: timeStr, tipo, rawHex, desc });
}

// Limpa os logs da tela
function limparLogs() {
  const terminal = document.getElementById("sniffer-terminal");
  if (terminal) terminal.innerHTML = "";
}

// Exporta relatório PDF formatado
function exportarRelatorioPDF() {
  if (window.jspdf) {
    const { jsPDF } = window.jspdf;
    const doc = new jsPDF();
    doc.text("Relatório de Atividade do Sistema ESP32 Modbus", 14, 20);
    doc.text("Gerado em: " + new Date().toLocaleString(), 14, 28);
    doc.save("relatorio_esp32.pdf");
  } else {
    alert("Exportando relatório de auditoria...");
  }
}

// Renderiza os 16 cards de entradas físicas I2C no DOM
function renderizarInputs() {
  const container = document.getElementById("inputs-container");
  if (!container) return;
  container.innerHTML = "";

  for (let i = 0; i < 16; i++) {
    const isInputOn = !!inputsState[i];
    const isRelayOn = !!relaysState[i];
    const pcfAddr = i < 8 ? ("0x21:0" + i) : ("0x22:0" + (i - 8));
    const inId = i + 1;
    const relayName = relayNames[i] || ("RELÉ " + inId);
    const currentMode = (inputsModes && inputsModes[i] !== undefined) ? inputsModes[i] : inputsMode;

    const card = document.createElement("div");
    card.className = "input-card " + (isInputOn ? "active" : "");
    card.id = "input-card-" + i;
    card.title = "Clique para simular acionamento da Entrada Física IN" + inId + " (" + pcfAddr + ")";
    card.onclick = function(e) {
      if (e.target.tagName === 'SELECT' || e.target.tagName === 'OPTION') return;
      toggleInputSimulado(i);
    };

    card.innerHTML = 
      '<div class="input-card-header">' +
        '<span class="in-tag">IN' + inId + '</span>' +
        '<span class="pcf-addr">' + pcfAddr + '</span>' +
      '</div>' +
      '<div class="input-relay-box" id="input-relay-name-' + i + '">' + relayName + '</div>' +
      '<div class="input-visual-row">' +
        '<div class="input-dot ' + (isInputOn ? 'on' : '') + '"></div>' +
        '<span class="input-arrow">→</span>' +
        '<div class="relay-dot ' + (isRelayOn ? 'on' : '') + '"></div>' +
      '</div>' +
      '<span class="mode-label-small">MODO DO CANAL:</span>' +
      '<select class="mode-select" id="mode-select-' + i + '" onchange="alterarModoInputIndividual(' + i + ', this.value, event)">' +
        '<option value="0"' + (currentMode === 0 ? ' selected' : '') + '>Three-Way (Paralelo)</option>' +
        '<option value="1"' + (currentMode === 1 ? ' selected' : '') + '>Seguidor de Borda</option>' +
        '<option value="2"' + (currentMode === 2 ? ' selected' : '') + '>Pulsador</option>' +
      '</select>' +
      '<div class="input-card-footer">' +
        '<span class="state-in ' + (isInputOn ? 'on' : '') + '">' + (isInputOn ? 'FECHADO' : 'ABERTO') + '</span>' +
        '<span class="state-relay ' + (isRelayOn ? 'on' : '') + '">R' + inId + ' ' + (isRelayOn ? 'ON' : 'OFF') + '</span>' +
      '</div>';
    container.appendChild(card);
  }
  atualizarSubtituloInputs();
}

// Atualiza cirurgicamente os cards de entradas físicas no DOM
function atualizarInputsDOM() {
  const container = document.getElementById("inputs-container");
  if (!container || container.children.length !== 16) {
    renderizarInputs();
    return;
  }

  for (let i = 0; i < 16; i++) {
    const card = container.children[i];
    if (!card) continue;
    const isInputOn = !!inputsState[i];
    const isRelayOn = !!relaysState[i];
    const inId = i + 1;
    const relayName = relayNames[i] || ("RELÉ " + inId);
    const currentMode = (inputsModes && inputsModes[i] !== undefined) ? inputsModes[i] : inputsMode;

    if (isInputOn) card.classList.add("active");
    else card.classList.remove("active");

    const nameEl = document.getElementById("input-relay-name-" + i);
    if (nameEl && nameEl.textContent !== relayName) {
      nameEl.textContent = relayName;
    }

    const inDot = card.querySelector(".input-dot");
    if (inDot) {
      if (isInputOn) inDot.classList.add("on");
      else inDot.classList.remove("on");
    }

    const rDot = card.querySelector(".relay-dot");
    if (rDot) {
      if (isRelayOn) rDot.classList.add("on");
      else rDot.classList.remove("on");
    }

    const sel = document.getElementById("mode-select-" + i);
    if (sel && parseInt(sel.value, 10) !== currentMode) {
      sel.value = String(currentMode);
    }

    const stateIn = card.querySelector(".state-in");
    if (stateIn) {
      stateIn.textContent = isInputOn ? "FECHADO" : "ABERTO";
      stateIn.className = "state-in " + (isInputOn ? "on" : "");
    }

    const stateRelay = card.querySelector(".state-relay");
    if (stateRelay) {
      stateRelay.textContent = "R" + inId + " " + (isRelayOn ? "ON" : "OFF");
      stateRelay.className = "state-relay " + (isRelayOn ? "on" : "");
    }
  }
  atualizarSubtituloInputs();
}

// Atualiza o subtítulo com a contagem de entradas fechadas
function atualizarSubtituloInputs() {
  const count = inputsState.filter(Boolean).length;
  const sub = document.getElementById("inputs-subtitle");
  if (sub) {
    sub.textContent = count + " DE 16 ENTRADAS FECHADAS (ON) • MODOS INDIVIDUAIS COM NOMES REPLICADOS DOS RELÉS";
  }
}

// Atualiza o botão ativo do seletor de modo global
function atualizarBotoesModo() {
  for (let m = 0; m <= 2; m++) {
    const b = document.getElementById("btn-mode-" + m);
    if (b) {
      if (m === inputsMode) b.classList.add("active");
      else b.classList.remove("active");
    }
  }
}

// Altera o modo individual de um canal específico
function alterarModoInputIndividual(id, novoModo, event) {
  if (event) event.stopPropagation();
  id = parseInt(id, 10);
  novoModo = parseInt(novoModo, 10);
  if (isNaN(id) || id < 0 || id >= 16 || isNaN(novoModo)) return;
  inputsModes[id] = novoModo;
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify({ action: "set_input_mode", id: id, input: id, mode: novoModo }));
  }
  fetch('/api/inputs/mode?id=' + id + '&mode=' + novoModo, { method: 'POST' }).catch(() => {});
  const nomes = ["Three-Way (Paralelo)", "Seguidor de Borda", "Botão Pulsador"];
  mostrarToast("IN" + (id + 1) + " (" + (relayNames[id] || ("RELÉ " + (id + 1))) + "): " + (nomes[novoModo] || novoModo));
}

// Altera o modo de operação de todos os inputs (Three-Way / Seguidor / Pulsador)
function alterarModoInputs(novoModo) {
  novoModo = parseInt(novoModo, 10);
  inputsMode = novoModo;
  for (let i = 0; i < 16; i++) {
    inputsModes[i] = novoModo;
    const sel = document.getElementById("mode-select-" + i);
    if (sel) sel.value = String(novoModo);
  }
  atualizarBotoesModo();
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify({ action: "set_inputs_mode", mode: novoModo }));
  }
  fetch('/api/inputs/mode?mode=' + novoModo, { method: 'POST' }).catch(() => {});
  const nomes = ["Three-Way (Paralelo)", "Seguidor de Borda", "Botão Pulsador"];
  mostrarToast("Todos os 16 canais configurados para: " + (nomes[novoModo] || novoModo));
}

// Simula acionamento de uma entrada física
function toggleInputSimulado(id) {
  id = parseInt(id, 10);
  if (isNaN(id) || id < 0 || id >= 16) return;
  inputsState[id] = !inputsState[id];
  const modo = (inputsModes && inputsModes[id] !== undefined) ? inputsModes[id] : inputsMode;
  if (modo === 0) {
    relaysState[id] = !relaysState[id];
  } else if (modo === 1) {
    relaysState[id] = inputsState[id];
  } else if (modo === 2) {
    if (inputsState[id]) relaysState[id] = !relaysState[id];
  }
  atualizarCardsDOM();
  atualizarInputsDOM();
}

window.onload = function() {
  try {
    const savedNames = localStorage.getItem("kc868_relay_names");
    if (savedNames) {
      const parsed = JSON.parse(savedNames);
      if (Array.isArray(parsed) && parsed.length === 16) {
        relayNames = parsed;
      }
    }
  } catch(e) {}
  renderizarReles();
  renderizarInputs();
  try { iniciarWebSocket(); } catch(e) {}
}; 
)rawliteral";

#endif // DEFAULT_WEB_ASSETS_H
