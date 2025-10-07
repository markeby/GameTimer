//#######################################################################
// Module:     GameTimer.h
// Descrption: Variables and constants for the game timer on
//             Lilygo T-Encoder-Pro
// Creator:    markeby
// Date:       9/15/2025
//#######################################################################
#pragma once
#include <lvgl.h>

#include "Player.h"
#include "Knobby.h"

//#####################################################################
// Most important is the number of players and the turn interval
#define NUMBER_OF_PLAYERS           2
#define TIME_PER_TURN_IN_SECONDS    80
#define TIME_COUNT_DOWN             10
#define FLASH_RATE                  250000      // 1/4 second

//#####################################################################
extern uint64_t    DeltaClock;
extern PLAYER_C*   Player[NUMBER_OF_PLAYERS];
extern PLAYER_C*   ActivePlayer;
extern int         CurrentPlayer;

//#####################################################################
extern const lv_font_t FontVerdanaz142;

//#####################################################################
// Constants that are never altered
#define ONE_MILLION     1000000
#define ARC_RANGE       (TIME_PER_TURN_IN_SECONDS * 100)

#define DbgD(d) {printf("==> %s:%d %s = %d\n",__FILE__,__LINE__, #d, d);}
#define DbgX(x) {printf("==> %s:%d %s = 0x%X\n",__FILE__,__LINE__, #x, x);}
#define DbgF(f) {printf("==> %s:%d %s = %f\n",__FILE__,__LINE__, #f, f);}
#define DbgS(s) {printf("==> %s:%d %s = %s\n",__FILE__,__LINE__, #s, s);}
#define DbgN    {printf("\n");}

