#ifndef NOTE_H
#define NOTE_H

#include <project.h>

/**
 * Represents a musical pitch as an enum.
 */
typedef enum
{
    Pitch_C,
    Pitch_CSharp,
    Pitch_D,
    Pitch_DSharp,
    Pitch_E,
    Pitch_F,
    Pitch_FSharp,
    Pitch_G,
    Pitch_GSharp,
    Pitch_A,
    Pitch_ASharp,
    Pitch_B,
    Pitch_Rest
} Pitch;

/**
 * Represents a musical note.
 */
typedef struct
{
    Pitch pitch;      /** a Pitch enum value */
    uint8 octave;     /** an integer octave number where middle C is 4 */
    uint16 frequency; /** a frequency in Hz */
    const char *name; /** a string name for the note */
} Note;

/**
 * Represents a note in a MIDI sequence.
 */
typedef struct
{
    Note note;       /** a Note struct */
    uint8 velocity;  /** an integer in [0, 127] where 0 is not pressed and 127 is fully pressed */
    uint16 duration; /** a duration in milliseconds */
} MidiNote;

extern const Note MIDDLE_NOTES[];

/**
 * Creates a Note struct for the given pitch and octave.
 */
Note createNote(const Pitch pitch, const uint8 octave);

/**
 * Plays the given MIDI note for the given duration on the speaker.
 */
void playMidiNote(const MidiNote *midiNote);

#endif
