// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include QMK_KEYBOARD_H

enum custom_keycodes {
    DRAG_SCROLL = SAFE_RANGE,
    KC_GESTURE,
};

// トラックボール関連の関数宣言
bool process_record_trackball(uint16_t keycode, keyrecord_t *record);
layer_state_t layer_state_set_trackball(layer_state_t state);

// グローバル変数の宣言
extern bool set_scrolling;
extern bool drag_scroll_pressed;
extern float scroll_accumulated_h;
extern float scroll_accumulated_v;

// ジェスチャー機能用の変数
extern bool gesture_mode;
extern float gesture_accumulated_x;