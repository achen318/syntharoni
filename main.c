#include <project.h>

#include "include/handlers.h"
#include "include/note.h"
#include "include/state.h"

void init()
{
    CYGlobalIntEnable;

    USB_Start(0, USB_3V_OPERATION);

    LCD_Char_Start();
    LCD_Char_ClearDisplay();

    ADC_SAR_Seq_Start();
    ADC_SAR_Seq_StartConvert();

    WaveDAC8_1_Start();
}

int main()
{
    init();

    AppState appState = initAppState();

    for (;;)
    {
        LCD_Char_ClearDisplay();
        updateState(&appState);

        handleUsb(&appState);
        handleRecording(&appState);
        handlePlayback(&appState);
        handlePlay(&appState);

        CyDelay(DELAY);
    }
}

/* [] END OF FILE */
