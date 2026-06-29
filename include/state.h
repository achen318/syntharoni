#ifndef STATE_H
#define STATE_H

#include <project.h>

#include "note.h"

#define KEY_COUNT 12

/**
 * Stores a button's state and associated read function.
 */
typedef struct
{
    uint8 pressed;       /** 0 if not pressed, 1 if pressed */
    uint8 toggled;       /** 0 if not active, 1 if active */
    uint8 justPressed;   /** 0 if not just pressed, 1 if just pressed */
    uint8 justReleased;  /** 0 if not just released, 1 if just released */
    uint8 (*read)(void); /** function pointer to read the button's value */
} ButtonState;

/**
 * Stores an analog value's state and its associated ADC channel.
 */
typedef struct
{
    uint16 value;       /** an integer value */
    uint8 active;       /** 0 if not active, 1 if active */
    uint8 justPressed;  /** 0 if not just pressed, 1 if just pressed */
    uint8 justReleased; /** 0 if not just released, 1 if just released */
    uint16 channel;     /** the ADC channel associated with this value */
} AnalogState;

/**
 * Discriminator for whether a key is digital (button) or analog (FSR).
 */
typedef enum
{
    KEY_TYPE_DIGITAL,
    KEY_TYPE_ANALOG
} KeyType;

/**
 * Union for storing either an input button value or analog velocity in [0, 127]
 * for a key.
 */
typedef union
{
    ButtonState button;
    AnalogState analog;
} KeyInput;

/**
 * Stores a key's state with input, associated pitch, and velocity.
 */
typedef struct
{
    KeyType type;    /** the type of the key */
    KeyInput input;  /** the input data */
    Pitch pitch;     /** the pitch of the key */
    uint16 velocity; /** the key's velocity in [0, 127] */
} KeyState;

/**
 * Stores state variables for the app.
 */
typedef struct
{
    uint8 usbConfigured; /** 0 if USB not configured, 1 if USB configured */

    ButtonState recordButton; /** button to toggle recording */
    ButtonState playButton;   /** button to toggle playback */

    KeyState keys[KEY_COUNT]; /** array of keys */

    KeyState *pressedKeys[KEY_COUNT]; /** array of pointers to pressed keys */
    size_t numPressedKeys;            /** the number of pressed keys */

    AnalogState octave; /** the current octave in [2, 5] */

    MidiNote song[256]; /** array of MIDI notes */
    size_t songLength;  /** the number of MIDI events */
} AppState;

/**
 * Creates and initializes an AppState struct.
 */
AppState initAppState();

/**
 * Update the button's states and toggled value if shouldToggle is true.
 */
void updateButton(ButtonState *button, uint8 shouldToggle);

/**
 * Updates the analog's states, confined to the range [start, end), activating
 * if the value exceeds the threshold.
 */
void updateAnalog(AnalogState *analog, uint16 start, uint16 end, uint16 threshold);

/**
 * Updates the key's states.
 */
void updateKey(KeyState *key);

/**
 * Updates the app state.
 */
void updateState(AppState *appState);

/**
 * Writes to result an 8-bit ADC value from the specified channel, transformed
 * to fit within the range [start, end).
 */
void readAdc(uint16 *result, uint16 channel, uint16 start, uint16 end);

#endif
