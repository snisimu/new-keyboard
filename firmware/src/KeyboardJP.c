/*
 * Copyright 2013-2014 Esrille Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Keyboard.h"

#include <string.h>
#include <xc.h>

static unsigned char const kanaKeys[6][6] =
{
    {KEY_R, KEY_O, KEY_M, KEY_A, KEY_ENTER},
    {KEY_N, KEY_I, KEY_C, KEY_O, KEY_ENTER},
    {KEY_N, KEY_I, KEY_C, KEY_O, KEY_F, KEY_ENTER},
    {KEY_M, KEY_T, KEY_Y, KEY_P, KEY_E, KEY_ENTER},
    {KEY_T, KEY_R, KEY_O, KEY_N, KEY_ENTER},
    {KEY_S, KEY_T, KEY_I, KEY_C, KEY_K, KEY_ENTER},
};

//
// Stickney Next
//
static unsigned char const matrixStickney[8][12][4] =
{
    {{KEY_LEFTSHIFT, KEY_RIGHT_BRACKET}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_LEFTSHIFT, KEY_NON_US_HASH}, {KEY_MINUS}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_LEFTSHIFT, KEY_PERIOD}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_LEFTSHIFT, KEY_COMMA}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_LEFT_BRACKET}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {KEY_LEFTSHIFT, KEY_0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_INTERNATIONAL3}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixStickneyShift[8][12][4] =
{
    {{KEY_LEFTSHIFT, KEY_RIGHT_BRACKET}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_LEFTSHIFT, KEY_NON_US_HASH}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_LEFTSHIFT, KEY_PERIOD}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_LEFTSHIFT, KEY_SLASH}},
    {{0}, {0}, {0}, {KEY_C}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_RIGHT_BRACKET}},
    {{0}, {0}, {KEY_P}, {KEY_EQUAL}, {KEY_QUOTE}, {0}, {0}, {0}, {KEY_SLASH}, {KEY_1}, {KEY_INTERNATIONAL1}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_NON_US_HASH}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

//
// TRON
//
static unsigned char const matrixTron[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_SLASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_R, KEY_A}, {KEY_R, KEY_U}, {KEY_K, KEY_O}, {KEY_H, KEY_A}, {KEY_X, KEY_Y, KEY_O}, {0}, {0}, {KEY_K, KEY_I}, {KEY_N, KEY_O}, {KEY_K, KEY_U}, {KEY_A}, {KEY_R, KEY_E}},
    {{KEY_T, KEY_A}, {KEY_T, KEY_O}, {KEY_K, KEY_A}, {KEY_T, KEY_E}, {KEY_M, KEY_O}, {0}, {0}, {KEY_W, KEY_O}, {KEY_I}, {KEY_U}, {KEY_S, KEY_I}, {KEY_X, KEY_N}},
    {{KEY_M, KEY_A}, {KEY_R, KEY_I}, {KEY_N, KEY_I}, {KEY_S, KEY_A}, {KEY_N, KEY_A}, {0}, {0}, {KEY_S, KEY_U}, {KEY_T, KEY_U}, {KEY_COMMA}, {KEY_PERIOD}, {KEY_X, KEY_T, KEY_U}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixTronLeft[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_LEFTSHIFT, KEY_SLASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_H, KEY_I}, {KEY_S, KEY_O}, {KEY_Z, KEY_SLASH}, {KEY_X, KEY_Y, KEY_A}, {KEY_H, KEY_O}, {0}, {0}, {KEY_G, KEY_I}, {KEY_G, KEY_E}, {KEY_G, KEY_U}, {KEY_A}, {KEY_W, KEY_Y, KEY_I}},
    {{KEY_N, KEY_U}, {KEY_N, KEY_E}, {KEY_X, KEY_Y, KEY_U}, {KEY_Y, KEY_O}, {KEY_H, KEY_U}, {0}, {0}, {KEY_DAKUTEN}, {KEY_D, KEY_I}, {KEY_V, KEY_U}, {KEY_Z, KEY_I}, {KEY_W, KEY_Y, KEY_E}},
    {{KEY_X, KEY_E}, {KEY_X, KEY_O}, {KEY_S, KEY_E}, {KEY_Y, KEY_U}, {KEY_H, KEY_E}, {0}, {0}, {KEY_Z, KEY_U}, {KEY_D, KEY_U}, {KEY_LEFTSHIFT, KEY_COMMA}, {KEY_LEFTSHIFT, KEY_PERIOD}, {KEY_X, KEY_W, KEY_A}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixTronRight[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_B, KEY_I}, {KEY_Z, KEY_O}, {KEY_G, KEY_O}, {KEY_B, KEY_A}, {KEY_B, KEY_O}, {0}, {0}, {KEY_E}, {KEY_K, KEY_E}, {KEY_M, KEY_E}, {KEY_M, KEY_U}, {KEY_R, KEY_O}},
    {{KEY_D, KEY_A}, {KEY_D, KEY_O}, {KEY_G, KEY_A}, {KEY_D, KEY_E}, {KEY_B, KEY_U}, {0}, {0}, {KEY_O}, {KEY_T, KEY_I}, {KEY_MINUS}, {KEY_M, KEY_I}, {KEY_Y, KEY_A}},
    {{KEY_X, KEY_K, KEY_A}, {KEY_X, KEY_K, KEY_E}, {KEY_Z, KEY_E}, {KEY_Z, KEY_A}, {KEY_B, KEY_E}, {0}, {0}, {KEY_W, KEY_A}, {KEY_X, KEY_I}, {KEY_X, KEY_A}, {KEY_HANDAKU}, {KEY_X, KEY_U}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

//
// Nicola
//
static unsigned char const matrixNicola[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_DAKUTEN}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_COMMA}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_PERIOD}, {KEY_K, KEY_A}, {KEY_T, KEY_A}, {KEY_K, KEY_O}, {KEY_S, KEY_A}, {0}, {0}, {KEY_R, KEY_A}, {KEY_T, KEY_I}, {KEY_K, KEY_U}, {KEY_T, KEY_U}, {KEY_COMMA}},
    {{KEY_U}, {KEY_S, KEY_I}, {KEY_T, KEY_E}, {KEY_K, KEY_E}, {KEY_S, KEY_E}, {0}, {0}, {KEY_H, KEY_A}, {KEY_T, KEY_O}, {KEY_K, KEY_I}, {KEY_I}, {KEY_X, KEY_N}},
    {{KEY_PERIOD}, {KEY_H, KEY_I}, {KEY_S, KEY_U}, {KEY_H, KEY_U}, {KEY_H, KEY_E}, {0}, {0}, {KEY_M, KEY_E}, {KEY_S, KEY_O}, {KEY_N, KEY_E}, {KEY_H, KEY_O}, {KEY_Z, KEY_SLASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixNicolaLeft[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_DAKUTEN}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_X, KEY_A}, {KEY_E}, {KEY_R, KEY_I}, {KEY_X, KEY_Y, KEY_A}, {KEY_R, KEY_E}, {0}, {0}, {KEY_P, KEY_A}, {KEY_D, KEY_I}, {KEY_G, KEY_U}, {KEY_D, KEY_U}, {KEY_P, KEY_I}},
    {{KEY_W, KEY_O}, {KEY_A}, {KEY_N, KEY_A}, {KEY_X, KEY_Y, KEY_U}, {KEY_M, KEY_O}, {0}, {0}, {KEY_B, KEY_A}, {KEY_D, KEY_O}, {KEY_G, KEY_I}, {KEY_P, KEY_O}, {KEY_X, KEY_N}},
    {{KEY_X, KEY_U}, {KEY_MINUS}, {KEY_R, KEY_O}, {KEY_Y, KEY_A}, {KEY_X, KEY_I}, {0}, {0}, {KEY_P, KEY_U}, {KEY_Z, KEY_O}, {KEY_P, KEY_E}, {KEY_B, KEY_O}, {KEY_LEFTSHIFT, KEY_SLASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixNicolaRight[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_HANDAKU}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_PERIOD}, {KEY_G, KEY_A}, {KEY_D, KEY_A}, {KEY_G, KEY_O}, {KEY_Z, KEY_A}, {0}, {0}, {KEY_Y, KEY_O}, {KEY_N, KEY_I}, {KEY_R, KEY_U}, {KEY_M, KEY_A}, {KEY_X, KEY_E}},
    {{KEY_V, KEY_U}, {KEY_Z, KEY_I}, {KEY_D, KEY_E}, {KEY_G, KEY_E}, {KEY_Z, KEY_E}, {0}, {0}, {KEY_M, KEY_I}, {KEY_O}, {KEY_N, KEY_O}, {KEY_X, KEY_Y, KEY_O}, {KEY_X, KEY_T, KEY_U}},
    {{KEY_PERIOD}, {KEY_B, KEY_I}, {KEY_Z, KEY_U}, {KEY_B, KEY_U}, {KEY_B, KEY_E}, {0}, {0}, {KEY_N, KEY_U}, {KEY_Y, KEY_U}, {KEY_M, KEY_U}, {KEY_W, KEY_A}, {KEY_X, KEY_O}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

//
// Nicola F
//
static unsigned char const matrixNicolaF[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_DAKUTEN}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_COMMA}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_PERIOD}, {KEY_K, KEY_A}, {KEY_T, KEY_A}, {KEY_K, KEY_O}, {KEY_S, KEY_A}, {0}, {0}, {KEY_R, KEY_A}, {KEY_T, KEY_I}, {KEY_K, KEY_U}, {KEY_T, KEY_U}, {KEY_COMMA}},
    {{KEY_U}, {KEY_S, KEY_I}, {KEY_T, KEY_E}, {KEY_K, KEY_E}, {KEY_S, KEY_E}, {0}, {0}, {KEY_H, KEY_A}, {KEY_T, KEY_O}, {KEY_K, KEY_I}, {KEY_I}, {KEY_X, KEY_N}},
    {{KEY_PERIOD}, {KEY_H, KEY_I}, {KEY_S, KEY_U}, {KEY_H, KEY_U}, {KEY_H, KEY_E}, {0}, {0}, {KEY_M, KEY_E}, {KEY_S, KEY_O}, {KEY_N, KEY_E}, {KEY_H, KEY_O}, {KEY_Z, KEY_SLASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixNicolaFLeft[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_DAKUTEN}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {KEY_LEFTSHIFT, KEY_SLASH}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {KEY_SLASH}, {KEY_LEFTSHIFT, KEY_EQUAL}, {KEY_RIGHT_BRACKET}, {KEY_NON_US_HASH}, {0}, {0}, {KEY_RIGHT_BRACKET}, {KEY_NON_US_HASH}, {0}, {0}, {0}},
    {{KEY_X, KEY_A}, {KEY_E}, {KEY_R, KEY_I}, {KEY_X, KEY_Y, KEY_A}, {KEY_R, KEY_E}, {0}, {0}, {KEY_P, KEY_A}, {KEY_D, KEY_I}, {KEY_G, KEY_U}, {KEY_D, KEY_U}, {KEY_P, KEY_I}},
    {{KEY_W, KEY_O}, {KEY_A}, {KEY_N, KEY_A}, {KEY_X, KEY_Y, KEY_U}, {KEY_M, KEY_O}, {0}, {0}, {KEY_B, KEY_A}, {KEY_D, KEY_O}, {KEY_G, KEY_I}, {KEY_P, KEY_O}, {KEY_X, KEY_N}},
    {{KEY_X, KEY_U}, {KEY_MINUS}, {KEY_R, KEY_O}, {KEY_Y, KEY_A}, {KEY_X, KEY_I}, {0}, {0}, {KEY_P, KEY_U}, {KEY_Z, KEY_O}, {KEY_P, KEY_E}, {KEY_B, KEY_O}, {KEY_LEFTSHIFT, KEY_SLASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixNicolaFRight[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_HANDAKU}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {KEY_LEFTSHIFT, KEY_SLASH}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {KEY_SLASH}, {KEY_LEFTSHIFT, KEY_EQUAL}, {KEY_RIGHT_BRACKET}, {KEY_NON_US_HASH}, {0}, {0}, {KEY_RIGHT_BRACKET}, {KEY_NON_US_HASH}, {0}, {0}, {0}},
    {{KEY_PERIOD}, {KEY_G, KEY_A}, {KEY_D, KEY_A}, {KEY_G, KEY_O}, {KEY_Z, KEY_A}, {0}, {0}, {KEY_Y, KEY_O}, {KEY_N, KEY_I}, {KEY_R, KEY_U}, {KEY_M, KEY_A}, {KEY_X, KEY_E}},
    {{KEY_V, KEY_U}, {KEY_Z, KEY_I}, {KEY_D, KEY_E}, {KEY_G, KEY_E}, {KEY_Z, KEY_E}, {0}, {0}, {KEY_M, KEY_I}, {KEY_O}, {KEY_N, KEY_O}, {KEY_X, KEY_Y, KEY_O}, {KEY_X, KEY_T, KEY_U}},
    {{KEY_PERIOD}, {KEY_B, KEY_I}, {KEY_Z, KEY_U}, {KEY_B, KEY_U}, {KEY_B, KEY_E}, {0}, {0}, {KEY_N, KEY_U}, {KEY_Y, KEY_U}, {KEY_M, KEY_U}, {KEY_W, KEY_A}, {KEY_X, KEY_O}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixNicolaFHandaku[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_Z, KEY_NON_US_HASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {KEY_LEFTSHIFT, KEY_INTERNATIONAL1}, {KEY_Z, KEY_RIGHT_BRACKET}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_PERIOD}, {KEY_K, KEY_A}, {KEY_T, KEY_A}, {KEY_K, KEY_O}, {KEY_S, KEY_A}, {0}, {0}, {KEY_R, KEY_A}, {KEY_T, KEY_I}, {KEY_K, KEY_U}, {KEY_T, KEY_U}, {KEY_COMMA}},
    {{KEY_U}, {KEY_S, KEY_I}, {KEY_T, KEY_E}, {KEY_K, KEY_E}, {KEY_S, KEY_E}, {0}, {0}, {KEY_P, KEY_A}, {KEY_T, KEY_O}, {KEY_K, KEY_I}, {KEY_I}, {KEY_X, KEY_N}},
    {{KEY_PERIOD}, {KEY_P, KEY_I}, {KEY_S, KEY_U}, {KEY_P, KEY_U}, {KEY_P, KEY_E}, {0}, {0}, {KEY_M, KEY_E}, {KEY_S, KEY_O}, {KEY_N, KEY_E}, {KEY_P, KEY_O}, {KEY_Z, KEY_SLASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

//
// M type
//
static unsigned char const matrixMtype[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_Q}, {KEY_L}, {KEY_J}, {KEY_F}, {KEY_C}, {0}, {0}, {KEY_M}, {KEY_Y}, {KEY_R}, {KEY_W}, {KEY_P}},
    {{KEY_E}, {KEY_U}, {KEY_I}, {KEY_A}, {KEY_O}, {0}, {0}, {KEY_K}, {KEY_S}, {KEY_T}, {KEY_N}, {KEY_H}},
    {{KEY_E, KEY_I}, {KEY_X}, {KEY_V}, {KEY_A, KEY_I}, {KEY_O, KEY_U}, {0}, {0}, {KEY_G}, {KEY_Z}, {KEY_D}, {KEY_COMMA}, {KEY_B}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixMtypeLeft[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_Q}, {KEY_L}, {KEY_J}, {KEY_F}, {KEY_C}, {0}, {0}, {KEY_M, KEY_Y}, {KEY_L, KEY_T, KEY_U}, {KEY_R, KEY_Y}, {KEY_X, KEY_N}, {KEY_P, KEY_Y}},
    {{KEY_E}, {KEY_U}, {KEY_I}, {KEY_A}, {KEY_O}, {0}, {0}, {KEY_K, KEY_Y}, {KEY_S, KEY_Y}, {KEY_T, KEY_Y}, {KEY_N, KEY_Y}, {KEY_H, KEY_Y}},
    {{KEY_E, KEY_I}, {KEY_X}, {KEY_V}, {KEY_A, KEY_I}, {KEY_O, KEY_U}, {0}, {0}, {KEY_G, KEY_Y}, {KEY_Z, KEY_Y}, {KEY_D, KEY_Y}, {KEY_PERIOD}, {KEY_B, KEY_Y}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const matrixMtypeRight[8][12][4] =
{
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
    {{KEY_E, KEY_K, KEY_I}, {KEY_U, KEY_K, KEY_U}, {KEY_I, KEY_K, KEY_U}, {KEY_A, KEY_K, KEY_U}, {KEY_O, KEY_K, KEY_U}, {0}, {0}, {KEY_M}, {KEY_Y}, {KEY_R}, {KEY_W}, {KEY_P}},
    {{KEY_E, KEY_X, KEY_N}, {KEY_U, KEY_X, KEY_N}, {KEY_I, KEY_X, KEY_N}, {KEY_A, KEY_X, KEY_N}, {KEY_O, KEY_X, KEY_N}, {0}, {0}, {KEY_K}, {KEY_S}, {KEY_T}, {KEY_N}, {KEY_RIGHTSHIFT, KEY_SLASH}},
    {{KEY_E, KEY_T, KEY_U}, {KEY_U, KEY_T, KEY_U}, {KEY_I, KEY_T, KEY_U}, {KEY_A, KEY_T, KEY_U}, {KEY_O, KEY_T, KEY_U}, {0}, {0}, {KEY_G}, {KEY_Z}, {KEY_RIGHTSHIFT, KEY_COMMA}, {KEY_RIGHTSHIFT, KEY_PERIOD}, {KEY_Z, KEY_SLASH}},
    {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
};

static unsigned char const dakuonFrom[] = { KEY_K, KEY_S, KEY_T, KEY_H };
static unsigned char const dakuonTo[] = { KEY_G, KEY_Z, KEY_D, KEY_B };

static unsigned char mode;

static const unsigned char* sent;
static const unsigned char* last;
static unsigned char lastMod;

void initKeyboardKana(void)
{
    mode = eeprom_read(EEPROM_KANA);
}

unsigned char switchKana(unsigned char* report, unsigned char count)
{
    ++mode;
    if (KANA_MAX < mode)
        mode = 0;
    eeprom_write(EEPROM_KANA, mode);
    const unsigned char* message = kanaKeys[mode];
    for (char i = 0; i < 6 && count < 8; ++i, ++count) {
        if (!kanaKeys[i])
            break;
        report[count] = message[i];
    }
    return count;
}

static char processKana(const unsigned char* current, const unsigned char* processed, unsigned char* report,
                        const unsigned char base[][12][4], const unsigned char left[][12][4], const unsigned char right[][12][4])
{
    unsigned char mod = current[0];
    unsigned char modifiers;
    unsigned char key;
    unsigned char count = 2;
    const unsigned char* a;
    const unsigned char* dakuon;

    modifiers = current[0] & ~MOD_SHIFT;
    report[0] = modifiers;
    for (char i = 2; i < 8 && count < 8; ++i) {
        unsigned char code = current[i];

        key = getKeyNumLock(code);
        if (key) {
            report[count++] = key;
            last = 0;
            lastMod = current[0];
            modifiers = current[0];
            continue;
        }

        unsigned char no_repeat = 12 * 4 <= code && code < 12 * 7  && code % 12 != 5 && code % 12 != 6;
        for (char j = 2; j < 8; ++j) {
            if (no_repeat && code == processed[j]) {
                code = VOID_KEY;
                break;
            }
        }
        if ((mod & MOD_SHIFT) == MOD_SHIFT) {
            if (lastMod & MOD_LEFTSHIFT)
                mod &= ~MOD_LEFTSHIFT;
            else if (processed[0] & MOD_RIGHTSHIFT)
                mod &= ~MOD_RIGHTSHIFT;
        }

        if (mod & MOD_LEFTSHIFT)
            a = left[code / 12][code % 12];
        else if (mod & MOD_RIGHTSHIFT)
            a = right[code / 12][code % 12];
        else if (current[1] & MOD_ALTSHIFT) // for Nicola F only
            a = matrixNicolaFHandaku[code / 12][code % 12];
        else
            a = base[code / 12][code % 12];
        if (!a[0]) {
            key = getKeyBase(code);
            if (key) {
                if (key == KEY_F13)
                    kana_led = 1;
                else if (key == KEY_F14)
                    kana_led = 0;
                report[count++] = key;
                last = 0;
                lastMod = current[0];
                modifiers = current[0];
                if (current[1] & MOD_LEFT_ALTSHIFT)
                    modifiers |= MOD_LEFTSHIFT;
                else if (current[1] & MOD_RIGHT_ALTSHIFT)
                    modifiers |= MOD_RIGHTSHIFT;
            }
            continue;
        }
        if (no_repeat && sent && sent[0]) {
            for (char i = 0; sent[i]; ++i) {
                for (char j = 0; a[j]; ++j) {
                    if (a[j] && sent[i] == a[j]) {
                        sent = 0;
                        return XMIT_BRK;
                    }
                }
            }
        }
        for (char i = 0; a[i] && count < 8; ++i) {
            key = a[i];
            switch (key) {
            case KEY_DAKUTEN:
                if (last && last[0]) {
                    dakuon = memchr(dakuonFrom, last[0], 4);
                    if (dakuon && count <= 5) {
                        report[count++] = KEY_BACKSPACE;
                        report[count++] = dakuonTo[dakuon - dakuonFrom];
                        report[count++] = last[1];
                    }
                }
                break;
            case KEY_HANDAKU:
                if (last && last[0] == KEY_H) {
                    if (count <= 5) {
                        report[count++] = KEY_BACKSPACE;
                        report[count++] = KEY_P;
                        report[count++] = last[1];
                    }
                }
                break;
            case KEY_LEFTSHIFT:
                modifiers |= MOD_LEFTSHIFT;
                break;
            case KEY_RIGHTSHIFT:
                modifiers |= MOD_RIGHTSHIFT;
                break;
            default:
                report[count++] = key;
                break;
            }
        }
        last = a;
        lastMod = current[0];
    }
    sent = (2 < count) ? last : 0;
    report[0] = modifiers;
    return XMIT_NORMAL;
}

char isKanaMode(const unsigned char* current)
{
    return kana_led && !(current[0] & (MOD_ALT | MOD_CONTROL | MOD_GUI)) && mode != KANA_ROMAJI;
}

char processKeysKana(const unsigned char* current, const unsigned char* processed, unsigned char* report)
{
    switch (mode) {
    case KANA_TRON:
        return processKana(current, processed, report, matrixTron, matrixTronLeft, matrixTronRight);
    case KANA_NICOLA:
        return processKana(current, processed, report, matrixNicola, matrixNicolaLeft, matrixNicolaRight);
    case KANA_NICOLA_F:
        return processKana(current, processed, report, matrixNicolaF, matrixNicolaFLeft, matrixNicolaFRight);
    case KANA_MTYPE:
        return processKana(current, processed, report, matrixMtype, matrixMtypeLeft, matrixMtypeRight);
    case KANA_STICKNEY:
        return processKana(current, processed, report, matrixStickney, matrixStickneyShift, matrixStickneyShift);
    default:
        return processKeysBase(current, processed, report);
    }
}

unsigned char controlKanaLED(unsigned char report)
{
    if (kana_led && mode != KANA_ROMAJI)
        report |= 0x02u;
    return report;
}