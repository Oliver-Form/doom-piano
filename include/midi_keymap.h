#ifndef MIDI_KEYMAP_H
#define MIDI_KEYMAP_H

#include <linux/uinput.h>

#define MAX_MIDI_MAP 128

typedef enum {
    ACTION_NONE = 0,
    ACTION_KEY,
    ACTION_MOUSE_BTN,
    ACTION_MOUSE_MOVE
} action_type_t;

typedef struct {
    int midi_note;
    action_type_t type;
    int value; 
} midi_action_t;

extern midi_action_t midi_keymap[MAX_MIDI_MAP];

void load_midi_keymap(const char *filename);
const midi_action_t* get_midi_action(int midi_note);

#endif 
