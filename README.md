# Bat-Button — Node A

Firmware do botão (ESP32-S3, ESP-IDF 5.2.2). Wi-Fi, broker e GPIO já estão no código, em `components/gotham_protocol/include/gotham_protocol.h`. Não precisa de menuconfig.

Botão: GPIO 4 e GND (pull-up interno).

```text
idf.py build
idf.py -p COM flash monitor
```

Publica `BAT_SIGNAL_ON` / `BAT_SIGNAL_OFF` em `gotham/dpgc/berg_caio/batsignal`. Heartbeat 30 s em `gotham/dpgc/berg_caio/status` com `"device": "bat_button"`.
