// midi_reader.c
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "midi_reader.h"

void midi_key_monitor(void) {
    snd_seq_t *seq_handle;
    int in_port;
    snd_seq_event_t *event;

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
        if (event->type == SND_SEQ_EVENT_NOTEON && event->data.note.velocity > 0) {
            printf("Note ON  - Key: %d  Velocity: %d\n",
                   event->data.note.note, event->data.note.velocity);
        } else if (event->type == SND_SEQ_EVENT_NOTEOFF || 
                   (event->type == SND_SEQ_EVENT_NOTEON && event->data.note.velocity == 0)) {
            printf("Note OFF - Key: %d\n", event->data.note.note);
        }
        fflush(stdout);
    }

    snd_seq_close(seq_handle);
}

int main() {
    midi_key_monitor();
    return 0;
}
