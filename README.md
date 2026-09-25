# 🦇 Projeto Bat-Sinal IoT (MQTT com ESP32-S3)

## 📖 O que é o projeto?
Este projeto implementa um sistema de comunicação IoT bidirecional entre dois microcontroladores utilizando o protocolo MQTT. Ele funciona como um "Bat-Sinal" remoto:
* **Node A (Emissor):** Lê o acionamento de um botão físico e publica uma mensagem de alerta em um servidor (Broker MQTT).
* **Node B (Receptor):** Fica inscrito no tópico do servidor, escutando as mensagens em tempo real. Ao receber o alerta do Node A, ele aciona um componente de saída (como um LED ou refletor) na sua própria bancada.

## 🛠️ O que usamos (Ferramentas e Hardware)
* **Microcontroladores:** 2x Placas ESP32-S3.
* **Linguagem:** C.
* **Framework:** ESP-IDF (v5.2.2).
* **Comunicação:** Wi-Fi (Modo Station) e protocolo TCP/IP via MQTT.
* **Componentes Físicos:** 
  * 1 Botão Tátil (Push-button) e jumpers (Node A).
  * 1 LED ou equivalente e um resistor apropriado (Node B).
* **Servidor (Backend):** Eclipse Mosquitto (Broker MQTT) rodando em uma máquina local.

---

## 🚀 Como rodar o projeto

### Pré-requisitos Comuns
Ambas as placas precisam estar conectadas à **mesma rede Wi-Fi** onde o Broker MQTT está hospedado (ou a uma rede que tenha rota para o IP do servidor).

### 1️⃣ Rodando o Node A (Emissor / Botão)
1. Clone o repositório e abra a pasta do projeto no terminal do ESP-IDF.
2. No código-fonte principal, configure as credenciais de rede e o endereço do servidor:
   ```c
   #define EXAMPLE_ESP_WIFI_SSID      "NOME_DA_REDE"
   #define EXAMPLE_ESP_WIFI_PASS      "SENHA_DA_REDE"
   #define MQTT_BROKER_URI            "mqtt://IP_DO_SERVIDOR:1883"
   ```
3. **Hardware:** Conecte uma perna do botão no pino **GPIO 4** e a outra perna no **GND** da placa (o pull-up é ativado via código).
4. Conecte o ESP32-S3 no USB, verifique a porta no sistema (ex: COM7, COM8) e execute a compilação e gravação:
   ```bash
   idf.py build
   idf.py -p COM_PORTA flash monitor
   ```
   *(Substitua `COM_PORTA` pela sua porta real. Ex: `-p COM7`)*

### 2️⃣ Rodando o Node B (Receptor / LED)
1. Abra a pasta do projeto correspondente ao receptor no terminal do ESP-IDF.
2. Certifique-se de configurar as **mesmas credenciais de Wi-Fi** e o **mesmo IP do Broker MQTT** usados no Node A.
3. O código do Node B deve estar configurado para se inscrever (subscribe) no exato mesmo tópico MQTT que o Node A está publicando.
4. **Hardware:** Conecte a perna positiva do LED na GPIO definida no código e a perna negativa no GND (use um resistor para proteger o LED).
5. Conecte a placa no USB, verifique a porta correspondente a ela e grave o código:
   ```bash
   idf.py build
   idf.py -p COM_PORTA flash monitor
   ```

### ✅ Teste Final
Com os dois monitores seriais abertos e indicando `Conectado ao Broker MQTT com sucesso!`:
* Aperte o botão no **Node A**.
* O monitor serial do Node A mostrará que a mensagem foi publicada.
* O LED no **Node B** acenderá instantaneamente, confirmando a comunicação ponta a ponta.
