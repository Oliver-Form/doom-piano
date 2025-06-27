#ifndef UINPUT_HELPER_H
#define UINPUT_HELPER_H

#include <linux/uinput.h>

int uinput_setup();
void uinput_cleanup(int uinput_fd);
void uinput_send_key(int uinput_fd, int keycode, int pressed);
void uinput_send_mouse_btn(int uinput_fd, int btn, int pressed);
void uinput_send_mouse_move(int uinput_fd, int direction);

#endif
