/* Definições gerais para a prática 04 */

// Qual canal vamos usar?
#define RF_CORE_CONF_CHANNEL (11)

// Como está estruturada a informação a ser transmitida?

// Payload para aplicação de temperatura
typedef union {
  int8_t i8[4];
  int32_t i32;
} temp_payload_t;

// Payload para aplicação de LEDs
typedef struct {
	int8_t cmd;
	int8_t led;
} comando_t;

typedef union {
  comando_t comando;
  int8_t i8[sizeof(comando_t)];
} leds_payload_t;

// Qual o endereço do servidor UDP?
#define UDP_CONNECTION_ADDR       "contiki-udp-server.local"   // se usar mDNS

// A qual porta vamos nos conectar?
#define CONN_PORT_TEMP 8801
#define CONN_PORT_LEDS 8802
