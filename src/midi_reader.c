#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "midi_reader.h"
#include "midi_output.h"
#include "note_utils.h"
#include "midi_keymap.h"
#include "uinput_helper.h"
#include <stdbool.h>

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
    snd_seq_t *seq_handle = NULL;
    snd_seq_t *out_seq_handle = NULL;
    int in_port = -1, out_port = -1;
    snd_seq_event_t *event = NULL;

    midi_output_init(&out_seq_handle, &out_port);

    if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_INPUT, 0) < 0) {
        fprintf(stderr, "Error opening ALSA sequencer.\n");
        snd_seq_close(out_seq_handle);
        exit(EXIT_FAILURE);
    }

    snd_seq_set_client_name(seq_handle, "MIDI Key Monitor");
    in_port = snd_seq_create_simple_port(seq_handle, "Listen:in",
        SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_APPLICATION);

    if (in_port < 0) {
        fprintf(stderr, "Error creating sequencer port.\n");
        snd_seq_close(seq_handle);
        snd_seq_close(out_seq_handle);
        exit(EXIT_FAILURE);
    }

    midi_send_program_change(out_seq_handle, out_port, 24);

    printf("Listening for MIDI Note On/Off events...\n");

    load_midi_keymap("include/midi_keymap.txt");
    int uinput_fd = uinput_setup();
    if (uinput_fd < 0) {
        fprintf(stderr, "Error setting up uinput. Try running as root or check /dev/uinput permissions.\n");
        snd_seq_close(seq_handle);
        snd_seq_close(out_seq_handle);
        exit(EXIT_FAILURE);
    }

    while (true) {
        if (snd_seq_event_input(seq_handle, &event) < 0) {
            fprintf(stderr, "Error reading MIDI event.\n");
            break;
        }
        printf("DEBUG: Received event type: %d\n", event->type);

        const midi_action_t *action = get_midi_action(event->data.note.note);
        if (event->type == SND_SEQ_EVENT_NOTEON && event->data.note.velocity > 0) {
            printf("Note ON  - Key: %d (%s)  Velocity: %d\n",
                   event->data.note.note,
                   midi_note_to_name(event->data.note.note),
                   event->data.note.velocity);
            if (action) {
                if (action->type == ACTION_KEY) uinput_send_key(uinput_fd, action->value, 1);
                else if (action->type == ACTION_MOUSE_BTN) uinput_send_mouse_btn(uinput_fd, action->value, 1);
                else if (action->type == ACTION_MOUSE_MOVE) uinput_send_mouse_move(uinput_fd, action->value);
            }
            midi_send_note_on(out_seq_handle, out_port, event->data.note.note, event->data.note.velocity);
        } else if (event->type == SND_SEQ_EVENT_NOTEOFF ||
                   (event->type == SND_SEQ_EVENT_NOTEON && event->data.note.velocity == 0)) {
            printf("Note OFF - Key: %d (%s)\n",
                   event->data.note.note,
                   midi_note_to_name(event->data.note.note));
            if (action) {
                if (action->type == ACTION_KEY) uinput_send_key(uinput_fd, action->value, 0);
                else if (action->type == ACTION_MOUSE_BTN) uinput_send_mouse_btn(uinput_fd, action->value, 0);
            }
            midi_send_note_off(out_seq_handle, out_port, event->data.note.note);
        }
    }

    uinput_cleanup(uinput_fd);
    snd_seq_close(seq_handle);
    snd_seq_close(out_seq_handle);
}

int main(void) {
    midi_key_monitor();
    return 0;
}
