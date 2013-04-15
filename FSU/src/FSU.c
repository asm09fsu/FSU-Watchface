#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#define MY_UUID { 0x0A, 0x11, 0x1B, 0x08, 0xE9, 0x88, 0x46, 0xF0, 0x93, 0xB9, 0x3F, 0x78, 0x66, 0x4C, 0xFE, 0xC1 }
PBL_APP_INFO(MY_UUID,
             "FSU Clock", "Alex Muller",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer text_time_layer;
BmpContainer image_container;

void handle_init(AppContextRef ctx) {
    (void)ctx;
    
    window_init(&window, "FSU Clock");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
    
    resource_init_current_app(&RESOURCES);
    
    
    bmp_init_container(RESOURCE_ID_IMAGE_FSU, &image_container);
    layer_add_child(&window.layer, &image_container.layer.layer);
    
    text_layer_init(&text_time_layer, window.layer.frame);
    text_layer_set_text_color(&text_time_layer, GColorWhite);
    text_layer_set_text_alignment(&text_time_layer, GTextAlignmentCenter);
    layer_set_frame(&text_time_layer.layer, GRect(0,116,144,68));
    text_layer_set_background_color(&text_time_layer, GColorClear);
    text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_AVENIR_44)));
    layer_add_child(&window.layer, &text_time_layer.layer);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
    
    (void)ctx;
    
    // Need to be static because they're used by the system later.
    static char time_text[] = "00:00";
    
    char *time_format;
    
    if (clock_is_24h_style()) {
        time_format = "%R";
    } else {
        time_format = "%I:%M";
    }
    
    string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);
    
    // Kludge to handle lack of non-padded hour format string
    // for twelve hour clock.
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
        memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }
    
    text_layer_set_text(&text_time_layer, time_text);
    
}

void handle_deinit(AppContextRef ctx) {
    (void)ctx;
    bmp_deinit_container(&image_container);
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }
        
    };
    app_event_loop(params, &handlers);
}
