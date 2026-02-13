#include "pico/stdlib.h"
#include "tusb.h"
#include <string.h>
#include "hardware/gpio.h"
// Pico W devices use a GPIO on the WIFI chip for the LED
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

#define CMD_LED_ON      0x01
#define CMD_LED_OFF     0x02
#define CMD_LED_BLINK   0x03

#define LED_PIN 25

// ================= FILA DE COMANDOS =================

typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t data[16];
} cmd_t;


// ================= CMD =================
static volatile int cmd_head = 0;
static volatile int cmd_tail = 0;
static cmd_t cmd_queue[4];



// ================= BLINK =================

static int blink_remaining = 0;
static absolute_time_t next_toggle;
static bool led_state = false;


// push não bloqueante
static int push_cmd(const uint8_t *buf, uint32_t len)
{
    int next = (cmd_head + 1) % 4;
    if (next == cmd_tail)
        return -1;

    cmd_queue[cmd_head].cmd = buf[0];
    cmd_queue[cmd_head].len = len - 1;

    if (cmd_queue[cmd_head].len > 16)
        cmd_queue[cmd_head].len = 16;

    memcpy(cmd_queue[cmd_head].data, &buf[1],
           cmd_queue[cmd_head].len);

    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    cmd_head = next;
    return 0;
}


static int pop_cmd(cmd_t *out)
{
    if (cmd_tail == cmd_head)
        return -1; // vazio

    *out = cmd_queue[cmd_tail];
    cmd_tail = (cmd_tail + 1) % 4;
    return 0;
}

// ================= LED =================

int pico_led_init(void)
{
#if defined(PICO_DEFAULT_LED_PIN)
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    return cyw43_arch_init();
#endif
}

void pico_set_led(bool led_on)
{
#if defined(PICO_DEFAULT_LED_PIN)
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

// ================= USB CALLBACK =================

void tud_vendor_rx_cb(uint8_t itf,
                      uint8_t const* buffer,
                      uint16_t bufsize)
{
    uint8_t buf[64];
    uint32_t count;

    // Drena COMPLETAMENTE o endpoint OUT
    while ((count = tud_vendor_available()) > 0) {
        if (count > sizeof(buf))
            count = sizeof(buf);

        tud_vendor_read(buf, count);
        push_cmd(buf, count);
    }
}




// ================= BLINK =================


void blink_task(void)
{
    if (blink_remaining == 0)
        return;

    if (absolute_time_diff_us(get_absolute_time(), next_toggle) <= 0) {
        led_state = !led_state;
        pico_set_led(led_state);
        blink_remaining--;

        if (blink_remaining == 0) {
            pico_set_led(false);   // garante desligado
            led_state = false;
            return;
        }

        next_toggle = make_timeout_time_ms(LED_DELAY_MS);
    }
}




// ================= PROCESSAMENTO =================

void handle_pending_commands(void)
{
    static cmd_t c;

    if (pop_cmd(&c) != 0)
        return;

    int cmd  = c.cmd;
    int plen = c.len;

    switch (cmd) {

    case CMD_LED_ON:
        pico_set_led(true);
        break;

    case CMD_LED_OFF:
        pico_set_led(false);
        break;


case CMD_LED_BLINK:
    if (c.len >= 2) {
        blink_remaining = c.data[1] * 2;
        led_state = false;
        pico_set_led(false);
        next_toggle = make_timeout_time_ms(LED_DELAY_MS);
    }
    break;





    default:
        // comando desconhecido
        break;
    }
}

// ================= MAIN =================

int main(void)
{
    stdio_init_all();
    pico_led_init();
    tusb_init();

    while (true) {
        tud_task();
        handle_pending_commands();
           blink_task();
    }
}


