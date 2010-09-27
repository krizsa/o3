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
#ifndef O3_C_KEYBOARD_BASE_H
#define O3_C_KEYBOARD_BASE_H

namespace o3 {

struct cKeyboardBase : cScr {
    o3_begin_class(cScr)
    o3_end_class()
    
    o3_glue_gen()
    
    virtual o3_fun void sendKeyDown(int key) = 0;
    
    virtual o3_fun void sendKeyUp(int key) = 0;
    
    virtual o3_fun void sendKeyPressed(int key)
    {
        sendKeyDown(key);
        sendKeyUp(key);
    }
};

} // namespace o3

#endif // O3_C_KEYBOARD_BASE_H
