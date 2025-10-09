//#######################################################################
// Module:     Knobby.ino
// Descrption: Process the encoder kob on Lilygo T-Encoder-Pro
// Creator:    markeby
// Date:       9/16/2025
//#######################################################################
#include <Arduino.h>

#include "PinConfig.h"
#include "Knobby.h"

//#######################################################################
   KNOBBY_C::KNOBBY_C ()
    {
    Data = 0;
    Trigger = KNOBBY::Null;
    Previous = 0B00000000;
    CycleTime = 2;
    }

//#######################################################################
KNOBBY KNOBBY_C::ScanLoop (void)
    {
    Trigger   = KNOBBY::Null;

    if ( millis () < CycleTime )
        return (Trigger);

    CycleTime = millis () + 100; // 1000ms for next scan

    uint8_t scan = 0B00000000;

    if ( digitalRead (KNOB_DATA_A) == 1 )
        scan |= 0B00000010;
    else
        scan &= 0B11111101;

    if ( digitalRead (KNOB_DATA_B) == 1 )
        scan |= 0B00000001;
    else
        scan &= 0B11111110;

    if ( Previous != scan )
        {
        if ( (scan == 0B00000000) || (scan == 0B00000011) )
            {
            Previous    = scan;
            return (Trigger);
            }
        else
            {
            if ( scan == 0B00000010 )
                {
                switch ( Previous )
                    {
                    case 0B00000000:
                        Trigger = KNOBBY::Up;
                        break;
                    case 0B00000011:
                        Trigger = KNOBBY::Down;
                        break;

                    default:
                        break;
                    }
                }
            if ( scan == 0B00000001 )
                {
                switch ( Previous )
                    {
                    case 0B00000000:
                        Trigger = KNOBBY::Down;
                        break;
                    case 0B00000011:
                        Trigger = KNOBBY::Up;
                        break;

                    default:
                        break;
                    }
                }
            }
        }
//  if ( PreviousTrigger && Trigger )
//      delay (500);
    PreviousTrigger = Trigger;
    return (Trigger);
    }

//#######################################################################
KNOBBY_C Knobby;

