#include <stdio.h>
#include "note_utils.h"

const char* midi_note_to_name(int note) {
    static char name[4];
    const char* note_names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (note / 12) - 1;
    int note_in_octave = note % 12;
    snprintf(name, sizeof(name), "%s%d", note_names[note_in_octave], octave);
    return name;
}
