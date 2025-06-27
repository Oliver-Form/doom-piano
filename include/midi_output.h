#ifndef MIDI_OUTPUT_H
#define MIDI_OUTPUT_H

#include <alsa/asoundlib.h>

// Initialize MIDI output, returns ALSA seq handle and port number
void midi_output_init(snd_seq_t **seq_handle, int *out_port);
// Send a MIDI Note On event
void midi_send_note_on(snd_seq_t *seq_handle, int out_port, int note, int velocity);
// Send a MIDI Note Off event
void midi_send_note_off(snd_seq_t *seq_handle, int out_port, int note);

#endif // MIDI_OUTPUT_H
