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
PLAYER_C*           Player[NUMBER_OF_PLAYERS];
PLAYER_C*           ActivePlayer;
int                 CurrentPlayer;
lv_obj_t*           Pages;
bool                StartState;

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
// Read the touchpad  ***** not using*****
//void my_touchpad_read (lv_indev_t *indev, lv_indev_data_t *data)
//    {
//    int16_t Touch_x[2], Touch_y[2];
//    uint8_t touchpad = touch.getPoint (Touch_x, Touch_y);
//
//    if ( touchpad > 0 )
//        {
//        data->state = LV_INDEV_STATE_PR;
//
//        /*Set the coordinates*/
//        data->point.x = Touch_x[0];
//        data->point.y = Touch_y[0];
//
//        // Serial.print("Data x ");
//        // Serial.printf("%d\n", x[0]);
//
//        // Serial.print("Data y ");
//        // Serial.printf("%d\n", y[0]);
//        }
//    else
//        data->state = LV_INDEV_STATE_REL;
//    }

//#####################################################################
// use Arduinos millis() as tick source
static uint32_t my_tick (void)
    {
    return millis ();
    }

lv_obj_t* RandomPage[NUMBER_OF_PLAYERS];
bool      RandomFirst = true;
int       RandomTimer = 0;
int       RandomCurrent;
//#####################################################################
void randomizeLoop ()
    {
    if ( RandomFirst )
        {
        RandomFirst = false;
        RandomCurrent = random (1, 200000) % NUMBER_OF_PLAYERS;
        lv_tabview_set_act (Pages, NUMBER_OF_PLAYERS + RandomCurrent, LV_ANIM_OFF);
        RandomTimer = 1000;
        }
    else
        {
        RandomTimer -= DeltaClock;
        if ( RandomTimer < 0 )
            {
            RandomCurrent = ++RandomCurrent % NUMBER_OF_PLAYERS;
            lv_tabview_set_act (Pages, NUMBER_OF_PLAYERS + RandomCurrent, LV_ANIM_OFF);
            RandomTimer = 1000;
            }
        }
    }

//#####################################################################
void PlayerStart (int player, bool state)
    {
    StartState = state;
    CurrentPlayer = player;
    ActivePlayer  = Player[CurrentPlayer];
    ActivePlayer->Start ();
    }

lv_palette_t Colors[NUMBER_OF_PLAYERS] = { LV_PALETTE_BLUE, LV_PALETTE_RED };
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

//    /*Initialize the (dummy) input device driver*/
//    lv_indev_t *indev = lv_indev_create();
//    lv_indev_set_type    (indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
//    lv_indev_set_read_cb (indev, my_touchpad_read);

    Pages = lv_tabview_create (lv_scr_act ());
    lv_tabview_set_tab_bar_size (Pages, 0);

    // Create players pages
    for ( int z = 0;  z < NUMBER_OF_PLAYERS;  z++)
        {
        Player[z] = new PLAYER_C(z,  Pages, lv_palette_main(Colors[z]));
        }
    // Create coin toss pages
    static lv_style_t styles[NUMBER_OF_PLAYERS];
    for ( int z = 0;  z < NUMBER_OF_PLAYERS;  z++)
        {
        RandomPage[z] = lv_tabview_add_tab (Pages, "");
        lv_style_init (&styles[z]);
        lv_style_set_bg_color (&styles[z], lv_palette_main (Colors[z]));
        obj = lv_obj_create (RandomPage[z]);
        lv_obj_add_style (obj, &styles[z], 0);
        lv_obj_center (obj);
        }
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
    ActivePlayer = nullptr;
    }

//#####################################################################
void loop()
    {
    static uint64_t strt = 0;       // Starting time for next frame delta calculation
    static int      lastKnobKey = 1;
    uint64_t        runtime;

    runtime = micros ();
    DeltaClock = (int)(runtime - strt);
    strt = runtime;

    lv_timer_handler ();

    KNOBBY knob = Knobby.ScanLoop ();
    if ( knob != KNOBBY::Null )
        ActivePlayer = nullptr;

    if ( ActivePlayer == nullptr )
        randomizeLoop ();
    else if ( !StartState )
        ActivePlayer->Loop ();

    int z = digitalRead (KNOB_KEY);
    if ( (z != lastKnobKey) & z )          // Button punched -- new player
        {
        if ( StartState )
            StartState = false;
        else if ( ActivePlayer == nullptr )
            {
            RandomFirst = true;
            PlayerStart (random (1, 200000) % NUMBER_OF_PLAYERS, true);
            }
        else
            {
            CurrentPlayer = ++CurrentPlayer % NUMBER_OF_PLAYERS;
            PlayerStart (CurrentPlayer, false);
            }
        lv_timer_handler ();
        delay (500);
        }
    lastKnobKey = z;
    delay (100);
    }
