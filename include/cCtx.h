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

namespace o3 {

	struct cCtx : cUnk, iCtx {

		cCtx(iMgr* mgr) 
			: m_mgr(mgr)
			, m_loop(g_sys->createMessageLoop())
		{        
		}

		virtual ~cCtx()
		{        
		}

		o3_begin_class(cUnk)
			o3_add_iface(iAlloc)
			o3_add_iface(iCtx)
			o3_end_class()

		siMgr                       m_mgr;
		siMessageLoop               m_loop;
		tMap<Str, Var>              m_values;
		void*                       m_app_window;

		// iAlloc
		void* alloc(size_t size)
		{
			return g_sys->alloc(size);
		}

		void free(void* ptr)
		{
			return g_sys->free(ptr);
		}

		// iCtx
		siMgr mgr()
		{
			return m_mgr;
		}

		virtual siMessageLoop loop()
		{
			return m_loop;
		}

		virtual Var value(const char* key) 
		{
			return m_values[key];
		}

		virtual Var setValue(const char* key, const Var& val)
		{
			return m_values[key] = val;
		}

		virtual Str fsRoot()
		{
			return Str();
		}

		virtual Var eval(const char* name, siEx* ex = 0)
		{
			return Var();    
		}

		virtual void setAppWindow(void* handle)
		{
			m_app_window = handle;
		}

		virtual void* appWindow() 
		{
			return (void*) m_app_window;
		}

		virtual bool isIE()
		{
			return false;
		}
	}; 
}