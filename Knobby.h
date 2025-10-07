//#######################################################################
// Module:     Knobby.h
// Descrption: Process the encoder kob on Lilygo T-Encoder-Pro
// Creator:    markeby
// Date:       9/16/2025
//#######################################################################
#pragma once

//#####################################################################
enum class KNOBBY
    {
    Null = 0,
    Up,
    Down
    };

class KNOBBY_C
    {
private:
    int32_t Data;
    KNOBBY  Trigger;
    uint8_t Previous;
    size_t  CycleTime;

public:
        KNOBBY_C ();
    KNOBBY  ScanLoop (void);
    };

extern KNOBBY_C Knobby;
