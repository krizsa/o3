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
#ifndef O3_C_WINDOW1_APPLE_H
#define O3_C_WINDOW1_APPLE_H

#include <Cocoa/Cocoa.h>
#include <ApplicationServices/ApplicationServices.h>

namespace o3 {

struct cWindow : cWindowBase {
    static o3_ext("cO3") o3_get siScr window(iCtx* ctx)
    {
        static Var window = ctx->value("window");
        
        if (window.type() == Var::TYPE_VOID)
            window = ctx->setValue("window", o3_new(cWindow)());
        return window.toScr();
    }
    
    NSWindow* m_window;
    CGPoint m_position;
    
    cWindow()
    {
        m_window = [[NSApplication sharedApplication] mainWindow];
        m_position.x = 0;
        m_position.y = 0;
    }
    
    o3_begin_class(cWindowBase)
    o3_end_class()
    
    o3_glue_gen()
    
    int x()
    {
        o3_trace3 trace;

        return m_window.frame.origin.x;
    }
    
    int setX(int x)
    {
        o3_trace3 trace;
        NSRect frame = m_window.frame;
        
        frame.origin.x = x;
        [m_window setFrame:frame display:NO];
        return x;
    }
    
    int y()
    {
        o3_trace3 trace;

        return m_window.frame.origin.y;
    }
    
    int setY(int y)
    {
        o3_trace3 trace;
        NSRect frame = m_window.frame;
        
        frame.origin.y = y;
        [m_window setFrame:frame display:NO];
        return y;
    }
    
    int width()
    {
        o3_trace3 trace;

        return m_window.frame.size.width;
    }
    
    int setWidth(int width)
    {
        o3_trace3 trace;
        NSRect frame = m_window.frame;
        
        frame.size.width = width;
        [m_window setFrame:frame display:YES];
        return width;
    }
    
    int height()
    {
        o3_trace3 trace;

        return m_window.frame.size.height;
    }
    
    int setHeight(int height)
    {
        o3_trace3 trace;
        NSRect frame = m_window.frame;
        
        frame.size.height = height;
        [m_window setFrame:frame display:YES];
        return height;
    }
    
    void sendMouseMove(int x, int y)
    {
        NSRect frame;
        CGFloat top, left;
        CGEventRef event;
        
        frame = [m_window frame];
        left = frame.origin.x;
        top = frame.origin.y + frame.size.height;
        frame = [[NSScreen mainScreen] frame];
        m_position.x = left + x;
        m_position.y = frame.size.height - top + y;
        event = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, m_position, 0);
        CGEventPost(kCGHIDEventTap, event);
    }
    
    /*
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
    */
};

}

#endif // O3_C_WINDOW1_APPLE_H
