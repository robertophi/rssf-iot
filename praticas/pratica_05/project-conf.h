#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

// Qual a faixa ISM que vamos usar?
#define DOT_15_4G_CONF_FREQUENCY_BAND_ID DOT_15_4G_FREQUENCY_BAND_915

// Qual canal vamos usar?
#define RF_CORE_CONF_CHANNEL (25)

// Configurando o nó para usar um router 6loWPAN
#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER (1)
#define CC26XX_WEB_DEMO_6LBR_CLIENT (1)

// Qual o endereço e porta do servidor MQTT-SN?
#define UDP_CONNECTION_ADDR "6lbr.local"   //name

#define UDP_CONNECTION_PORT 1883

// Configurações da nossa aplicação exemplo
#define REQUEST_RETRIES 4
#define DEFAULT_SEND_INTERVAL (10 * CLOCK_SECOND)
#define REPLY_TIMEOUT (3 * CLOCK_SECOND)



#endif /* PROJECT_CONF_H_ */
