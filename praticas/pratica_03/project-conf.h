/* Definições gerais para a prática 03 */

// Qual canal vamos usar?
#define RF_CORE_CONF_CHANNEL (0)

// Como está estruturada a informação a ser transmitida?
typedef union {
  int8_t i8[4];
  int32_t i32;
} int_payload_t;

// Qual o endereço do servidor UDP?
#define UDP_CONNECTION_ADDR       "contiki-udp-server.local"   // se usar mDNS

// A qual porta vamos nos conectar?
#define CONN_PORT 8802
