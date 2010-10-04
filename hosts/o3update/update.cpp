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

#define O3_UPDATER

#include "o3.h"
#include "fs/fs.h"
#include "http/http.h"
#include "md5/md5.h"
#include "blob/blob.h"

namespace o3 {

	volatile int32_t g_outerComponents = 0;

	int incWrapperCount() {
		return atomicInc(g_outerComponents);
	}

	int decWrapperCount() {
		return atomicDec(g_outerComponents);
	} 

	struct cCtx : cUnk, iCtx, iCtx1 {

		cCtx(iMgr* mgr) 
			: m_mgr(mgr)
			, m_track(0)
			, m_loop(g_sys->createMessageLoop())
		{        
		}

		virtual ~cCtx()
		{        
		}

		o3_begin_class(cUnk)
			o3_add_iface(iAlloc)
			o3_add_iface(iCtx)
			o3_add_iface(iCtx1)
			o3_end_class()

		siMgr                       m_mgr;
		ComTrack*                   m_track;
		siMessageLoop               m_loop;
		tMap<Str, Var>              m_values;
		HANDLE                      m_app_window;

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

		virtual ComTrack** track() 
		{
			return &m_track;
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

		virtual void tear() 
		{
			for (ComTrack *i = m_track, *j = 0; i; i = j) {
				j = i->m_next;
				i->m_phead = 0;
				i->tear();
			}
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

	bool validate(const Buf& hashes, const Buf& installer ) 
	{
		if (hashes.size()<32 || installer.size()<16) 
			return false;

		cMD5Hash md;
		Buf ih = md.hash(installer);

		Str hash(hashes);
		cBlob* blob = o3_new(cBlob);
		siScr sblob = blob;
		Buf hh = blob->fromHex(hash);

		return !memCompare(hh.ptr(), ih.ptr(), 16);
	}


	void touch( iFs* installer ) 
	{
		if (installer)
			installer->setModifiedTime(installer->modifiedTime() + 1);
	}

	struct Updater : cUnk
	{
		siCtx m_ctx;
		siMgr m_mgr;

		void start()
		{
			cSys sys;

			m_mgr = o3_new(cMgr)();
			m_ctx = o3_new(cCtx)(m_mgr);	

			m_mgr->addFactory("http", &cHttp::factory);

			siThread t = g_sys->createThread(Delegate(this,&Updater::update));


			for(;;) {
				m_ctx->loop()->wait(10);
			}

		}


		void update(iUnk*) 
		{
			Str inst_path = installDirPath();
			inst_path.findAndReplaceAll("\\", "/");
			siFs fs = cFs::fs(m_ctx);
			if (!fs)
				return;

			siFs uninstaller = fs->get(inst_path + "/" + O3_PLUGIN_INSTALLER);
			if (!uninstaller)
				return;

			Buf hashes = m_mgr->downloadHashes(m_ctx);

			if (validate(hashes, uninstaller->blob()))
				return;

			Str tmp_path = tmpPath();
			tmp_path.findAndReplaceAll("\\", "/");

			siFs tmp = cFs::fs(m_ctx)->get(tmp_path);
			if (!tmp)
				return;

			siFs installer = tmp->get(O3_PLUGIN_INSTALLER);

			if (installer->exists()) {
				if (validate(hashes, installer->blob())){
					touch(installer);
					return;
				}			
			}

			Buf installer_data = m_mgr->downloadInstaller(m_ctx);
			if (validate(hashes, installer_data))
				installer->setData(installer_data);
		}

	};

}



//int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR arg, int show){
int main(int argc, char **argv) {

    using namespace o3;  

	HANDLE h = CreateMutex(
		NULL,
		TRUE,
		L"Global\o3updater"
	);

	if (!h || GetLastError() == ERROR_ALREADY_EXISTS)
		return 0;

	Updater updater;
	updater.addRef();
	updater.start();

	CloseHandle(h);

    return 0;
}  

