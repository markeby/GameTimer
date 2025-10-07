//#######################################################################
// Module:     GameTimer.ino
// Descrption: Code for game timer on Lilygo T-Encoder-Pro
// Creator:    markeby
// Date:       9/15/2025
//#######################################################################
#include <Arduino.h>
#include "GameTimer.h"
#include <lvgl.h>

#include "PinConfig.h"
#include "Player.h"

//#####################################################################
    PLAYER_C::PLAYER_C (int number, lv_obj_t* pages, lv_color_t color)
    {
    Pages     = pages;
    Time      = 0;
    CountDown = 0;
    HitZero   = true;
    Number    = number;
    Color     = color;

    // Create clock
    Page  = lv_tabview_add_tab (Pages, "");
    Arc   = lv_arc_create (Page);

    lv_obj_set_size            (Arc, 300, 300);
    lv_arc_set_rotation        (Arc, 270);
    lv_arc_set_bg_angles       (Arc, 0, 359);
    lv_arc_set_range           (Arc, 0, ARC_RANGE);
    lv_obj_remove_style        (Arc, NULL, LV_PART_KNOB);      // Be sure the knob is not displayed
    lv_obj_set_style_arc_color (Arc, Color, LV_PART_INDICATOR);
    lv_arc_set_value           (Arc, 10);
    lv_obj_center              (Arc);

    Label = lv_label_create (Arc);                  // digiatl time display
    lv_obj_set_style_text_font  (Label, &FontVerdanaz142, 0);
    lv_obj_set_style_text_color (Label, Color, 0);
    lv_label_set_text           (Label, "00");
    lv_obj_center               (Label);
    lv_obj_align                (Label, LV_ALIGN_CENTER, 0, 0);

    lv_timer_t* flash_timer;
    }

//#####################################################################
void PLAYER_C::Start ()
    {
    ZeroTimer  = -1;
    Time       = (TIME_PER_TURN_IN_SECONDS + 1) * ONE_MILLION;
    CountDown  = 0;
    FlashTimer = FLASH_RATE;
    FlashState = true;
    Flash      = false;
    HitZero    = false;
    Active     = true;

    lv_arc_set_value      (Arc, ARC_RANGE);
    lv_label_set_text_fmt (Label, "%d", TIME_PER_TURN_IN_SECONDS);
    lv_tabview_set_act    (Pages, (byte)Number, LV_ANIM_OFF);
    lv_obj_clear_flag     (Label, LV_OBJ_FLAG_HIDDEN); // Show the object
    ledcWrite             (BUZZER_DATA, 0);          // make sure sound is turned off
    }

//#####################################################################
void PLAYER_C::Loop ()
    {
    if ( Active )
        {
        if ( Time > 0 )
            {
            Time -= DeltaClock;
            if ( Time < 0 )
                 Time = 0;

            int t = Time / 10000;
            lv_arc_set_value (Arc, t);
            t = (t + 99) / 100;
            if ( t > TIME_PER_TURN_IN_SECONDS )
                t = TIME_PER_TURN_IN_SECONDS;
            lv_label_set_text_fmt (Label, "%d", t);

            if ( (t <= TIME_COUNT_DOWN) && (t != TimeSeconds) )
                {
                if ( CountDown == 0 )
                    CountDown = TIME_COUNT_DOWN;
                else
                    CountDown--;
                if ( t == 0 )
                    {
                    HitZero   = true;
                    Flash     = true;
                    ZeroTimer = -1;
                    ZeroPhase = 4;
                    }
                else
                    {
                    ledcWriteNote (BUZZER_DATA, (note_t)CountDown, 4);
                    delay         (100);
                    ledcWrite     (BUZZER_DATA, 0);          // turn it off
                    }
                }
            TimeSeconds = t;
            }
        if ( HitZero )
            {
            if ( ZeroTimer < 0 )
                {
                ZeroTimer = 1 * 1000000;
                switch ( ZeroPhase )
                    {
                    case 4:
                        ledcWriteNote (BUZZER_DATA, note_t::NOTE_D, 4);
                        break;
                    case 3:
                        ledcWriteNote (BUZZER_DATA, note_t::NOTE_Cs, 4);
                        break;
                    case 2:
                        ledcWriteNote (BUZZER_DATA, note_t::NOTE_C, 4);
                        break;
                    case 1:
                        ledcWriteNote (BUZZER_DATA, note_t::NOTE_B, 3);
                        break;
                    case 0:
                        ledcWrite (BUZZER_DATA, 0);          // turn it off
                        Active = false;
                        break;
                    default:
                        break;
                    }
                ZeroPhase--;
                }
            else
                ZeroTimer -= DeltaClock;
            }
        }

    if ( Flash )
        {
        if ( FlashTimer < 0 )
            {
            FlashState = !FlashState;
            if ( FlashState )
                lv_obj_clear_flag (Label, LV_OBJ_FLAG_HIDDEN);  // Show the object
            else
                lv_obj_add_flag   (Label, LV_OBJ_FLAG_HIDDEN);  // Hide the object
            FlashTimer = FLASH_RATE;
            }
        else
            FlashTimer -= DeltaClock;
        }
    }

