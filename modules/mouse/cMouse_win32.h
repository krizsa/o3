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
#ifndef O3_C_MOUSE1_WIN32_H
#define O3_C_MOUSE1_WIN32_H

namespace o3 {

struct cMouse : cMouseBase {
	o3_begin_class(cMouseBase)
	o3_end_class()

	o3_glue_gen()

	void sendMoveTo(int x, int y) 
	{
		static DWORD cx = GetSystemMetrics(SM_CXSCREEN);
		static DWORD cy = GetSystemMetrics(SM_CYSCREEN);

		double X = 65535 * (double) x / (double) cx;
		double Y = 65535 * (double) y / (double) cy;
		mouse_event( MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE,
			(DWORD) X,(DWORD) Y, 0, 0);
	}

	void sendLeftButtonDown() 
	{
		mouse_event( MOUSEEVENTF_LEFTDOWN,0,0,0,0);
	}

	void sendLeftButtonUp() 
	{
		mouse_event( MOUSEEVENTF_LEFTUP,0,0,0,0);
	}

	void sendRightButtonDown()
	{
		mouse_event( MOUSEEVENTF_RIGHTDOWN,0,0,0,0);
	}

	void sendRightButtonUp()
	{
		mouse_event( MOUSEEVENTF_RIGHTUP,0,0,0,0); 
	}

	void sendMouseScrolled(int i)
	{
		mouse_event( MOUSEEVENTF_WHEEL, 0, 0, WHEEL_DELTA * i, 0);
	}
};

} // namespace o3

#endif // O3_C_MOUSE1_WIN32_H
