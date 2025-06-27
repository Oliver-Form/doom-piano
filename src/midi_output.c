#include "midi_output.h"
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

void midi_output_init(snd_seq_t **seq_handle, int *out_port) {
    if (snd_seq_open(seq_handle, "default", SND_SEQ_OPEN_OUTPUT, 0) < 0) {
        fprintf(stderr, "Error opening ALSA sequencer for output.\n");
        exit(1);
    }
    snd_seq_set_client_name(*seq_handle, "MIDI Output");
    *out_port = snd_seq_create_simple_port(*seq_handle, "Output:out",
        SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
        SND_SEQ_PORT_TYPE_APPLICATION);
    if (*out_port < 0) {
        fprintf(stderr, "Error creating output port.\n");
        exit(1);
    }
}

void midi_send_note_on(snd_seq_t *seq_handle, int out_port, int note, int velocity) {
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, out_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_noteon(&ev, 0, note, velocity);
    snd_seq_event_output_direct(seq_handle, &ev);
}

void midi_send_note_off(snd_seq_t *seq_handle, int out_port, int note) {
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, out_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_noteoff(&ev, 0, note, 0);
    snd_seq_event_output_direct(seq_handle, &ev);
}
