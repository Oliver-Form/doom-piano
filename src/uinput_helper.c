#include "uinput_helper.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int uinput_setup() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) return -1;
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    for (int k = 0; k < 256; ++k) ioctl(fd, UI_SET_KEYBIT, k);
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "doom-midi-uinput");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;
    write(fd, &uidev, sizeof(uidev));
    ioctl(fd, UI_DEV_CREATE);
    usleep(100000);
    return fd;
}

void uinput_cleanup(int uinput_fd) {
    if (uinput_fd >= 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
    }
}

void uinput_send_key(int uinput_fd, int keycode, int pressed) {
    struct input_event ev = {0};
    ev.type = EV_KEY;
    ev.code = keycode;
    ev.value = pressed;
    write(uinput_fd, &ev, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(uinput_fd, &ev, sizeof(ev));
}

void uinput_send_mouse_btn(int uinput_fd, int btn, int pressed) {
    struct input_event ev = {0};
    ev.type = EV_KEY;
    ev.code = btn;
    ev.value = pressed;
    write(uinput_fd, &ev, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(uinput_fd, &ev, sizeof(ev));
}

void uinput_send_mouse_move(int uinput_fd, int direction) {
    struct input_event ev = {0};
    ev.type = EV_REL;
    if (direction == 0) { ev.code = REL_Y; ev.value = -10; } // UP
    else if (direction == 1) { ev.code = REL_Y; ev.value = 10; } // DOWN
    else if (direction == 2) { ev.code = REL_X; ev.value = -10; } // LEFT
    else if (direction == 3) { ev.code = REL_X; ev.value = 10; } // RIGHT
    write(uinput_fd, &ev, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(uinput_fd, &ev, sizeof(ev));
}
