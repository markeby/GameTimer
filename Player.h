//#######################################################################
// Module:     Player.h
// Descrption: Variables and constants for the game timer on
//             Lilygo T-Encoder-Pro
// Creator:    markeby
// Date:       9/15/2025
//#######################################################################
#pragma once
#include <lvgl.h>

//#####################################################################
#define ONE_MILLION     1000000

extern const lv_font_t Verdanaz_72;
extern const lv_font_t FontVerdanaz142;

//#####################################################################
class PLAYER_C
    {
private:
    int         Number;             // Player Number
    lv_obj_t*   Pages;              // Base page for tabs
    lv_obj_t*   Page;               // This tabbed page
    lv_obj_t*   Arc;                // analog clock
    lv_obj_t*   Label;              // digital clock
    lv_color_t  Color;              // playter unique color
    int         Time;               // Time counter in micro seconds
    int         TimeSeconds;        // Time counter in seconds
    int         CountDown;          // End of time count down (Usually 10 seconds);
    bool        HitZero;            // Hit zero time flag
    int         ZeroPhase;          // Multi phase sounds played at zero time
    int         ZeroTimer;          // timer for each phase
    bool        Flash;              // Flag for flashing display components
    int         FlashTimer;         // timer counter for flashing
    bool        FlashState;         // Current state for flashing
    bool        Active;             // timer is active

 public:
         PLAYER_C   (int number, lv_obj_t* pages, lv_color_t color);
    void Loop       (void);         // Timer loop control
    void Start      (int timer);    // Start this time.
    };

