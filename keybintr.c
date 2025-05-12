// Keyboard handling through interrupt 09h
// Demonstrates handling of extended scan codes (0xE0 prefix), but ignores translation of scan codes to character codes.

#include <conio.h>
#include <dos.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void (_interrupt _far * old_keyb_handler)(void);

enum { SCAN_ESC = 0x01 };

enum { BUFFER_SIZE = 16 };
static volatile uint16_t buffer[BUFFER_SIZE];
static volatile uint8_t writepos = 0;
static volatile uint8_t readpos = 0;
static volatile uint8_t extended = 0;  // strictly speaking, we shouldn't assume this

// Register reference: https://bitsavers.org/pdf/ibm/pc/xt/6361459_PC_XT_Technical_Reference_Apr84.pdf, p 1-26
void _interrupt keyb_handler(void) {
    uint16_t scancode;
    uint8_t status, new_writepos;

    scancode = inp(0x60);
    status = inp(0x61);

    // acknowledge key by strobing PORTB7
    outp(0x61, status | 0x80);
    outp(0x61, status);

    if (extended) {
        scancode |= 0xE000;
        extended = 0;
    }
    else if (scancode == 0xE0) {
        extended = 1;
        goto end;
    }

    if (scancode & 0x80) {
        // "break" (key released)
    }
    else {
        // "make" (key pressed)
    }

    new_writepos = (writepos + 1) % BUFFER_SIZE;
    if (new_writepos != readpos) {
        buffer[writepos] = scancode;
        writepos = new_writepos;
    }

end:

    // send EOI to PIC
    outp(0x20, 0x20);
}

void keyb_init(void) {
    // need to reset the keybord, enable IRQ or anything? assuming all already done by BIOS.

    _disable();
    old_keyb_handler = _dos_getvect(0x09);
    _dos_setvect(0x09, keyb_handler);
    _enable();
}

void keyb_shutdown(void) {
    _dos_setvect(0x09, old_keyb_handler);
}

void main(void) {
    keyb_init();
    atexit(keyb_shutdown);

    printf("Press any key (ESC to exit). Scan codes will be displayed below:\n");

    for (;;) {
        if (writepos != readpos) {
            // process key press
            uint16_t scancode = buffer[readpos];
            readpos = (readpos + 1) % BUFFER_SIZE;

            printf("%04X ", scancode);
            fflush(stdout);

            if (scancode == SCAN_ESC) {
                break;
            }
        }
    }

    putc('\n', stdout);
}
