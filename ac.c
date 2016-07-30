#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h" // wifi_*
#include "espconn.h" // sockets
#include "mem.h" // os_zalloc
#include "osapi.h"
#include "ir.h"
#include "led.h"
#include "wifi_settings.h"

static volatile os_timer_t heartbeat_timer;

void message_recvd (void *arg, char *data, unsigned short len);
void hearbeat (void *arg);

void setup_socket (void)
{
    // Prepare listening socket
    struct espconn *conn = (struct espconn *)os_zalloc(sizeof(struct espconn));
    espconn_create(conn);
    conn->type = ESPCONN_TCP;
    conn->state = ESPCONN_NONE;
    conn->proto.tcp = (esp_tcp*)os_zalloc(sizeof(esp_tcp));
    conn->proto.tcp->local_port = 4000;
    espconn_regist_recvcb(conn, message_recvd);
    espconn_accept(conn);
    espconn_regist_time(conn, 15, 0); // timeout (seconds)
}

void setup_wifi (void)
{
    // Connect to WiFi
    struct station_config station_conf = {
        .ssid     = WIFI_SSID,
        .password = WIFI_PASSWORD
    };
    wifi_set_opmode(STATION_MODE);
    wifi_station_set_config(&station_conf);
    wifi_station_connect();
}

void message_recvd (void *arg, char *data, unsigned short len)
{
    struct espconn *conn = (struct espconn *) arg;

    if(!len)
        return;

    uint8_t cmd = data[0];
    os_printf("Got command 0x%02X\n", cmd);
    ir_send(cmd);
}

void hearbeat (void *args)
{
    led_toggle();
}

void user_init (void)
{
    gpio_init();

    uart_div_modify(0, UART_CLK_FREQ / 115200);
    led_init();
    ir_init();

    setup_socket();
    setup_wifi();

    os_timer_setfn(&heartbeat_timer, (os_timer_func_t*)hearbeat, NULL);
    os_timer_arm(&heartbeat_timer, 1000, 1);
}
