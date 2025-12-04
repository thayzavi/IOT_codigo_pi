Sistema IoT de Monitoramento e Alerta de Enchentes

Este projeto utiliza ESP32 + HC-SR04 para medir o nível da água e enviar
dados para um backend.

FUNCIONALIDADES: - Medição contínua do nível da água. - Classificação
dos níveis: Normal, Médio e Alerta Máximo. - LED piscante com diferentes
padrões. - Conexão Wi-Fi com reconexão automática. - Envio periódico de
dados para o backend.

```wifi
const char* ssid = "suaRede";
const char* password = "suaSenha";
```


ESQUEMA DE CONEXÃO: - TRIG → GPIO 4 - ECHO → GPIO 3 - LED → GPIO 10

NÍVEIS: - > 20 cm → NORMAL - 10–20 cm → MÉDIO - < 10 cm → ALERTA MÁXIMO

ENVIO AO BACKEND: POST /sensor JSON enviado: { “distancia”: 12.4 }

COMO USAR:
1. Configure o Wi-Fi no código.
2. Faça o upload no ESP32.
3.Abra o Serial Monitor (115200).
4. Observe as leituras e alertas.
