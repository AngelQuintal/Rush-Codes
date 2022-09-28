#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
/* #include "protocol_examples_common.h" */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define ESP_WIFI_SSID      "Students"
#define ESP_WIFI_PASS      "P0l1t3cn1c4.b1s"
#define ESP_MAXIMUM_RETRY  10
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK

static EventGroupHandle_t s_wifi_event_group;


static const char *TAG = "MQTTWS_SENDER";
static int s_retry_num = 0;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static void
event_handler(void*            arg,
              esp_event_base_t event_base,
              int32_t          event_id,
              void*            event_data)
{
     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
     {
          esp_wifi_connect();
     }
     else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
     {
          if (s_retry_num < ESP_MAXIMUM_RETRY)
          {
               esp_wifi_connect();
               s_retry_num++;
               ESP_LOGI(TAG, "retry to connect to the AP");
          }
          else
          {
               xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
          }
          ESP_LOGI(TAG,"connect to the AP fail");
     }
     else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
     {
          ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
          ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
          s_retry_num = 0;
          xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
     }
}

void
wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

static void
log_error_if_nonzero(const char *message, int error_code)
{
     if (error_code != 0) {
          ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
     }
}

static void
mqtt_event_handler(void*            handler_args,
                   esp_event_base_t base,
                   int32_t          event_id,
                   void*            event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void
mqtt_app_start(void)
{
     const esp_mqtt_client_config_t mqtt_cfg = {
          .host = "ws://saposteamiot.tk",
          .uri  = "ws://saposteamiot.tk:8094/mqtt",
          .port = 8094,
          /* .credentials.username = "myEsp1", */
          /* .credentials.authentication.password = "123456" */
     };

     esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
     /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
     esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
     esp_mqtt_client_start(client);
}

void
app_main(void)
{
     ESP_LOGI(TAG, "[APP] Startup..");
     ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
     ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

     esp_log_level_set("*", ESP_LOG_INFO);
     esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
     esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
     esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
     esp_log_level_set("TRANSPORT_WS", ESP_LOG_VERBOSE);
     esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
     esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

     ESP_ERROR_CHECK(nvs_flash_init());
     ESP_ERROR_CHECK(esp_netif_init());
     /* ESP_ERROR_CHECK(esp_wifi_init()); */
     ESP_ERROR_CHECK(esp_event_loop_create_default());


     /* OUR CODE HERE */
     wifi_init_sta();
     mqtt_app_start();
}
