/*
 * Copyright (C) 2010 Ajax.org BV
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef O3_C_KEYBOARD1_APPLE_H
#define O3_C_KEYBOARD1_APPLE_H

#include <ApplicationServices/ApplicationServices.h>

namespace o3 {

struct cKeyboard1 : cKeyboard1Base {
    static o3_ext("cO3") o3_get siScr keyboard(iCtx* ctx)
    {
        static Var keyboard = ctx->value("keyboard");
        
        if (keyboard.type() == Var::TYPE_VOID)
            keyboard = ctx->setValue("keyboard", o3_new(cKeyboard1)());
        return keyboard.toScr();
    }
    
    o3_begin_class(cKeyboard1Base)
    o3_end_class()
    
    o3_glue_gen()
    
    int keyCode(int key) {
        static int key_codes[] = {
        //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            -1, -1, -1, -1, -1, -1, -1, -1, 51, 48, -1, -1, -1, 36, -1, -1, // 0
            56, 59, 58, 113,57, -1, -1, -1, -1, -1, -1, 53, -1, -1, -1, -1, // 1
            49, 116,121,119,115,123,126,125,124,-1, -1, -1, -1, 105,117,-1, // 2
            29, 18, 19, 20, 21, 23, 22, 26, 28, 25, -1, 41, -1, 24, -1, -1, // 3
            -1, 0,  11, 8,  2,  14, 3,  5,  4,  34, 38, 40, 37, 46, 45, 31, // 4
            35, 12, 15, 1,  17, 32, 9,  13, 7,  16, 6,  55, -1, -1, -1, -1, // 5
            82, 83, 84, 85, 86, 87, 88, 89, 91, 92, 67, 69, -1, 78, 65, 75, // 6
            122,120,99, 118,96, 97, 98, 100,101,109,103,111,124,-1, -1, -1, // 7
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 8
            71, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 9
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 10
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 41 ,-1, 43, -1, 47, 44, // 11
            50, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 12
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 33, 42, 30, 39, -1, // 13
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 14
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 15
        };

        return key_codes[key];
    }
    
    void sendKeyDown(int key)
    {
        CGEventRef event;
        
        event = CGEventCreateKeyboardEvent(NULL, keyCode(key), true);
        CGEventPost(kCGHIDEventTap, event);
    }
    
    void sendKeyUp(int key)
    {
        CGEventRef event;
        
        event = CGEventCreateKeyboardEvent(NULL, keyCode(key), false);
        CGEventPost(kCGHIDEventTap, event);
    }
};

} // namespace o3

#endif // O3_C_KEYBOARD1_APPLE_H
