/*
Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once


/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
/* Locking resynchronize hack */
#define MASTER_LEFT
#define SPLIT_USB_DETECT
#define POINTING_DEVICE_RIGHT
#define SPLIT_POINTING_ENABLE
#define ADNS5050_SCLK_PIN F7
#define ADNS5050_SDIO_PIN F6
#define ADNS5050_CS_PIN B1
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 3
/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
// #define NO_DEBUG

/* disable print */
// #define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
// タッピング判定時間（ミリ秒）
// この時間内にキーを離せばタップ、それ以上押し続けるとホールド（modifier）として認識
// #define TAPPING_TERM 200

// Mod-Tapキーを押している間に別のキーを押して離した場合、
// TAPPING_TERMを待たずに即座にmodifierとして認識する
// コメントアウトすると無効化
// #define PERMISSIVE_HOLD