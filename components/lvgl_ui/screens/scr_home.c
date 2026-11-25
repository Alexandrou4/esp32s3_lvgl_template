#include "scr_home.h"
#include "screens.h"
#include "ui.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/temperature_sensor.h"

static const char *TAG = "scr_home";
static lv_timer_t *s_home_timer = NULL;
static temperature_sensor_handle_t s_temperature_sensor = NULL;

/* ---- Callback para actualizar los datos cada 1 segundo ---- */

static void home_timer_cb(lv_timer_t *timer)
{
    (void)timer;

    /* esp_timer_get_time() devuelve microsegundos desde el encendido */
    int64_t seg = esp_timer_get_time() / 1000000;
    int h = (int)(seg / 3600);
    int m = (int)((seg % 3600) / 60);
    int s = (int)(seg % 60);

    /* Formato HH:MM:SS */
    lv_label_set_text_fmt(ui_lblUptime, "%02d:%02d:%02d", h, m, s);

    if (s_temperature_sensor) {
        float temperature_celsius;
        esp_err_t err = temperature_sensor_get_celsius(s_temperature_sensor, &temperature_celsius);
        if (err == ESP_OK) {
            lv_label_set_text_fmt(ui_lblTemperatura, "%d °C", (int)(temperature_celsius + 0.5f));
        } else {
            ESP_LOGW(TAG, "No se pudo leer la temperatura: %s", esp_err_to_name(err));
        }
    }
}

/* ---- Ciclo de vida ---- */

void scr_home_prepare(void)
{
    temperature_sensor_config_t config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    temperature_sensor_handle_t sensor = NULL;
    esp_err_t err = temperature_sensor_install(&config, &sensor);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "No se pudo instalar el sensor de temperatura: %s", esp_err_to_name(err));
        return;
    }

    err = temperature_sensor_enable(sensor);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "No se pudo habilitar el sensor de temperatura: %s", esp_err_to_name(err));
        esp_err_t uninstall_err = temperature_sensor_uninstall(sensor);
        if (uninstall_err != ESP_OK) {
            ESP_LOGE(TAG, "No se pudo desinstalar el sensor de temperatura: %s",
                     esp_err_to_name(uninstall_err));
        }
        return;
    }

    s_temperature_sensor = sensor;
}

void scr_home_init(void)
{
    ESP_LOGI(TAG, "Pantalla Home cargada con éxito");

    /* Refresco inmediato de entrada */
    home_timer_cb(NULL);

    /* Actualizar los datos cada 1000 ms */
    s_home_timer = lv_timer_create(home_timer_cb, 1000, NULL);
}

void scr_home_uninit(void)
{
    /* Destruir el timer al salir de la pantalla para liberar recursos */
    if (s_home_timer) {
        lv_timer_del(s_home_timer);
        s_home_timer = NULL;
    }
}

void scr_home_step(void)
{
}