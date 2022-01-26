#include QMK_KEYBOARD_H
#include "keymap.h"

#define RAPID_A_CD 300
#define TAPPING_TERM 200

#define PRESS_ELSE(state, tap, press) if (state->pressed) return tap; else return press;

void alt_finished(qk_tap_dance_state_t *state, void *user_data);
void alt_reset(qk_tap_dance_state_t *state, void *user_data);

extern keymap_config_t keymap_config;

typedef struct {
    uint16_t timer;
    bool is_pressed;
} key_timer;

enum dance_actions {
    TD_ALT
};

enum custom_keycodes {
    R_A = SAFE_RANGE,
    LCTRL,
    RCTRL
};

enum taps {
    UNDEF,
    SINGLE_TAP,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    TRIPLE_TAP,
    TRIPLE_HOLD,
};

enum layers_idx {
    _BLQ, // Base Layer
    _FL, // Functions
    _CL, // Colemak mod-dh
};

static key_timer rapid_a_state = {
    .timer = 0,
    .is_pressed = false
};
static uint16_t timer_lctrl;
static uint16_t timer_rctrl;
static enum taps taps_alt = UNDEF;

qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_ALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, alt_finished, alt_reset)
};

// RCMD_T(KC_RBRC)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BLQ] = LAYOUT_ilayraz(
     KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSLS, KC_GRV,
     KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSPC, KC_DEL,
     KC_LCTRL, R_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_PGUP,
     KC_LSPO, KC_BSLS, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSPC, KC_UP, KC_PGDN,
     LCTRL, LCMD_T(KC_LBRC), TD(TD_ALT), KC_SPC, RCTRL, KC_RCMD, KC_LEFT, KC_DOWN, KC_RIGHT),

    [_FL] = LAYOUT_ilayraz(
     KC_GRV, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_MSTP, KC_MPLY,
     KC_TRNS, KC_TRNS, KC_UP, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_INS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MNXT,
     KC_CAPS, KC_TRNS, KC_TRNS, UC(0x2014), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_VOLU,
     KC_TRNS, KC_LCTRL, KC_TRNS, KC_TRNS, TO(_CL), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_VOLD,
     KC_TRNS, KC_TRNS, TG(_FL), KC_ENT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MUTE),

    [_CL] = LAYOUT_ilayraz(
     KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSLS, KC_GRV,
     KC_TAB, KC_Q, KC_W, KC_F, KC_P, KC_B, KC_J, KC_L, KC_U, KC_Y, KC_SCLN, KC_LBRC, KC_RBRC, KC_BSPC, KC_DEL,
     KC_LCTRL, KC_A, KC_R, KC_S, KC_T, KC_G, KC_M, KC_N, KC_E, KC_I, KC_O, KC_QUOT, KC_ENT, KC_PGUP,
     KC_LSPO, KC_Z, KC_X, KC_C, KC_D, KC_V, KC_K, KC_H, KC_COMM, KC_DOT, KC_SLSH, KC_BSLS, KC_RSPC, KC_UP, KC_PGDN,
     KC_BSLS, TO(_BLQ), TD(TD_ALT), KC_SPC, RCTRL, KC_RCMD, KC_LEFT, KC_DOWN, KC_RIGHT),
};


// Called every key press/release
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case R_A:
        rapid_a_state.is_pressed = record->event.pressed;
        rapid_a_state.timer = timer_read();

        if (rapid_a_state.is_pressed) tap_code(KC_A);
        break;
    case LCTRL:
        if (record->event.pressed) {
            timer_lctrl = timer_read();
            register_code(KC_LCTRL);
        } else {
            unregister_code(KC_LCTRL);
            if (timer_elapsed(timer_lctrl) < TAPPING_TERM)
                SEND_STRING("{");
        }
        break;
    case RCTRL:
        if (record->event.pressed) {
            timer_rctrl = timer_read();
            register_code(KC_RCTRL);
        } else {
            unregister_code(KC_RCTRL);
            if (timer_elapsed(timer_rctrl) < TAPPING_TERM)
                SEND_STRING("}");
        }
        break;
    case KC_ESC:
        clear_keyboard();
        break;
    }

    return true;
}

// Called every scan (often)
void matrix_scan_user() {
    if (rapid_a_state.is_pressed) {
        if (timer_elapsed(rapid_a_state.timer) > RAPID_A_CD) {
            tap_code(KC_A);
            rapid_a_state.timer = timer_read();
        }
    }
}

/**
 * TAP DANCE functions
 **/

enum taps cur_dance(qk_tap_dance_state_t *state) {
    switch (state->count) {
    case 1:
        PRESS_ELSE(state, SINGLE_HOLD, SINGLE_TAP);
    case 2:
        PRESS_ELSE(state, DOUBLE_HOLD, DOUBLE_TAP);
    case 3:
        PRESS_ELSE(state, TRIPLE_HOLD, TRIPLE_TAP);
    default:
        return UNDEF;
    }
}

void alt_finished(qk_tap_dance_state_t *state, void *user_data) {
    taps_alt = cur_dance(state);

    switch(taps_alt) {
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
    default: break;
    }
}

void alt_reset (qk_tap_dance_state_t *state, void *user_data) {
    switch (taps_alt) {
    case SINGLE_HOLD: unregister_code(KC_LALT); break;
    case DOUBLE_TAP:
    case DOUBLE_HOLD: layer_off(_FL); break;
    case TRIPLE_HOLD:
        layer_off(_FL);
        unregister_code(KC_LALT);
        break;
    default: break;
    }
}
