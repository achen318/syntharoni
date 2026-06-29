#include "include/handlers.h"

#include "include/note.h"

/**
 * Gets the index of the key to play in the current time slice, or KEY_COUNT if
 * no keys are pressed.
 */
static size_t getNextKeyIndex(AppState *appState)
{
    static size_t sliceIndex = 0;

    if (appState->numPressedKeys == 0)
        return KEY_COUNT;

    if (sliceIndex >= appState->numPressedKeys)
        sliceIndex = 0;

    return sliceIndex++;
}

void handleUsb(AppState *appState)
{
    // Stop if USB is not configured, we have no data to read,
    // or we are recording or playing back
    if (!appState->usbConfigured || USB_GetEPState(2) == USB_OUT_BUFFER_EMPTY ||
        appState->recordButton.toggled || appState->playButton.toggled)
        return;

    // Accumulate data
    static uint8 buffer[1 + 256 * 5];
    static uint16 bytesReceived = 0;

    uint16 bytesAvailable = USB_GetEPCount(2);
    USB_ReadOutEP(2, &buffer[bytesReceived], bytesAvailable);
    bytesReceived += bytesAvailable;

    // Retrieve the song length and wait until we have the entire song
    if (bytesReceived == 0)
        return;

    const uint8 songLength = buffer[0];

    if (bytesReceived < 1 + songLength * 5)
        return;

    // Parse the song
    appState->songLength = songLength;

    size_t i;
    for (i = 0; i < appState->songLength; ++i)
    {
        const size_t offset = 1 + i * 5;

        appState->song[i] = (MidiNote){
            .note = createNote((Pitch)buffer[offset], buffer[offset + 1]),
            .velocity = buffer[offset + 2],
            .duration = (buffer[offset + 3] << 8) | buffer[offset + 4]};
    }

    bytesReceived = 0;
}

void handleRecording(AppState *appState)
{
    // Remove a trailing rest if we stop recording
    if (appState->recordButton.justReleased &&
        appState->songLength > 0 &&
        appState->song[appState->songLength - 1].note.pitch == Pitch_Rest)
        --appState->songLength;

    // Stop if we are not recording
    if (!appState->recordButton.toggled)
        return;

    // Start a new song if we start recording
    if (appState->recordButton.justPressed)
        appState->songLength = 0;

    LCD_Char_Position(1, 0);
    LCD_Char_PrintString("Recording...");

    size_t keyIndex = getNextKeyIndex(appState);
    static Pitch prevPitch = Pitch_Rest;

    // No keys pressed, so add a rest if we are not already resting
    if (keyIndex == KEY_COUNT)
    {
        if (prevPitch != Pitch_Rest)
        {
            appState->song[appState->songLength++] = (MidiNote){
                .note = createNote(Pitch_Rest, 0),
                .velocity = 0,
                .duration = 0};
            prevPitch = Pitch_Rest;
        }
    }

    else
    {
        KeyState playedKey = *appState->pressedKeys[keyIndex];

        // Add the note if we are not already playing it
        if (playedKey.pitch != prevPitch)
        {
            appState->song[appState->songLength++] = (MidiNote){
                .note = createNote(playedKey.pitch, appState->octave.value),
                .velocity = playedKey.velocity,
                .duration = 0};
            prevPitch = playedKey.pitch;
        }

        // Update current note velocity to be the max of velocities
        else if (appState->song[appState->songLength - 1].velocity < playedKey.velocity)
            appState->song[appState->songLength - 1].velocity = playedKey.velocity;
    }

    // Increment duration of the current note/rest
    if (appState->songLength > 0)
        appState->song[appState->songLength - 1].duration += DELAY;
}

void handlePlayback(AppState *appState)
{
    if (appState->playButton.toggled)
    {
        size_t i;

        // Play the song (blocking)
        for (i = 0; i < appState->songLength; ++i)
        {
            LCD_Char_ClearDisplay();
            LCD_Char_Position(1, 0);
            LCD_Char_PrintString("Playing...");

            playMidiNote(&appState->song[i]);
        }

        // Return from playback mode
        appState->playButton.toggled = 0;
    }
}

void handlePlay(AppState *appState)
{
    if (!appState->playButton.toggled)
    {
        size_t keyIndex = getNextKeyIndex(appState);

        if (keyIndex == KEY_COUNT)
        {
            WaveDAC8_1_Stop();
            return;
        }

        WaveDAC8_1_Start();
        KeyState playedKey = *appState->pressedKeys[keyIndex];

        // Adjust volume based on velocity
        WaveDAC8_1_SetRange(playedKey.velocity > VELOCITY_THRESHOLD
                                ? WaveDAC8_1_RANGE_4V
                                : WaveDAC8_1_RANGE_1V);

        // Play the note
        Note toPlay = createNote(playedKey.pitch, appState->octave.value);

        Clock_1_SetDividerValue(DIVIDED / toPlay.frequency);

        LCD_Char_Position(0, 0);
        LCD_Char_PrintString(toPlay.name);
        LCD_Char_PrintNumber(toPlay.octave);
        LCD_Char_PrintString(" Vel:");
        LCD_Char_PrintNumber(playedKey.velocity);
    }
}
