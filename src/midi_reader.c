// midi_reader.c
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "midi_reader.h"
#include "midi_output.h"
#include "note_utils.h"

void midi_send_program_change(snd_seq_t *seq_handle, int out_port, int program) {
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, out_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_pgmchange(&ev, 0, program); // channel 0
    snd_seq_event_output_direct(seq_handle, &ev);
}

void midi_key_monitor(void) {
    snd_seq_t *seq_handle;
    int in_port;
    snd_seq_event_t *event;

    // MIDI output setup
    snd_seq_t *out_seq_handle;
    int out_port;
    midi_output_init(&out_seq_handle, &out_port);

    // Open sequencer
    if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_INPUT, 0) < 0) {
        fprintf(stderr, "Error opening ALSA sequencer.\n");
        exit(1);
    }

    snd_seq_set_client_name(seq_handle, "MIDI Key Monitor");
    in_port = snd_seq_create_simple_port(seq_handle, "Listen:in",
        SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_APPLICATION);

    if (in_port < 0) {
        fprintf(stderr, "Error creating sequencer port.\n");
        exit(1);
    }

    printf("Listening for MIDI Note On/Off events...\n");

    while (1) {
        snd_seq_event_input(seq_handle, &event);
        printf("DEBUG: Received event type: %d\n", event->type);
        fflush(stdout);
        if (event->type == SND_SEQ_EVENT_NOTEON && event->data.note.velocity > 0) {
            printf("Note ON  - Key: %d (%s)  Velocity: %d\n",
                   event->data.note.note,
                   midi_note_to_name(event->data.note.note),
                   event->data.note.velocity);
            midi_send_note_on(out_seq_handle, out_port, event->data.note.note, event->data.note.velocity);
        } else if (event->type == SND_SEQ_EVENT_NOTEOFF || 
                   (event->type == SND_SEQ_EVENT_NOTEON && event->data.note.velocity == 0)) {
            printf("Note OFF - Key: %d (%s)\n",
                   event->data.note.note,
                   midi_note_to_name(event->data.note.note));
            midi_send_note_off(out_seq_handle, out_port, event->data.note.note);
        }
        fflush(stdout);
    }

    snd_seq_close(seq_handle);
}

int main() {
    midi_key_monitor();
    return 0;
}
