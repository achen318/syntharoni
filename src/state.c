#include "include/state.h"

AppState initAppState()
{
    return (AppState){
        .recordButton = {.read = RecordButton_Read},
        .playButton = {.read = PlayButton_Read},
        .keys = {
            {.type = KEY_TYPE_ANALOG,
             .pitch = Pitch_C,
             .input.analog = {.channel = 1}},
            {.type = KEY_TYPE_DIGITAL,
             .pitch = Pitch_CSharp,
             .input.button = {.read = Key_CSharp_Read}},
            {.type = KEY_TYPE_ANALOG,
             .pitch = Pitch_D,
             .input.analog = {.channel = 2}},
            {.type = KEY_TYPE_DIGITAL,
             .pitch = Pitch_DSharp,
             .input.button = {.read = Key_DSharp_Read}},
            {.type = KEY_TYPE_ANALOG,
             .pitch = Pitch_E,
             .input.analog = {.channel = 3}},
            {.type = KEY_TYPE_ANALOG,
             .pitch = Pitch_F,
             .input.analog = {.channel = 4}},
            {.type = KEY_TYPE_DIGITAL,
             .pitch = Pitch_FSharp,
             .input.button = {.read = Key_FSharp_Read}},
            {.type = KEY_TYPE_ANALOG,
             .pitch = Pitch_G,
             .input.analog = {.channel = 5}},
            {.type = KEY_TYPE_DIGITAL,
             .pitch = Pitch_GSharp,
             .input.button = {.read = Key_GSharp_Read}},
            {.type = KEY_TYPE_ANALOG,
             .pitch = Pitch_A,
             .input.analog = {.channel = 6}},
            {.type = KEY_TYPE_DIGITAL,
             .pitch = Pitch_ASharp,
             .input.button = {.read = Key_ASharp_Read}},
            {.type = KEY_TYPE_ANALOG,
             .pitch = Pitch_B,
             .input.analog = {.channel = 7}}}};
}

void updateButton(ButtonState *button, uint8 shouldToggle)
{
    const uint8 value = !button->read();

    button->justPressed = !button->pressed && value;
    button->justReleased = button->pressed && !value;

    if (shouldToggle && button->justPressed)
        button->toggled ^= 1;

    button->pressed = value;
}

void updateAnalog(AnalogState *analog, uint16 start, uint16 end, uint16 threshold)
{
    readAdc(&analog->value, analog->channel, start, end);
    const uint8 prevState = analog->active;

    analog->active = analog->value > threshold;
    analog->justPressed = !prevState && analog->value > threshold;
    analog->justReleased = prevState && analog->value <= threshold;
}

void updateKey(KeyState *key)
{
    if (key->type == KEY_TYPE_DIGITAL)
    {
        updateButton(&key->input.button, 0);
        key->velocity = key->input.button.pressed ? 127 : 0;
    }
    else
    {
        updateAnalog(&key->input.analog, 0, 128, 10);
        key->velocity = key->input.analog.value;
    }
}

void updateState(AppState *appState)
{
    // Update USB configuration
    if (!appState->usbConfigured && USB_GetConfiguration())
    {
        appState->usbConfigured = 1;
        USB_EnableOutEP(2);
    }

    // Update record button when not in Playback
    updateButton(&appState->recordButton, !appState->playButton.toggled);

    // Update playback button when not in Recording
    updateButton(&appState->playButton, !appState->recordButton.toggled);

    // Update keys and pressedKeys
    appState->numPressedKeys = 0;

    size_t i;
    for (i = 0; i < KEY_COUNT; ++i)
    {
        updateKey(&appState->keys[i]);

        if ((appState->keys[i].type == KEY_TYPE_DIGITAL &&
             appState->keys[i].input.button.pressed) ||
            (appState->keys[i].type == KEY_TYPE_ANALOG &&
             appState->keys[i].input.analog.active))
            appState->pressedKeys[appState->numPressedKeys++] = &appState->keys[i];
    }

    // Update octave based on potentiometer
    updateAnalog(&appState->octave, 2, 6, 0);
}

void readAdc(uint16 *result, uint16 channel, uint16 start, uint16 end)
{
    if (ADC_SAR_Seq_IsEndConversion(ADC_SAR_Seq_WAIT_FOR_RESULT))
    {
        uint16 value = ADC_SAR_Seq_GetResult16(channel);

        /* Fix underflow and overflow */
        if (value & 0x8000)
            value = 0;
        else if (value >= 0x7f)
            value = 0x7f;

        /* Transform to [start, end) */
        *result = (end - start) * value / 0x80 + start;
    }
}
