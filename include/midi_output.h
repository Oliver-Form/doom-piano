#ifndef MIDI_OUTPUT_H
#define MIDI_OUTPUT_H

#include <alsa/asoundlib.h>

void midi_output_init(snd_seq_t **seq_handle, int *out_port);
void midi_send_note_on(snd_seq_t *seq_handle, int out_port, int note, int velocity);
void midi_send_note_off(snd_seq_t *seq_handle, int out_port, int note);

#endif
