#include "include/note.h"

#include "include/handlers.h"

const Note MIDDLE_NOTES[] = {
    {Pitch_C, 4, 262, "C"},
    {Pitch_CSharp, 4, 277, "C#"},
    {Pitch_D, 4, 294, "D"},
    {Pitch_DSharp, 4, 311, "D#"},
    {Pitch_E, 4, 330, "E"},
    {Pitch_F, 4, 349, "F"},
    {Pitch_FSharp, 4, 370, "F#"},
    {Pitch_G, 4, 392, "G"},
    {Pitch_GSharp, 4, 415, "G#"},
    {Pitch_A, 4, 440, "A"},
    {Pitch_ASharp, 4, 466, "A#"},
    {Pitch_B, 4, 494, "B"},
    {Pitch_Rest, 0, 0, "REST"}};

Note createNote(const Pitch pitch, const uint8 octave)
{
    Note note;

    note.pitch = pitch;
    note.octave = octave;
    note.frequency = octave >= 4
                         ? MIDDLE_NOTES[pitch].frequency << (octave - 4)
                         : MIDDLE_NOTES[pitch].frequency >> (4 - octave);
    note.name = MIDDLE_NOTES[pitch].name;

    return note;
}

void playMidiNote(const MidiNote *midiNote)
{
    if (midiNote->note.pitch != Pitch_Rest)
    {
        LCD_Char_Position(0, 0);
        LCD_Char_PrintString(midiNote->note.name);
        LCD_Char_PrintNumber(midiNote->note.octave);
        LCD_Char_PrintString(" Vel:");
        LCD_Char_PrintNumber(midiNote->velocity);

        Clock_1_SetDividerValue(DIVIDED / midiNote->note.frequency);
        WaveDAC8_1_Start();
        WaveDAC8_1_SetRange(midiNote->velocity > VELOCITY_THRESHOLD
                                ? WaveDAC8_1_RANGE_4V
                                : WaveDAC8_1_RANGE_1V);
    }

    CyDelay(midiNote->duration);
    WaveDAC8_1_Stop();
}
