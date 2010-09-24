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
#ifndef O3_C_MOUSE1_APPLE_H
#define O3_C_MOUSE1_APPLE_H

#include <ApplicationServices/ApplicationServices.h>

namespace o3 {

struct cMouse : cMouseBase {
    CGPoint m_position;
        
    static o3_ext("cO3") o3_get siScr mouse(iCtx* ctx)
    {
        static Var mouse = ctx->value("mouse");
        
        if (mouse.type() == Var::TYPE_VOID)
            mouse = ctx->setValue("mouse", o3_new(cMouse)());
        return mouse.toScr();
    }
    
    o3_begin_class(cMouseBase)
    o3_end_class()
    
    o3_glue_gen()
    
    void sendMouseMoved(int x, int y)
    {
        CGEventRef event;
        
        m_position.x = x;
        m_position.y = y;
        event = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, m_position,
                0);
        CGEventPost(kCGHIDEventTap, event);
    }
    
    void sendLeftMouseDown()
    {
        CGEventRef event;
        
        event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, m_position,
                0);
        CGEventPost(kCGHIDEventTap, event);
    }
    
    void sendLeftMouseUp()
    {
        CGEventRef event;
        
        event = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, m_position,
                0);
        CGEventPost(kCGHIDEventTap, event);
    }
    
    void sendRightMouseDown()
    {
        CGEventRef event;
        
        event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown,
                m_position, 0);
    }
    
    void sendRightMouseUp()
    {
        CGEventRef event;
        
        event = CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, m_position,
                0);
        CGEventPost(kCGHIDEventTap, event);
    }

    virtual o3_fun void sendMouseScrolled(int dy)
    {
        CGEventRef event;
        
        event = CGEventCreateScrollWheelEvent (NULL, kCGScrollEventUnitPixel, 1,
                dy);
        CGEventPost(kCGHIDEventTap, event);
    }
};

} // namespace o3

#endif // O3_C_MOUSE1_APPLE_H
