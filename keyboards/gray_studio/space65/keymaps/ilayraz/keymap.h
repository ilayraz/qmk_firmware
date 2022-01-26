#pragma once

#include "quantum.h"

#define LAYOUT_ilayraz( \
  K00, K01, K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D, K0E, \
  K10, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C, K1D, K1E, \
  K20, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B,      K2D, K2E, \
  K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, K3C, K3D, K3E, \
  K40, K41, K42,                K46,           K49,      K4B, K4C, K4D, K4E \
) { \
        { K00, K01,   K02, K03, K04, K05, K06, K07, K08, K09, K0A, K0B, K0C, K0D,   KC_NO, K0E }, \
        { K10, KC_NO, K11, K12, K13, K14, K15, K16, K17, K18, K19, K1A, K1B, K1C,   K1D,   K1E }, \
        { K20, KC_NO, K21, K22, K23, K24, K25, K26, K27, K28, K29, K2A, K2B, K2D,   KC_NO, K2E }, \
        { K30, K31, K32, K33, K34, K35, K36, K37, K38, K39, K3A, K3B, KC_NO, K3C, K3D, K3E }, \
        { K40, K41, KC_NO, K42, KC_NO, KC_NO, KC_NO, K46, KC_NO, KC_NO, K49, K4B, KC_NO, K4C, K4D, K4E} \
}
