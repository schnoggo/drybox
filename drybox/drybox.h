/**
 * Constants and macros for the drybox
 * @package drybox
 * @author Lon Koenig <lon@lonk.me>
 * @license https://opensource.org/licenses/MIT
 */

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

// code compile-time options:


// other animation Constants:
#define FADE_LENGTH 5

// #define PRDBG
// macros:
// create Serial.print macros:
#ifdef PRDBG
#define dprint(ccc) Serial.print(ccc);
#define dprintln(ccc) Serial.println(ccc);
#else
#define dprint(ccc) ""
#define dprintln(ccc) ""
#endif
