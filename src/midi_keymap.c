#include "midi_keymap.h"
#include <stdio.h>
#include <string.h>

midi_action_t midi_keymap[MAX_MIDI_MAP];

static int key_str_to_code(const char *str) {
    if (strcmp(str, "W") == 0) return KEY_W;
    if (strcmp(str, "A") == 0) return KEY_A;
    if (strcmp(str, "S") == 0) return KEY_S;
    if (strcmp(str, "D") == 0) return KEY_D;
    if (strcmp(str, "SPACE") == 0) return KEY_SPACE;
    
    return 0;
}

static int mouse_btn_str_to_code(const char *str) {
    if (strcmp(str, "LEFT") == 0) return BTN_LEFT;
    if (strcmp(str, "RIGHT") == 0) return BTN_RIGHT;
    return 0;
}

static int mouse_move_str_to_dir(const char *str) {
    if (strcmp(str, "UP") == 0) return 0;
    if (strcmp(str, "DOWN") == 0) return 1;
    if (strcmp(str, "LEFT") == 0) return 2;
    if (strcmp(str, "RIGHT") == 0) return 3;
    return -1;
}

void load_midi_keymap(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[128];
    for (int i = 0; i < MAX_MIDI_MAP; ++i) midi_keymap[i].type = ACTION_NONE;
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || strlen(line) < 5) continue;
        int note; char type[16], value[16];
        if (sscanf(line, "%d %15s %15s", &note, type, value) == 3) {
            midi_keymap[note].midi_note = note;
            if (strcmp(type, "KEY") == 0) {
                midi_keymap[note].type = ACTION_KEY;
                midi_keymap[note].value = key_str_to_code(value);
            } else if (strcmp(type, "MOUSE_BTN") == 0) {
                midi_keymap[note].type = ACTION_MOUSE_BTN;
                midi_keymap[note].value = mouse_btn_str_to_code(value);
            } else if (strcmp(type, "MOUSE_MOVE") == 0) {
                midi_keymap[note].type = ACTION_MOUSE_MOVE;
                midi_keymap[note].value = mouse_move_str_to_dir(value);
            }
        }
    }
    fclose(f);
}

const midi_action_t* get_midi_action(int midi_note) {
    if (midi_note < 0 || midi_note >= MAX_MIDI_MAP) return NULL;
    if (midi_keymap[midi_note].type == ACTION_NONE) return NULL;
    return &midi_keymap[midi_note];
}
