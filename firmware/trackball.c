// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "trackball.h"

// マウス感度とスムージングパラメータ
#define MOVEMENT_SENSITIVITY 0.5f
#define SMOOTHING_FACTOR 0.7f
#define SENSITIVITY_MULTIPLIER 1.5f
#define ROTATION_ANGLE_45_DEG 45.0f

// スクロール感度の調整用パラメータ
#define SCROLL_DIVISOR_H 1.0
#define SCROLL_DIVISOR_V 1.0
#define SENSOR_MULTIPLIER 0.02
#define MAX_SCROLL_STEP 1  // スクロールの最大ステップ数を制限

// グローバル変数
bool set_scrolling = false;
bool drag_scroll_pressed = false;
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

// ジェスチャー機能用の変数
bool gesture_mode = false;
float gesture_accumulated_x = 0.0f;
bool gesture_triggered = false;

#define GESTURE_THRESHOLD 50.0f  // ジェスチャー検出の閾値

// マウス移動の累積とスムージング用変数
static float x_accumulator = 0.0;
static float y_accumulator = 0.0;
static float prev_x = 0.0, prev_y = 0.0;

void pointing_device_init_user(void) {
    // D4ピンの状態に応じて初期auto mouse layerを設定
    bool d4_is_high = readPin(D4);
    if (d4_is_high) {
        set_auto_mouse_layer(3);
    } else {
        set_auto_mouse_layer(7);
    }
    set_auto_mouse_enable(true);
}

// マウスレポートを処理し、ドラッグスクロールを実行する関数
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // ジェスチャーモード中にマウス移動を検出（回転変換前の生の値を使用）
    if (gesture_mode && !gesture_triggered) {
        // マウスのX移動量を累積
        gesture_accumulated_x += mouse_report.x;

        // 左方向のジェスチャー検出
        if (gesture_accumulated_x < -GESTURE_THRESHOLD) {
            tap_code16(LCTL(KC_LEFT));  // Ctrl+左矢印を送信
            gesture_triggered = true;
            gesture_accumulated_x = 0.0f;
        }
        // 右方向のジェスチャー検出
        else if (gesture_accumulated_x > GESTURE_THRESHOLD) {
            tap_code16(LCTL(KC_RIGHT));  // Ctrl+右矢印を送信
            gesture_triggered = true;
            gesture_accumulated_x = 0.0f;
        }
    }

    // 45度回転変換 + 上下左右反転 (M_PI/180 ≈ 0.017453)
    double rad = (double)ROTATION_ANGLE_45_DEG * (M_PI / 180);
    float rotated_x = -(mouse_report.x * cos(rad) - mouse_report.y * sin(rad));
    float rotated_y = -(mouse_report.x * sin(rad) + mouse_report.y * cos(rad));

    // スムージングを適用
    float smoothed_x = prev_x * SMOOTHING_FACTOR + rotated_x * (1.0 - SMOOTHING_FACTOR);
    float smoothed_y = prev_y * SMOOTHING_FACTOR + rotated_y * (1.0 - SMOOTHING_FACTOR);
    prev_x = smoothed_x;
    prev_y = smoothed_y;

    // 感度調整
    smoothed_x *= SENSITIVITY_MULTIPLIER;
    smoothed_y *= SENSITIVITY_MULTIPLIER;
    
    if (set_scrolling) {
        // スクロールモード時は水平方向を優先するか垂直方向を優先するかを判定
        if (abs((int)smoothed_x) > abs((int)smoothed_y)) {
            smoothed_y = 0; // 水平スクロール
        } else {
            smoothed_x = 0; // 垂直スクロール
        }

        scroll_accumulated_v += SENSOR_MULTIPLIER * smoothed_y / SCROLL_DIVISOR_V;
        scroll_accumulated_h += SENSOR_MULTIPLIER * smoothed_x / SCROLL_DIVISOR_H;

        int8_t scroll_v;
        if (scroll_accumulated_v >= 0) {
            scroll_v = (int8_t)floorf(scroll_accumulated_v);
        } else {
            scroll_v = (int8_t)ceilf(scroll_accumulated_v);
        }

        int8_t scroll_h;
        if (scroll_accumulated_h >= 0) {
            scroll_h = (int8_t)floorf(scroll_accumulated_h);
        } else {
            scroll_h = (int8_t)ceilf(scroll_accumulated_h);
        }

        mouse_report.v = scroll_v;
        mouse_report.h = scroll_h;

        scroll_accumulated_v -= scroll_v;
        scroll_accumulated_h -= scroll_h;

        mouse_report.x = 0;
        mouse_report.y = 0;
    } else {
        // 通常のマウス移動モード：累積処理で滑らかな動作を実現
        x_accumulator += smoothed_x * MOVEMENT_SENSITIVITY;
        y_accumulator += smoothed_y * MOVEMENT_SENSITIVITY;
        
        if (fabs(x_accumulator) >= 1.0) {
            mouse_report.x = (int8_t)x_accumulator;
            x_accumulator -= mouse_report.x;
        } else {
            mouse_report.x = 0;
        }
        
        if (fabs(y_accumulator) >= 1.0) {
            mouse_report.y = (int8_t)y_accumulator;
            y_accumulator -= mouse_report.y;
        } else {
            mouse_report.y = 0;
        }
    }
    return mouse_report;
}

// キー入力処理
bool process_record_trackball(uint16_t keycode, keyrecord_t *record) {
    if (keycode == DRAG_SCROLL) {
        if (record->event.pressed) {
            drag_scroll_pressed = true;
            set_scrolling = true;
        } else {
            drag_scroll_pressed = false;
            set_scrolling = false;
            // スクロール累積値をリセット
            scroll_accumulated_v = 0;
        }
        return false;
    }
    return true;
}

// レイヤーチェンジ時の処理（trackball固有の処理）
layer_state_t layer_state_set_trackball(layer_state_t state) {
    if (!drag_scroll_pressed && get_highest_layer(state) != AUTO_MOUSE_DEFAULT_LAYER) {
        set_scrolling = false;
        // スクロール累積値をリセット
        scroll_accumulated_v = 0;
    }
    return state;
}