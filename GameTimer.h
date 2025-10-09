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
// system parameter defaults
#define MIN_NUMBER_PLAYERS       2
#define DEFAULT_NUMBER_PLAYERS   2
#define MAX_NUMBER_PLAYERS       4
#define MIN_TIME                10
#define DEFAULT_TIME            80
#define MAX_TIME                100
#define TIME_COUNT_DOWN         10
#define FLASH_RATE              250000      // 1/4 second

//#####################################################################
extern uint64_t    DeltaClock;
extern PLAYER_C*   Player[MAX_NUMBER_PLAYERS];
extern PLAYER_C*   ActivePlayer;
extern int         CurrentPlayer;

//#####################################################################
// State control bytes
enum class GAME_STATE: byte
    {
    SET_PLAYERS = 0,
    SET_TIME,
    SELECT_START,
    IN_PLAY_START,
    IN_PLAY,
    };

//#####################################################################
// Debug macros

#define DbgD(d) {printf("==> %s:%d %s = %d\n",__FILE__,__LINE__, #d, d);}
#define DbgDn(d) {printf("==> %s:%d %s = %d\n\n",__FILE__,__LINE__, #d, d);}
#define DbgX(x) {printf("==> %s:%d %s = 0x%X\n",__FILE__,__LINE__, #x, x);}
#define DbgXn(x) {printf("==> %s:%d %s = 0x%X\n\n",__FILE__,__LINE__, #x, x);}
#define DbgF(f) {printf("==> %s:%d %s = %f\n",__FILE__,__LINE__, #f, f);}
#define DbgFn(f) {printf("==> %s:%d %s = %f\n\n",__FILE__,__LINE__, #f, f);}
#define DbgS(s) {printf("==> %s:%d %s = %s\n",__FILE__,__LINE__, #s, s);}
#define DbgSn(s) {printf("==> %s:%d %s = %s\n\n",__FILE__,__LINE__, #s, s);}

