#include QMK_KEYBOARD_H
#include "keymap.h"

#define TAPPING_TERM 200
#define RAPID_A_CD 300

extern keymap_config_t keymap_config;

int cur_dance(qk_tap_dance_state_t *state);
void alt_finished(qk_tap_dance_state_t *state, void *user_data);
void alt_reset(qk_tap_dance_state_t *state, void *user_data);

typedef struct {
    int state;
    bool is_press_action;
} tap;

typedef struct {
    uint16_t timer;
    bool is_pressed;
} key_timer;

static tap alttap_state = {
    .is_press_action = true,
    .state = 0
};

static key_timer rapid_a_state = {
    .timer = 0,
    .is_pressed = false
};

enum taps {
    SINGLE_TAP = 1,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    TRIPLE_TAP,
    TRIPLE_HOLD,
};

enum dance_actions {
    TD_ALT,
};

enum custom_keycodes {
    R_A = SAFE_RANGE,
};

enum layers_idx {
    _BLQ, // Base Layer
    _FL, // Functions
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BLQ] = LAYOUT_ilayraz(
     KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSLS, KC_GRV,
     KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSPC, KC_DEL,
     KC_LCTRL, R_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_PGUP,
     KC_LSPO, KC_BSLS, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSPC, KC_UP, KC_PGDN,
     KC_LCTL, KC_LGUI, TD(TD_ALT), KC_SPC, KC_RCTL, KC_RGUI, KC_LEFT, KC_DOWN, KC_RIGHT),

    [_FL] = LAYOUT_ilayraz(
     KC_GRV, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_MSTP, KC_MPLY,
     KC_TRNS, KC_TRNS, KC_UP, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_INS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MNXT,
     KC_CAPS, KC_LEFT, KC_DOWN, KC_RIGHT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_VOLU,
     KC_TRNS, KC_LCTRL, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_VOLD,
     KC_TRNS, KC_TRNS, TG(_FL), KC_ENT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MUTE)
};

qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_ALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, alt_finished, alt_reset)
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case R_A:
        rapid_a_state.is_pressed = record->event.pressed;
        rapid_a_state.timer = timer_read();

        if (rapid_a_state.is_pressed) tap_code(KC_A);
        break;
    }

    return true;
}

void matrix_scan_user() {
    if (rapid_a_state.is_pressed) {
        if (timer_elapsed(rapid_a_state.timer) > RAPID_A_CD) {
            tap_code(KC_A);
            rapid_a_state.timer = timer_read();
        }
    }
}

int cur_dance(qk_tap_dance_state_t *state) {
    switch (state->count) {
    case 1:
        if (state->pressed) return SINGLE_HOLD;
        else return SINGLE_TAP;
        break;
    case 2:
        if (state->pressed) return DOUBLE_HOLD;
        else return DOUBLE_TAP;
        break;
    case 3:
        if (state->pressed) return TRIPLE_HOLD;
        else return TRIPLE_TAP;
        break;
    }
    return -1;
}

void alt_finished(qk_tap_dance_state_t *state, void *user_data) {
    alttap_state.state = cur_dance(state);
    switch(alttap_state.state) {
    case SINGLE_TAP:
        set_oneshot_layer(_FL, ONESHOT_START);
        clear_oneshot_layer_state(ONESHOT_PRESSED);
        break;
    case SINGLE_HOLD: register_code(KC_LALT); break;
    case DOUBLE_TAP:
    case DOUBLE_HOLD: layer_on(_FL); break;
    case TRIPLE_TAP:
        set_oneshot_mods(MOD_LALT);
        set_oneshot_layer(_FL, ONESHOT_START);
        clear_oneshot_layer_state(ONESHOT_PRESSED);
        break;
    case TRIPLE_HOLD:
        register_code(KC_LALT);
        layer_on(_FL);
        break;
    }
}

void alt_reset (qk_tap_dance_state_t *state, void *user_data) {
    switch (alttap_state.state) {
    case SINGLE_HOLD: unregister_code(KC_LALT); break;
    case DOUBLE_TAP:
    case DOUBLE_HOLD: layer_off(_FL); break;
    case TRIPLE_HOLD:
        layer_off(_FL);
        unregister_code(KC_LALT);
        break;
    }
}
