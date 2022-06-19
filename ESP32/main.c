// Library includes
#include "main.h"
#include <sys/param.h>

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"

#include "esp_http_client.h"
#include "driver/mcpwm.h"

#include "esp_spiffs.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "driver/gpio.h"							// vld
#include "driver/uart.h"
//#include "driver/ledc.h"

// Other includes
#include "test_pwm.h"

// WiFi Defines
#define EXAMPLE_AP_WIFI_SSID		"WRover"
#define EXAMPLE_AP_WIFI_PASS		""
#define EXAMPLE_MAX_STA_CONN		4
#define EXAMPLE_STA_MAXIMUM_RETRY	5

// Programe Defines
#define PORT 8080
#define CONFIG_EXAMPLE_IPV4
#define WIFI_3

#ifdef WIFI_1
	#define EXAMPLE_STA_WIFI_SSID		"Koko"
	#define EXAMPLE_STA_WIFI_PASS		"KokoLia13"
#elif defined(WIFI_2)
	#define EXAMPLE_STA_WIFI_SSID		"Bluebird"
	#define EXAMPLE_STA_WIFI_PASS		"vas1905vld"
#elif defined(WIFI_3)
	#define EXAMPLE_STA_WIFI_SSID		"Tech_D3881996"
	#define EXAMPLE_STA_WIFI_PASS		"FJHPEPJJ"
#endif

// Function defines
void do_retransmit();

static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG="Novice Vasko";

esp_http_client_handle_t client;
esp_err_t err;
 
static int s_retry_num = 0;
programData_t programs[21];
QueueHandle_t queue, socket_queue;
uint32_t ipaddress, ipnetmask, ipgw;

static esp_err_t init_spiffs(void){
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,   // This decides the maximum number of files that can be created on the storage
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }else{
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_STA_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		ipaddress = event->ip_info.ip.addr;
		ipnetmask = event->ip_info.netmask.addr;
		ipgw = event->ip_info.gw.addr;
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t wifi_init_apsta(void){
    s_wifi_event_group = xEventGroupCreate();

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = EXAMPLE_AP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_AP_WIFI_SSID),
            .password = EXAMPLE_AP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
			//.channel = AP_CHANNEL
        },
    };
    if (strlen(EXAMPLE_AP_WIFI_PASS) == 0) {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    wifi_config_t sta_config = {
        .sta = {
            .ssid = EXAMPLE_STA_WIFI_SSID,
            .password = EXAMPLE_STA_WIFI_PASS,
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_apsta finished. SSID:%s password:%s",
             EXAMPLE_AP_WIFI_SSID, EXAMPLE_AP_WIFI_PASS);

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);


    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_STA_WIFI_SSID, EXAMPLE_STA_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_STA_WIFI_SSID, EXAMPLE_STA_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);
 
	return ESP_OK;
}

esp_err_t start_file_server(const char *base_path);

void init_programs_data (programData_t *programs){
	programs->startHour = 0;
	programs->startMinute = 0;
	programs->separator1 = ',';
	programs->endHour = 0;
	programs->endMinute = 0;
	programs->separator2 = ',';
	*((uint8_t*)&programs->days_of_the_week) = 0x00;
	programs->separator3 = ',';
	programs->temperature = 0x2D;
	programs->separator4 = ',';
	programs->crc = programs->startHour + programs->startMinute + programs->endHour + \
					programs->endMinute + *((uint8_t*)&programs->days_of_the_week) + programs->temperature;
	programs->separator5 = '\n';
}

int make_file_data (char *buf, uint8_t mode){
	if (mode == 0) {
		const char *filepath = "/spiffs/program_list.hex";
		FILE *fd = NULL;
		struct stat file_stat;

		if (stat(filepath, &file_stat) == -1) {
			gpio_set_level(BLINK_GPIO, 1);					// DEL
			for (int i=0; i<21; i++) {
				init_programs_data(&programs[i]);
			}
			fd = fopen(filepath, "w");
			if (!fd) {
				gpio_set_level(BLINK_GPIO, 1);
				return 1;
			}
			fwrite(&programs[0], sizeof(programData_t), 21, fd);
			fclose(fd);
			gpio_set_level(BLINK_GPIO, 0);					// DEL
		}
	
		fd = fopen(filepath, "r");
		if (!fd) {
			gpio_set_level(BLINK_GPIO, 1);
			return 2;
		}
		if (fread(&programs[0], sizeof(programData_t), 21, fd) != 21) {
			gpio_set_level(BLINK_GPIO, 1);
			return 3;
		}
	
		for (int i=0; i<21; i++) {
			if (programs[i].crc != programs[i].startHour + programs[i].startMinute + programs[i].endHour + \
										programs[i].endMinute + *((uint8_t*)&programs[i].days_of_the_week) + programs[i].temperature) { 
				init_programs_data(&programs[i]);
			}
		}
		fclose(fd);
	}else if (mode == 1){
		char *r;
		uint8_t n;
		uint8_t m;

		r = strstr(buf, "program");
		if (r != NULL) {
			if(*(r+9) == '&') {
				n = (*(r+8) - 0x30);
			} else {
				n = ((*(r+8) - 0x30) * 10) + (*(r+9) - 0x30);
			}
			n--;
			r = strstr(buf, "start_time");
			if (r != NULL) {
				m = ((*(r+11) - 0x30) * 10) + (*(r+12) - 0x30);
				programs[n].startHour = m;
				m = ((*(r+16) - 0x30) * 10) + (*(r+17) - 0x30);
				programs[n].startMinute = m;
			}
			r = strstr(buf, "end_time");
			if (r != NULL) {
				m = ((*(r+9) - 0x30) * 10) + (*(r+10) - 0x30);
				programs[n].endHour = m;
				m = ((*(r+14) - 0x30) * 10) + (*(r+15) - 0x30);
				programs[n].endMinute = m;
			}

			r = strstr(buf, "mon");
			if (r != NULL) {
				*((uint8_t*)&programs[n].days_of_the_week) = 0;
				if (*(r+4) == 'y') programs[n].days_of_the_week.b_monday = 1;
				if (*(r+10) == 'y') programs[n].days_of_the_week.b_tuesday = 1;
				if (*(r+16) == 'y') programs[n].days_of_the_week.b_wednesday = 1;
				if (*(r+22) == 'y') programs[n].days_of_the_week.b_thursday = 1;
				if (*(r+28) == 'y') programs[n].days_of_the_week.b_friday = 1;
				if (*(r+34) == 'y') programs[n].days_of_the_week.b_saturday = 1;
				if (*(r+40) == 'y') programs[n].days_of_the_week.b_sunday = 1;
				if (*(r+46) == 'y') programs[n].days_of_the_week.b_hollidays = 1;
			}
			
			r = strstr(buf, "temperature");
			if (r != NULL) {
				m = ((*(r+12) - 0x30) * 10) + (*(r+13) - 0x30);
				programs[n].temperature = m;
			}
			programs[n].crc = programs[n].startHour + programs[n].startMinute + programs[n].endHour + \
											programs[n].endMinute + *((uint8_t*)&programs[n].days_of_the_week) + programs[n].temperature;
		}
	}
		
	return Data_OK;
}

void ip_file(void) {
	const char *filepath = "/spiffs/ip_data.hex";
    FILE *fd = NULL;

	fd = fopen(filepath, "w");
	fwrite(&ipaddress, sizeof(uint32_t), 1, fd);
	fwrite(&ipnetmask, sizeof(uint32_t), 1, fd);
	fwrite(&ipgw, sizeof(uint32_t), 1, fd);
	fclose(fd);
}

static void tcp_server_task(void *pvParameters){
    char addr_str[128];
    int addr_family;
    int ip_protocol;

#ifdef CONFIG_EXAMPLE_IPV4
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else // IPV6
    struct sockaddr_in6 dest_addr;
    bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
    dest_addr.sin6_family = AF_INET6;
    dest_addr.sin6_port = htons(PORT);
    addr_family = AF_INET6;
    ip_protocol = IPPROTO_IPV6;
    inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if(listen_sock < 0){
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Socket created...");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if(err != 0){
        ESP_LOGE(TAG, "Socket unable to bind: errno %d!", errno);
        close(listen_sock);
        return;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if(err != 0){
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        close(listen_sock);
        return;
    }

    while(1){
        struct sockaddr_storage source_addr;
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if(sock < 0){
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Convert ip address to string
        if(source_addr.ss_family == PF_INET){
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }else if(source_addr.ss_family == PF_INET6){
            inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
        }

        ESP_LOGI(TAG, "Accepted connection with %s", addr_str);
		xQueueSend(socket_queue, &sock, 1000 / portTICK_RATE_MS);
        do_retransmit();

        shutdown(sock, 0);
        close(sock);
    }
    return;
}

void do_retransmit(){
    int len, sock = 0;
    char rx_buffer[50] = {}, response[] = "Hello from Jupiter\0";
    xQueueReceive(socket_queue, &sock, 20);

    do{
        len = recv((int)sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if(len < 0){
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        }else if(len == 0){
            ESP_LOGW(TAG, "Connection closed!");
        }else{
            ESP_LOGI(TAG, "Received %d bytes...\nMessage: %s", len, rx_buffer);
 
            // Make corresponding action
            int to_write = strlen(response);
            int written = 0;
            while(to_write > 0){
                written = send((int)sock, response + written, to_write, 0);
                if(written < 0){
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                }
                to_write -= written;
            }
            ESP_LOGI(TAG, "Sent message response to computer");
        }
    }while(len > 0);
}

void app_main(void){
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(wifi_init_apsta());
    ESP_ERROR_CHECK(init_spiffs());
    ESP_ERROR_CHECK(start_file_server("/spiffs"));
    make_file_data(NULL, 0);

    //ESSENTIAL CODE
    example_ledc_init();
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

    socket_queue = xQueueCreate(1, sizeof(int));
    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 1 | portPRIVILEGE_BIT, NULL);
}