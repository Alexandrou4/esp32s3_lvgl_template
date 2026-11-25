#include "scr_splash.h"
#include "screens.h"
#include "ui.h"
#include "bsp_display.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "scr_splash";

static lv_timer_t *s_dots_timer = NULL;
static uint8_t s_dots_frame = 0;

static const char *s_dots_frames[] = {
    "Iniciando",
    "Iniciando.",
    "Iniciando..",
    "Iniciando..."
};

/* ---- Animación de fade-in de retroiluminación ---- */

static void backlight_fade_in_1s(void)
{
    /* Iniciar con la pantalla totalmente apagada */
    bsp_display_set_brightness(0);

    /* Rampa suave de 0 a 100% en 100 pasos de 10 ms (1000 ms en total) */
    for (int i = 0; i <= 100; i++) {
        bsp_display_set_brightness(i);
        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

/* ---- Callback de animación de puntos ---- */

static void dots_anim_cb(lv_timer_t *timer)
{
    (void)timer;
    lv_label_set_text(ui_lblIniciando, s_dots_frames[s_dots_frame]);
    s_dots_frame = (s_dots_frame + 1) % 4;
}

/* ---- Callback de evento táctil ---- */

static void splash_touch_cb(lv_event_t *e)
{
    (void)e;
    ESP_LOGI(TAG, "Toque detectado, navegando a Home");
    ui_screens_navigate(SCREEN_HOME);
}

/* ---- Ciclo de vida ---- */

void scr_splash_prepare(void)
{
    /* Asegurar que la pantalla captura el toque */
    lv_obj_add_flag(ui_scrSplash, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_scrSplash, splash_touch_cb, LV_EVENT_CLICKED, NULL);
}

void scr_splash_init(void)
{
    /* Ejecutar el fade-in de 1 segundo */
    backlight_fade_in_1s();

    s_dots_frame = 0;
    lv_label_set_text(ui_lblIniciando, s_dots_frames[0]);
    
    /* Timer para actualizar el texto cada 400 ms */
    s_dots_timer = lv_timer_create(dots_anim_cb, 400, NULL);
}

void scr_splash_uninit(void)
{
    if (s_dots_timer) {
        lv_timer_del(s_dots_timer);
        s_dots_timer = NULL;
    }
}

void scr_splash_step(void)
{
}