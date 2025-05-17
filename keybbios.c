// Keyboard input through interrupt 16h (_bios_keybrd)

#include <bios.h>
#include <dos.h>
#include <stdint.h>
#include <stdio.h>

void main(void) {
    printf("Press any key (ESC to exit). Character and scan codes will be displayed below:\n");

    for (;;) {
        if (_bios_keybrd(_KEYBRD_READY)) {
            uint16_t key;
            uint8_t charcode, scancode;

            // PS2_and_PC_BIOS_Interface_Technical_Reference_Apr87.pdf page 4-24 provides an overview of all character and scan codes.
            // Here we use the Standard Function (_KEYBRD_READ), as opposed to the Extended Function (_NKEYBRD_READ).
            // The difference is actually not that big, it just lets you distinguish some keys better (e.g. dedicated arrow keys vs numpad arrows)
            key = _bios_keybrd(_KEYBRD_READ);
            charcode = (key & 0xFF);
            scancode = (key >> 8);

            printf("%02X(%02X)  ", charcode, scancode);
            fflush(stdout);

            if (charcode == 27) {
                break;
            }
        }
    }

    putc('\n', stdout);
}
