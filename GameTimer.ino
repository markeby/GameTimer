//#######################################################################
// Module:     GameTimer.ino
// Descrption: Code for game timer on Lilygo T-Encoder-Pro
// Creator:    markeby
// Date:       9/15/2025
//#######################################################################
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "TouchDrvCHSC5816.hpp"

#include "PinConfig.h"
#include "GameTimer.h"

// LVGL draw into this buffer (in bytes), 1/10 screen size usually works well
#define DRAW_BUF_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t    draw_buf[DRAW_BUF_SIZE / 4];

Arduino_DataBus*    bus     = new Arduino_ESP32QSPI (SCREEN_CS, SCREEN_SCLK, SCREEN_SDIO0, SCREEN_SDIO1, SCREEN_SDIO2, SCREEN_SDIO3);
Arduino_GFX*        gfx     = new Arduino_SH8601    (bus, SCREEN_RST, 0, false, SCREEN_WIDTH, SCREEN_HEIGHT);
TouchDrvCHSC5816    touch;
TouchDrvInterface*  pTouch;
uint64_t            DeltaClock;
PLAYER_C*           Player[MAX_NUMBER_PLAYERS];
PLAYER_C*           ActivePlayer;
int                 CurrentPlayer;
lv_obj_t*           Pages;
GAME_STATE          GameState;
int                 NumberPlayers   = DEFAULT_NUMBER_PLAYERS;
int                 TimerStartValue = DEFAULT_TIME;

//#####################################################################
void CHSC5816_Initialization(void)
    {
    TouchDrvCHSC5816 *pd1 = static_cast<TouchDrvCHSC5816 *>(pTouch);

    touch.setPins(TOUCH_RST, TOUCH_INT);
    if (!touch.begin(Wire, CHSC5816_SLAVE_ADDRESS, IIC_SDA, IIC_SCL))
        {
        printf ("Failed to find CHSC5816!");
        while (1)
            delay(1000);
        }
    }

//#####################################################################
// LVGL calls it when a rendered image needs to copied to the display
void my_disp_flush (lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
    {
    uint32_t w = lv_area_get_width  (area);
    uint32_t h = lv_area_get_height (area);
    gfx->draw16bitRGBBitmap (area->x1, area->y1, (uint16_t *)px_map, w, h);
    lv_display_flush_ready  (disp);         //Call it to tell LVGL you are ready
    }

//#####################################################################
// use Arduinos millis() as tick source
static uint32_t my_tick (void)
    {
    return millis ();
    }

lv_obj_t* RandomPage[MAX_NUMBER_PLAYERS];
bool      RandomFirst = true;
int       RandomTimer = 0;
int       RandomCurrent;

lv_obj_t* NumPlayPage;
byte      NumPlayPageIndex;
lv_obj_t* NumPlaySpinbox;

lv_obj_t* TimeSetPage;
byte      TimeSetPageIndex;
lv_obj_t* TimeSetPageSpinbox;

//#####################################################################
void randomizeLoop ()
    {
    if ( RandomFirst )
        {
        RandomFirst = false;
        RandomCurrent = random (1, 2000) % NumberPlayers;
        lv_tabview_set_act (Pages, MAX_NUMBER_PLAYERS + RandomCurrent, LV_ANIM_OFF);
        RandomTimer = 1000;
        }
    else
        {
        RandomTimer -= DeltaClock;
        if ( RandomTimer < 0 )
            {
            RandomCurrent = ++RandomCurrent % NumberPlayers;
            DbgD (RandomCurrent);
            lv_tabview_set_act (Pages, MAX_NUMBER_PLAYERS + RandomCurrent, LV_ANIM_OFF);
            RandomTimer = 1000;
            }
        }
    }

//#####################################################################
void StateChange (GAME_STATE state)
    {
    switch ( state )
        {
        case GAME_STATE::SET_PLAYERS:
            lv_tabview_set_act  (Pages, NumPlayPageIndex, LV_ANIM_OFF);
            break;
        case GAME_STATE::SET_TIME:
            lv_tabview_set_act  (Pages, TimeSetPageIndex, LV_ANIM_OFF);
            break;
        case GAME_STATE::SELECT_START:
            break;
        case GAME_STATE::IN_PLAY_START:
            lv_tabview_set_act  (Pages, CurrentPlayer, LV_ANIM_OFF);
            break;
        case GAME_STATE::IN_PLAY:
            lv_tabview_set_act  (Pages, CurrentPlayer, LV_ANIM_OFF);
            break;
        default:
            break;
        }
    GameState = state;
    }

//#####################################################################
void PlayerStart (int player)
    {
    CurrentPlayer = player;
    ActivePlayer  = Player[CurrentPlayer];
    ActivePlayer->Start (TimerStartValue);
    }

lv_palette_t Colors[MAX_NUMBER_PLAYERS] = { LV_PALETTE_RED, LV_PALETTE_BLUE, LV_PALETTE_GREEN, LV_PALETTE_PURPLE };
//#####################################################################
void lvgl_init (void)
    {
    lv_obj_t* obj;

    lv_init ();

    // Set a tick source so that LVGL will know how much time elapsed.
    lv_tick_set_cb (my_tick);

    lv_display_t *disp;
    disp = lv_display_create (SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_flush_cb  (disp, my_disp_flush);
    lv_display_set_buffers   (disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    Pages = lv_tabview_create (lv_scr_act ());
    lv_tabview_set_tab_bar_size (Pages, 0);

    // Create players pages
    for ( int z = 0;  z < MAX_NUMBER_PLAYERS;  z++)
        Player[z] = new PLAYER_C(z,  Pages, lv_palette_main (Colors[z]));

    // Create coin toss pages
    static lv_style_t styles[MAX_NUMBER_PLAYERS];
    for ( int z = 0;  z < MAX_NUMBER_PLAYERS;  z++)
        {
        RandomPage[z] = lv_tabview_add_tab (Pages, "");
        lv_style_init (&styles[z]);
        lv_style_set_bg_color (&styles[z], lv_palette_main (Colors[z]));
        obj = lv_obj_create (RandomPage[z]);
        lv_obj_add_style (obj, &styles[z], 0);
        lv_obj_center (obj);
        }

    static lv_style_t spinstyle;
    lv_style_init               (&spinstyle);
    lv_style_set_text_font      (&spinstyle, &Verdanaz_72);
    lv_style_set_bg_opa         (&spinstyle, LV_OPA_TRANSP);
    lv_style_set_border_opa     (&spinstyle, LV_OPA_TRANSP);

    lv_obj_t* label;
    NumPlayPage = lv_tabview_add_tab (Pages, "");
    NumPlayPageIndex = MAX_NUMBER_PLAYERS * 2;
    label = lv_label_create     (NumPlayPage);
    lv_obj_set_style_text_font  (label, &lv_font_montserrat_46, 0);
    lv_label_set_text           (label, "Set players");
    lv_obj_align                (label, LV_ALIGN_TOP_MID, 0, 81);

    NumPlaySpinbox = lv_spinbox_create (NumPlayPage);
    lv_obj_add_style            (NumPlaySpinbox, &spinstyle, 0);
    lv_obj_set_size             (NumPlaySpinbox, 72, 100);
    lv_obj_remove_style         (NumPlaySpinbox, NULL, LV_PART_CURSOR);
    lv_spinbox_set_range        (NumPlaySpinbox, 2, 4);
    lv_spinbox_set_digit_format (NumPlaySpinbox, 1, 0);
    lv_obj_center               (NumPlaySpinbox);
    lv_spinbox_set_value        (NumPlaySpinbox, NumberPlayers);

    TimeSetPage = lv_tabview_add_tab (Pages, "");
    TimeSetPageIndex = NumPlayPageIndex + 1;
    label = lv_label_create     (TimeSetPage);
    lv_obj_set_style_text_font  (label, &lv_font_montserrat_46, 0);
    lv_label_set_text           (label, "Set time");
    lv_obj_align                (label, LV_ALIGN_TOP_MID, 0, 81);

    TimeSetPageSpinbox = lv_spinbox_create (TimeSetPage);
    lv_obj_add_style            (TimeSetPageSpinbox, &spinstyle, 0);
    lv_obj_set_size             (TimeSetPageSpinbox, 160, 100);
    lv_obj_remove_style         (TimeSetPageSpinbox, NULL, LV_PART_CURSOR);
    lv_spinbox_set_range        (TimeSetPageSpinbox, 10, 100);
    lv_spinbox_set_digit_format (TimeSetPageSpinbox, 2, 0);
    lv_obj_center               (TimeSetPageSpinbox);
    lv_spinbox_set_value        (TimeSetPageSpinbox, TimerStartValue);
    }

//#####################################################################
void setup ()
    {
    Serial.begin (115200);

    // inialize touch scrren
    pinMode                 (SCREEN_EN, OUTPUT);
    digitalWrite            (SCREEN_EN, HIGH);
    CHSC5816_Initialization ();

    // initialize encoder knob
    pinMode (KNOB_DATA_A, INPUT_PULLUP);
    pinMode (KNOB_DATA_B, INPUT_PULLUP);

    // initialize buzzer
    pinMode    (BUZZER_DATA, OUTPUT);
    ledcAttach (BUZZER_DATA, 2000, 8);
    ledcWrite  (BUZZER_DATA, 0);

    // initialize graphics subsystem
    gfx->begin      (40000000);
    gfx->fillScreen (BLACK);
    for ( int z = 0;  z <= 255 ; z++ )
        {
        gfx->Display_Brightness (z);
        delay (3);
        }

    // Initialize display
    lvgl_init ();

    // Let's go!
    ActivePlayer = nullptr;
    StateChange (GAME_STATE::SET_PLAYERS);
    }

//#####################################################################
void loop()
    {
    static uint64_t last_time   = 0;    // Previous interval time value to calculate delta time
    static int      last_button = 1;    // Previous button status
    bool            bump = false;
    uint64_t        runtime;

    runtime = micros ();
    DeltaClock = (int)(runtime - last_time);
    last_time = runtime;

    lv_timer_handler ();
    KNOBBY knob = Knobby.ScanLoop ();
    int button = digitalRead (KNOB_KEY);
    if ( button & (button != last_button) )
        bump = true;
    last_button = button;

    switch ( GameState )
        {
        case GAME_STATE::SET_PLAYERS:
            if ( bump )
                StateChange (GAME_STATE::SET_TIME);
            else if ( knob != KNOBBY::Null )
                {
                DbgD (NumberPlayers);
                NumberPlayers = (NumberPlayers + (int)knob) % (MAX_NUMBER_PLAYERS + 1);
                if ( NumberPlayers < MIN_NUMBER_PLAYERS )
                    NumberPlayers = MIN_NUMBER_PLAYERS;
                lv_spinbox_set_value(NumPlaySpinbox, NumberPlayers);
                lv_obj_invalidate (NumPlaySpinbox);
                }

            break;

        case GAME_STATE::SET_TIME:
            if ( bump )
                StateChange (GAME_STATE::SELECT_START);
            else if ( knob != KNOBBY::Null )
                {
                DbgD (TimerStartValue);
                TimerStartValue = TimerStartValue + ((int)knob * 10);
                if ( TimerStartValue > MAX_TIME  )
                    TimerStartValue = MAX_TIME;
                if ( TimerStartValue < MIN_TIME )
                    TimerStartValue = MIN_TIME;
                lv_spinbox_set_value (TimeSetPageSpinbox, TimerStartValue);
                lv_obj_invalidate (NumPlaySpinbox);
                }
            break;

        case GAME_STATE::SELECT_START:
            if ( bump )
                {
                PlayerStart (RandomCurrent);
                StateChange (GAME_STATE::IN_PLAY_START);
                }
            else
                randomizeLoop ();
            break;

        case GAME_STATE::IN_PLAY_START:
            if ( bump )
                StateChange (GAME_STATE::IN_PLAY);
            else if ( knob != KNOBBY::Null )
                StateChange (GAME_STATE::SELECT_START);
            break;

        case GAME_STATE::IN_PLAY:
            if ( bump )
                PlayerStart (++CurrentPlayer % NumberPlayers);
            else if ( knob != KNOBBY::Null )
                StateChange(GAME_STATE::SELECT_START);
            else
                ActivePlayer->Loop ();
            break;

        default:
            break;
        }

    if (  bump )
         delay (500);
    else
        delay (100);
    }
