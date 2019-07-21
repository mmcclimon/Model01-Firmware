// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "locally built"
#endif

#include "Kaleidoscope.h"
#include "kaleidoscope/hid.h"

// EEProm stuff, including Focus
#include "Kaleidoscope-EEPROM-Settings.h"
#include "Kaleidoscope-EEPROM-Keymap.h"
#include "Kaleidoscope-FocusSerial.h"

// other defaults (better commented in KALEIDOSCOPE_INIT_PLUGINS below)
#include "Kaleidoscope-HardwareTestMode.h"
#include "Kaleidoscope-HostPowerManagement.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-LED-Palette-Theme.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-Macros.h"

// buncha LED effects plugins
#include "Kaleidoscope-LEDEffect-BootGreeting.h"
#include "Kaleidoscope-LED-Stalker.h"
#include "Kaleidoscope-LEDEffect-SolidColor.h"
#include "Kaleidoscope-LEDEffect-Rainbow.h"
#include "Kaleidoscope-LEDEffect-Breathe.h"
#include "Kaleidoscope-Colormap.h"

// random other stuff
#include "Kaleidoscope-Qukeys.h"
#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope-LED-ActiveModColor.h"

enum macro_names { 
  MACRO_VERSION_INFO,
  MACRO_ANY
};

enum layer_names { 
  PRIMARY, SYMBOL, FUNCTION 
};

// long comment about keymaps removed here, presumably still available in the
// default firmware repo


KEYMAPS(
  // QWERTY layer
  [PRIMARY] = KEYMAP_STACKED
  (___             , Key_1         , Key_2                , Key_3       , Key_4 , Key_5 , Key_LEDEffectNext ,
   Key_Tab         , Key_Q         , Key_W                , Key_E       , Key_R , Key_T , Key_Backtick      ,
   CTL_T(Escape)   , Key_A         , Key_S                , Key_D       , Key_F , Key_G ,
   Key_LeftShift   , Key_Z         , Key_X                , Key_C       , Key_V , Key_B , Key_Tab           ,
   Key_LeftControl , Key_Backspace , OSL(SYMBOL)          , OSM(LeftGui),
   ShiftToLayer(FUNCTION),

   M(MACRO_ANY) , Key_6     , Key_7        , Key_8            , Key_9      , Key_0         , Key_Minus      ,
   Key_Enter    , Key_Y     , Key_U        , Key_I            , Key_O      , Key_P         , Key_Backslash  ,
                  Key_H     , Key_J        , Key_K            , Key_L      , Key_Semicolon , Key_Quote      ,
   Key_Equals   , Key_N     , Key_M        , Key_Comma        , Key_Period , Key_Slash     , Key_RightShift ,
   OSM(RightAlt), Key_Enter , Key_Spacebar , Key_RightControl ,
   ShiftToLayer(FUNCTION)),


  // SYMBOL layer
  [SYMBOL] =  KEYMAP_STACKED
  (___ , Key_F1               , Key_F2            , Key_F3                , Key_F4             , Key_F5               , ___ ,
   ___ , LSHIFT(Key_6)        , LSHIFT(Key_7)     , LSHIFT(Key_8)         , Key_Pipe           , LSHIFT(Key_Backtick) , ___ ,
   ___ , LSHIFT(Key_1)        , LSHIFT(Key_2)     , LSHIFT(Key_3)         , LSHIFT(Key_4)      , LSHIFT(Key_5)        ,
   ___ , ___                  , ___               , ___                   , ___                , ___                  , ___ ,
   ___ , ___ , ___ , ___ ,
   ___ ,

   ___ , Key_F6               , Key_F7            , Key_F8                , Key_F9             , Key_F10              , Key_F11 ,
   ___ , Key_LeftParen        , Key_Equals        , Key_RightParen        , LSHIFT(Key_8)      , LSHIFT(Key_Equals)   , Key_F12 ,
         Key_LeftCurlyBracket , Key_Minus         , Key_RightCurlyBracket , LSHIFT(Key_Period) , LSHIFT(Key_Comma)    , ___     ,
   ___ , Key_LeftBracket      , LSHIFT(Key_Minus) , Key_RightBracket      , ___                , ___                  , ___     ,
   ___ , ___ , ___ , ___ ,
   ___),

  // FUNCTION layer
  [FUNCTION] =  KEYMAP_STACKED
  (___,      Key_F1,           Key_F2,      Key_F3,     Key_F4,        Key_F5,           Key_CapsLock,
   Key_Tab,  ___,              Key_mouseUp, ___,        Key_mouseBtnR, Key_mouseWarpEnd, Key_mouseWarpNE,
   Key_Home, Key_mouseL,       Key_mouseDn, Key_mouseR, Key_mouseBtnL, Key_mouseWarpNW,
   Key_End,  Key_PrintScreen,  Key_Insert,  ___,        Key_mouseBtnM, Key_mouseWarpSW,  Key_mouseWarpSE,
   ___, Key_Delete, ___, ___,
   ___,

   Consumer_ScanPreviousTrack, Key_F6,                 Key_F7,                   Key_F8,                   Key_F9,          Key_F10,          Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack, Key_LeftCurlyBracket,     Key_RightCurlyBracket,    Key_LeftBracket, Key_RightBracket, Key_F12,
                               Key_LeftArrow,          Key_DownArrow,            Key_UpArrow,              Key_RightArrow,  ___,              ___,
   Key_PcApplication,          Consumer_Mute,          Consumer_VolumeDecrement, Consumer_VolumeIncrement, ___,             Key_Backslash,    Key_Pipe,
   ___, ___, Key_Enter, ___,
   ___)
) // KEYMAPS


// Macros
// ------

// type out some build info
static void versionInfoMacro(uint8_t keyState) {
  if (keyToggledOn(keyState)) {
    Macros.type(PSTR("Keyboardio Model 01 - Kaleidoscope "));
    Macros.type(PSTR(BUILD_INFORMATION));
  }
}

// type a random alphanumeric character
static void anyKeyMacro(uint8_t keyState) {
  static Key lastKey;
  bool toggledOn = false;
  if (keyToggledOn(keyState)) {
    lastKey.keyCode = Key_A.keyCode + (uint8_t)(millis() % 36);
    toggledOn = true;
  }

  if (keyIsPressed(keyState))
    kaleidoscope::hid::pressKey(lastKey, toggledOn);
}

// Macro handler; probably don't mess with this.
const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch (macroIndex) {

  case MACRO_VERSION_INFO:
    versionInfoMacro(keyState);
    break;

  case MACRO_ANY:
    anyKeyMacro(keyState);
    break;
  }
  return MACRO_NONE;
}



// define some solid color led modes
static kaleidoscope::plugin::LEDSolidColor solidRed(160, 0, 0);
static kaleidoscope::plugin::LEDSolidColor solidOrange(140, 70, 0);
static kaleidoscope::plugin::LEDSolidColor solidYellow(130, 100, 0);
static kaleidoscope::plugin::LEDSolidColor solidGreen(0, 160, 0);
static kaleidoscope::plugin::LEDSolidColor solidBlue(0, 70, 130);
static kaleidoscope::plugin::LEDSolidColor solidIndigo(0, 0, 170);
static kaleidoscope::plugin::LEDSolidColor solidViolet(130, 0, 120);

// toggle LEDs off when the host goes to sleep,and back on when it wakes up.
void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::plugin::HostPowerManagement::Suspend:
    LEDControl.set_all_leds_to({0, 0, 0});
    LEDControl.syncLeds();
    LEDControl.paused = true;
    break;
  case kaleidoscope::plugin::HostPowerManagement::Resume:
    LEDControl.paused = false;
    LEDControl.refreshAll();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Sleep:
    break;
  }
}

// let's go, yo. (Order matters here, esp. for LED effects.)
KALEIDOSCOPE_INIT_PLUGINS(
  // Stuff for EEPROM, which is how Chrysalis works.
  EEPROMSettings,
  EEPROMKeymap,
  Focus,
  FocusSettingsCommand,
  FocusEEPROMCommand,

  // Other defaults
  HardwareTestMode,         // tap prog/led/left-fn
  LEDControl,               // support for al other LED stuff
  LEDPaletteTheme,          // shared palette for plugins (like Colormap)
  HostPowerManagement,      // turn off LEDs on sleep
  Macros,                   // macro support
  MouseKeys,                // move the mouse with the keyboard

  // LED effects
  BootGreetingEffect,       // pulse LED button for 10s on connect
  LEDOff,                   // turn 'em off
  StalkerEffect,            // light up recent keys
  LEDRainbowWaveEffect,     // it's a rainbow wave.
  solidRed,                 // solid red (duh. no-comments looked weird though)
  solidOrange,              // solid orange
  solidYellow,              // solid yellow
  solidGreen,               // solid green
  solidBlue,                // solid blue
  solidIndigo,              // solid indigo
  solidViolet,              // solid violet
  ColormapEffect,           // set up per-layer color maps
  ActiveModColorEffect,     // show active mods (useful for one-shot)

  // other custom stuff
  Qukeys,                   // dual-use keys
  OneShot                   // don't hold down mods
);

void setup() {
  Kaleidoscope.setup();

  LEDRainbowWaveEffect.brightness(125);   // bump brightness; looks better

  // Stalker will be mostly blue, but with yellow highlights
  StalkerEffect.inactive_color = CRGB(0, 0, 130);
  StalkerEffect.variant = STALKER(Haunt, (CRGB(130, 130, 0)));
  StalkerEffect.step_length = 30;

  ActiveModColorEffect.highlight_color = CRGB(200, 0, 200);

  // Use 5 layers for EEProm and Colormap
  EEPROMKeymap.setup(5);
  ColormapEffect.max_layers(5);

  LEDOff.activate();    // start with LEDs off.
}

void loop() {
  Kaleidoscope.loop();
}
