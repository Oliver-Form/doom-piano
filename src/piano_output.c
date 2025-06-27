#include <stdio.h>
#include <stdlib.h>
#include "piano_output.h"
#include "note_utils.h"

// This implementation assumes you have wav files named like C4.wav, D#4.wav, etc. in a "piano_samples" directory.
// You can change the path as needed.
void play_piano_note(int note) {
    char cmd[128];
    const char* note_name = midi_note_to_name(note);
    snprintf(cmd, sizeof(cmd), "aplay -q piano_samples/%s.wav &", note_name);
    system(cmd);
}
