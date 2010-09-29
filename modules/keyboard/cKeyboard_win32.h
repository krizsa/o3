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
#ifndef O3_C_KEYBOARD1_WIN32_H
#define O3_C_KEYBOARD1_WIN32_H

namespace o3 {

#include <winuser.h>

struct cKeyboard : cKeyboardBase {
	o3_begin_class(cKeyboard)
	o3_end_class()

	o3_glue_gen()

	void sendKeyDown(int key)
	{
		int win_keycode = mapJsKeyCodes(key);
		if (win_keycode > 0)
			keyDown((BYTE)win_keycode);
	}

	void sendKeyUp(int key)
	{
		int win_keycode = mapJsKeyCodes(key);
		if (win_keycode > 0)
			keyUp((BYTE)win_keycode);
	}

	static void keyDown(BYTE VKey) 
	{
		BYTE ScanCode = 0;
		ScanCode = LOBYTE(::MapVirtualKey(VKey, 0));

		::keybd_event(VKey, ScanCode, 0, 0);
	}

	static void keyUp(BYTE VKey) 
	{
		BYTE ScanCode = LOBYTE(::MapVirtualKey(VKey, 0));
		::keybd_event(VKey, ScanCode, KEYEVENTF_KEYUP, 0);
	}

	static int mapJsKeyCodes(int js_code) {
		switch(js_code){
			case 8: return VK_BACK;		//backspace  	
			case 9: return VK_TAB;  	//tab 	
			case 13: return VK_RETURN;	//enter 	13
			case 16: return VK_SHIFT;	//shift 	16
			case 17: return VK_CONTROL;	//ctrl 	17
			case 18: return VK_MENU;	//alt 	18
			case 19: return VK_PAUSE;	//pause/break 	19
			case 20: return VK_CAPITAL; //caps lock 	20
			case 27: return VK_ESCAPE;  //escape 	27
			case 33: return VK_PRIOR;	//page up 	33
			case 34: return VK_NEXT;	//page down 	34
			case 35: return	VK_END;		//end 	35
			case 36: return VK_HOME;	//home 	36
			case 37: return VK_LEFT;	//left arrow 	37
			case 38: return VK_UP;		//up arrow 	38
			case 39: return VK_RIGHT;	//right arrow 	39
			case 40: return VK_DOWN;	//down arrow 	40
			case 45: return	VK_INSERT;	//insert 	45
			case 46: return VK_DELETE;	//delete 	46
			case 144: return VK_NUMLOCK;//num lock 	144
			case 145: return VK_SCROLL;//scroll lock 	145
			default: 
				//numpad 0 	96	
				//numpad 9 	105
				if (js_code >= 96 && js_code <= 105) {
					return VK_NUMPAD0 + js_code - 96;
				}						
				//f1 	112
				//f12 	123
				if (js_code >= 112 && js_code <= 123) {
					return VK_F1 + js_code - 112;
				}						
				//0 	48
				//9 	57
				if (js_code >= 48 && js_code <= 57) {
					return 30 + js_code - 48;
				}						
				//a 	65
				//z 	90
				if (js_code >= 65 && js_code <= 90) {
					return 41 + js_code - 65;
				}

				// key not supported:
				return -1;
		}

	}
};

} // namespace o3

#endif // O3_C_KEYBOARD1_WIN32_H
