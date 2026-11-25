#include "scr_home.h"
#include "screens.h"
#include "ui.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "scr_home";
static lv_timer_t *s_uptime_timer = NULL;

/* ---- Callback para actualizar el Uptime cada 1 segundo ---- */

static void uptime_timer_cb(lv_timer_t *timer)
{
    (void)timer;

    /* esp_timer_get_time() devuelve microsegundos desde el encendido */
    int64_t seg = esp_timer_get_time() / 1000000;
    int h = (int)(seg / 3600);
    int m = (int)((seg % 3600) / 60);
    int s = (int)(seg % 60);

    /* Formato HH:MM:SS */
    lv_label_set_text_fmt(ui_lblUptime, "%02d:%02d:%02d", h, m, s);
}

/* ---- Ciclo de vida ---- */

void scr_home_prepare(void)
{
}

void scr_home_init(void)
{
    ESP_LOGI(TAG, "Pantalla Home cargada con éxito");

    /* Refresco inmediato de entrada */
    uptime_timer_cb(NULL);

    /* Crear timer para actualizar el tiempo activo cada 1000 ms */
    s_uptime_timer = lv_timer_create(uptime_timer_cb, 1000, NULL);
}

void scr_home_uninit(void)
{
    /* Destruir el timer al salir de la pantalla para liberar recursos */
    if (s_uptime_timer) {
        lv_timer_del(s_uptime_timer);
        s_uptime_timer = NULL;
    }
}

void scr_home_step(void)
{
}