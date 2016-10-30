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

enum command {
    CMD_SET_ADDR,
    CMD_SET_ADDR_CSUM,
    CMD_SEND,
    CMD_SEND_CSUM,
};

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
    os_printf("Socket setup finished\n");
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
    os_printf("WiFi setup finished\n");
}

void message_recvd (void *arg, char *data, unsigned short len)
{
    struct espconn *conn = (struct espconn *) arg;

    os_printf("Message = [ ");
    unsigned short i;
    for(i = 0; i < len; ++i)
        os_printf("%02X ", data[i]);
    os_printf("]\n");

    while(len)
    {
        switch(data[0]) {

        case CMD_SET_ADDR:
            if(len < 2)
            {
                os_printf("Invalid SET_ADDR command\n");
                return;
            }
            else
            {
                ir_set_address(data[1]);
                len -= 2;
                data += 2;
            }
            break;

        case CMD_SET_ADDR_CSUM:
            if(len < 3)
            {
                os_printf("Invalid SET_ADDR_CSUM command\n");
                return;
            }
            else
            {
                ir_set_address_csum(data[1], data[2]);
                len -= 3;
                data += 3;
            }
            break;

        case CMD_SEND:
            if(len < 2)
            {
                os_printf("Invalid SEND command\n");
                return;
            }
            else
            {
                ir_send(data[1]);
                len -= 2;
                data += 2;
            }
            break;

        case CMD_SEND_CSUM:
            if(len < 3)
            {
                os_printf("Invalid SEND_CSUM command\n");
                return;
            }
            else
            {
                ir_send_csum(data[1], data[2]);
                len -= 3;
                data += 3;
            }
            break;

        default:
            os_printf("Invalid command %02X\n", data[0]);
        }
    }
}

void hearbeat (void *args)
{
    led_toggle();
}

void user_init (void)
{
    gpio_init();

    uart_div_modify(0, UART_CLK_FREQ / 115200);
    os_printf("Initializing...\n");
    led_init();
    ir_init();

    setup_socket();
    setup_wifi();

    os_timer_setfn(&heartbeat_timer, (os_timer_func_t*)hearbeat, NULL);
    os_timer_arm(&heartbeat_timer, 1000, 1);
}
