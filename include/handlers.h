#ifndef HANDLERS_H
#define HANDLERS_H

#include <project.h>

#include "state.h"

#define DELAY 50
#define VELOCITY_THRESHOLD 100
#define DIVIDED 241000

/**
 * Handles USB communication: downloading a song from the computer.
 *
 * Expects a MIDI file to be in the following format:
 * - size (1 byte): the number of MIDI events (max 256)
 * - events (size bytes): each MIDI event is 5 bytes in the format:
 *     - pitch (1 byte): the enum value of the pitch to play
 *     - octave (1 byte): the octave to play the pitch in
 *     - velocity (1 byte): the velocity of the note in [0, 127]
 *     - duration (2 bytes): the duration of the note (big-endian)
 */
void handleUsb(AppState *appState);

/**
 * Handles Recording mode: augmenting the play mode to also record the song.
 */
void handleRecording(AppState *appState);

/**
 * Handles Playback mode: playing the recorded song.
 */
void handlePlayback(AppState *appState);

/**
 * Handles Play mode: allowing the user to play notes live.
 */
void handlePlay(AppState *appState);

#endif
