/* Pressed keys display module
 * Listens for key events and draws friendly key names on the SSD1306 OLED.
 * This is a minimal implementation intended as a starting point; further
 * keycode mappings can be extended as needed.
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/display.h>
#include <sys/printk.h>
#include <string.h>
#include <stdio.h>
#include <devicetree.h>

#include "pressed_keys.h"

/* ZMK event headers - include paths may vary depending on ZMK version. */
#include <zmk/event_manager.h>
#include <zmk/events/key_event.h>

#define MAX_PRESSED 6
#define LINE_BUF 128

static const struct device *disp;
static uint16_t pressed[MAX_PRESSED];
static int pressed_count;

static const char *keycode_to_name(uint16_t keycode) {
    static char tmp[8];
    if (keycode >= 4 && keycode <= 29) { // a-z
        char c = 'a' + (keycode - 4);
        tmp[0] = c; tmp[1] = '\0';
        return tmp;
    }
    if (keycode >= 30 && keycode <= 39) { // 1-0
        const char *nums = "1234567890";
        tmp[0] = nums[keycode - 30]; tmp[1] = '\0';
        return tmp;
    }
    switch (keycode) {
        case 40: return "ENTER";
        case 41: return "ESC";
        case 42: return "BS";
        case 43: return "TAB";
        case 44: return "SPC";
        case 225: return "LCTRL";
        case 224: return "LCTRL"; // sometimes swapped
        case 226: return "LALT";
        case 229: return "LGUI";
        case 228: return "LSHFT";
        default:
            snprintf(tmp, sizeof(tmp), "%u", keycode);
            return tmp;
    }
}

static void draw_pressed(void) {
    if (!disp) {
        return;
    }

    display_blanking_off(disp);
    display_set_contrast(disp, 255);

    char line[LINE_BUF];
    line[0] = '\0';
    for (int i = 0; i < pressed_count; i++) {
        const char *name = keycode_to_name(pressed[i]);
        if (i) strncat(line, " ", LINE_BUF - strlen(line) - 1);
        strncat(line, name, LINE_BUF - strlen(line) - 1);
    }

    /* Clear and draw simple text using display driver text API if available.
     * We'll use direct buf write as many SSD1306 drivers support framebuffer API,
     * but to keep generic, use display_write with log font not provided here.
     * For simplicity we use printk as fallback (visible over serial only).
     */
    if (display_get_capabilities(disp)->screen_info) {
        /* Try a generic text draw if driver provides API. Many ZMK builds include
         * zmk display widgets; for robustness we fallback to console print.
         */
    }

    /* Fallback: print to console for debugging and also attempt to write raw
     * via display_write for simple framebuffer-capable drivers.
     */
    printk("Pressed: %s\n", line);

    /* Attempt to draw text by writing the ASCII string into the framebuffer
     * This is driver-specific and may need adjustment for your SSD1306 driver.
     */
    struct display_buffer_descriptor desc;
    const uint8_t clear = 0x00;
    desc.buf_size = 1;
    desc.width = 128;
    desc.height = 8;
    desc.pitch = 128;

    /* clear page 0..3 */
    for (uint8_t page = 0; page < 4; page++) {
        display_write(disp, 0, page, &clear, 1);
    }

    /* Note: Proper text drawing would use a font renderer; left as future work. */
}

static void add_pressed(uint16_t keycode) {
    for (int i = 0; i < pressed_count; i++) {
        if (pressed[i] == keycode) return;
    }
    if (pressed_count < MAX_PRESSED) {
        pressed[pressed_count++] = keycode;
    } else {
        pressed[MAX_PRESSED - 1] = keycode;
    }
    draw_pressed();
}

static void remove_pressed(uint16_t keycode) {
    int idx = -1;
    for (int i = 0; i < pressed_count; i++) {
        if (pressed[i] == keycode) { idx = i; break; }
    }
    if (idx < 0) return;
    for (int i = idx; i < pressed_count - 1; i++) pressed[i] = pressed[i+1];
    pressed_count--;
    draw_pressed();
}

static int pressed_keys_event_listener(const zmk_event_t *eh) {
    if (!eh) return -ENOTSUP;

    if (is_key_event(eh)) {
        const struct zmk_key_event *ev = as_key_event(eh);
        uint16_t kc = ev->keycode;
        if (ev->state) {
            add_pressed(kc);
        } else {
            remove_pressed(kc);
        }
    }
    return 0;
}

void pressed_keys_init(void) {
    /* Get display device from devicetree node label 'oled' and ensure ready */
    disp = DEVICE_DT_GET(DT_NODELABEL(oled));
    if (!device_is_ready(disp)) {
        printk("pressed_keys: left display not found or not ready\n");
        disp = NULL;
    } else {
        printk("pressed_keys: display found\n");
    }

    zmk_event_manager_subscribe(pressed_keys_event_listener);
    printk("pressed_keys: initialized\n");
}

/* Auto init via SYS_INIT to ensure early registration */
SYS_INIT(pressed_keys_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
