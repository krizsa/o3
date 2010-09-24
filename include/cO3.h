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
#ifndef O3_C_O3_H
#define O3_C_O3_H

#include "pub_key.h"
#include "crypto.h"
#include <tools_zip.h>

#ifdef O3_WITH_LIBEVENT
	#include<event.h>    
#endif	

namespace o3 {

o3_cls(cLoadProgress);
struct cLoadProgress : cScr {
	typedef iHttp::ReadyState ReadyState;

	cLoadProgress()
		: m_state(iHttp::READY_STATE_UNINITIALIZED)
		, m_bytes_received(0)
	{
		m_mutex = g_sys->createMutex();
	}

	virtual ~cLoadProgress()
	{

	}
	
	o3_begin_class(cScr)
	o3_end_class()

	o3_glue_gen()

	Str m_file_name;
	size_t m_bytes_received;
	ReadyState m_state;
	siMutex m_mutex;

	o3_enum("ReadyState",
		READY_STATE_UNINITIALIZED,
		READY_STATE_LOADING,
		READY_STATE_LOADED,
		READY_STATE_INTERACTIVE,
		READY_STATE_COMPLETED);

	o3_get size_t bytesReceived()
	{
		o3_trace3 trace;
		Lock lock(m_mutex);

		return m_bytes_received;
	}

	o3_get ReadyState readyState()
	{
		o3_trace3 trace;
		Lock lock(m_mutex);

		return m_state;
	}

	o3_get Str fileName()
	{
		o3_trace3 trace;
		Lock lock(m_mutex);

		return m_file_name;
	}

	void setFileName(const Str& name)
	{
		o3_trace3 trace;
		Lock lock(m_mutex);

		m_file_name = name;
	}

	void setState(ReadyState state)
	{
		o3_trace3 trace;
		Lock lock(m_mutex);

		m_state = state;
	}

	void setBytesReceived(size_t bytes_received)
	{
		o3_trace3 trace;
		Lock lock(m_mutex);

		m_bytes_received = bytes_received;
	}
};

struct cO3 : cScr {
    tVec<Str>   m_args;
    tVec<Str>   m_envs;
	siWeak		m_ctx;	
	tList<Str>  m_to_approve;
	tList<Str>  m_to_load;
	siScr		m_onnotification;
	bool		m_loading;
	scLoadProgress	m_load_progress;

	siEvent		m_change_event;
	siMutex		m_mutex;

	o3_prop siScr	m_onapprove;
	o3_prop siScr	m_ondone;
	o3_prop siScr	m_onprogress;
	o3_prop siScr	m_onfail;


    cO3(iCtx* ctx, int /*argc*/, char** argv, char** envp)
		: m_loading(false)
	{
        if (argv)
            while (*argv)
                m_args.push(*argv++);
        if (envp)
            while (*envp)
                m_envs.push(*envp++);

		m_load_progress = o3_new(cLoadProgress);		
		m_ctx = ctx;
		ctx->setValue("cO3", this);

		m_change_event = g_sys->createEvent();
		m_mutex = g_sys->createMutex();
	}

    ~cO3()
    {
    }

    o3_begin_class(cScr)
    o3_end_class()

	o3_glue_gen()

    o3_get tVec<Str> args()
    {
        return m_args;
    }

    o3_get tVec<Str> envs()
    {
        return m_envs;
    }

	o3_fun void wait(iCtx* ctx, int timeout = -1)
	{
		o3_trace3 trace;
#ifdef O3_WITH_LIBEVENT
	event_dispatch();    
#endif	
#ifndef O3_PLUGIN
		ctx->loop()->wait(timeout);
#endif 	
	}

	o3_fun void exit(int status = 0)
	{
		status;
		o3_trace3 trace;
#ifdef O3_WIN32
		//SendMessage((HWND) siCtx(m_ctx)->appWindow(), WM_DESTROY, 0 , 0 );
		::DestroyWindow((HWND) siCtx(m_ctx)->appWindow());
#else
		::exit(status);
#endif		
	}

	o3_get Str versionInfo()
	{		
		o3_trace3 trace;

		Str version(O3_VERSION_STRING);
		version.findAndReplaceAll("_", ".");
		return version;    
	}

	o3_get Str settings(iCtx* ctx)
	{
		return ctx->mgr()->allSettings();
	}		

	o3_set Str setSettings(iCtx* ctx, const Str& settings, siEx* ex)
	{
		if (!ctx->mgr()->writeAllSettings(settings) && ex)
			*ex = o3_new(cEx)("O3 settings could not be saved.");
		return settings;
	}

	o3_get Str settingsURL()
	{
		return Str(O3_UI_URL) + "/" + O3_VERSION_STRING + "/settings.html";
	}

    o3_fun bool loadModule(iCtx* ctx, const char* name) 
    {
        o3_trace3 trace;

        return ctx->mgr()->loadModule(name);
    }

	o3_fun void require(iCtx* ctx, const Str& module)
	{
#ifdef O3_PLUGIN
		m_to_approve.pushBack(module);
#else
		ctx->mgr()->loadModule(module);
#endif
	}

	o3_fun void loadModules(iCtx* ctx) 
	{
		o3_trace3 trace;
		// if a load is in progress the second call should fail
		if (m_loading)
			return;

		m_ctx = ctx;
		ctx->mgr()->pool()->post(Delegate(this, &cO3::moduleLoading),
			o3_cast this);
	}

	o3_set siScr setOnUpdateNotification(iCtx* ctx, iScr* scr)
	{
		m_ctx = ctx;
		return m_onnotification = scr;
	}

	o3_get siFs settingsDir(iCtx* ctx)
	{
		siMgr mgr = ctx->mgr();
		if (!mgr->safeLocation())
			return siFs();

		factory_t f = ctx->mgr()->factory("settingsDir");
		siFs ret = f ? siFs(f(0)) : siFs();
		return ret;
	}

	o3_get Str approvalURL()
	{
		siCtx ctx = (m_ctx);
		siMgr mgr = siCtx(m_ctx)->mgr();
		Str host = hostFromURL(mgr->currentUrl());
		Str url = Str(O3_UI_URL) + "/" + O3_VERSION_STRING + "/approve.html?domain=";
		url.append(host);
		return url; 
	}

	
	void onDone(iUnk*)
	{
		m_loading = false;
		Delegate(siCtx(m_ctx),m_ondone)(this);
	}

	void onStateChange(iUnk* http)
	{
		siHttp ihttp = http;
		m_load_progress->setState(
			ihttp->readyState());
		Delegate(siCtx(m_ctx), m_onprogress)(
			siScr(m_load_progress.ptr()));
	}

	void onProgress(iUnk* http)
	{
		siHttp ihttp = http;
		m_load_progress->setBytesReceived(
			ihttp->bytesReceived());
		Delegate(siCtx(m_ctx), m_onprogress)(
			siScr(m_load_progress.ptr()));
	}

	void onNotification(iUnk* http)
	{
		http;
		Delegate(siCtx(m_ctx), m_onprogress)(
			siScr(this));
	}

	void onFail(iUnk*)
	{
		Delegate(siCtx(m_ctx), m_onfail)(
			siScr(this));	
	}

	// read the settings, checks it against m_to_approve
	// marks the component to be approved in the settings file with '2'
	// opens a blocking approval box, that let's the user to approve components
	// checks the settings file again to see which components are approved now
	void approveModules() 
	{
		siCtx ctx = siCtx(m_ctx);
		siMgr mgr = ctx->mgr();
		// if localhost, we dont need any approval
		Str host = hostFromURL(mgr->currentUrl());
		/*if (strEquals(host.ptr(), "localhost")) {
			m_to_load = m_to_approve;
			m_to_approve.clear();
			return;
		}*/

		// read settings
		tMap<Str, int> settings = mgr->readSettings();

		// approval
		Str name;
		size_t to_approve_no = 0;
		tList<Str>::Iter it;
		for (it = m_to_approve.begin(); 
			it != m_to_approve.end(); ++it) 
		{
			// if not yet approved set it to be approved in the
			// settings file
			if ( settings[*it] != 1) {				
				settings[*it] = 2;
				to_approve_no++;
			}
		}

		if (to_approve_no) {		
			// update settings file
			mgr->writeSettings(settings);
			// set up files listener on the setting file
			// show the message box and wait untill the setting file
			// is changed
			mgr->monitorSettings(ctx, m_change_event);
			ctx->loop()->post(Delegate(ctx, m_onapprove),o3_cast this);
			m_change_event->wait(m_mutex);
			ctx->loop()->post(Delegate(ctx, m_onapprove),o3_cast this);
			// update settings data from file
			settings = mgr->readSettings();
		}
		// fill the m_to_load list
		for (it = m_to_approve.begin(); 
			it != m_to_approve.end(); ++it) 
		{
			if (settings[*it] == 1)
			 m_to_load.append(*it);
		}

		m_to_approve.clear();
	}


	// loading the approved modules, downloading/unpacking/validating 		
	// them if they are missing repeating the process if it failed 
	// and the user selected retry in the pop-up warning window
	// NOTE: this method is executed from a worker thread async
	// NOTE2: after it has finished it will launch the updating asynch
	void moduleLoading(iUnk*)
	{
		approveModules();

		siCtx ctx = siCtx(m_ctx);
		siMgr mgr = ctx->mgr();		
		tList<Str>::Iter
			it = m_to_load.begin(),
			end = m_to_load.end();	
		
		bool success = true;
		for (; it != end; ++it) {
			if ( ! mgr->loadModule(*it)) {					
				m_load_progress->setFileName(*it);
				Buf downloaded = mgr->downloadComponent(ctx,*it,
					Delegate(this, &cO3::onStateChange), 
					Delegate(this, &cO3::onProgress));
				siStream stream = o3_new(cBufStream)(downloaded);
				if (!unpackModule(*it, stream) 
					|| !mgr->loadModule(*it))
						success = false;
			}
		}

		if (success) {			
			m_to_load.clear();
			ctx->loop()->post(Delegate(this, &cO3::onDone),o3_cast this);
		} else
			ctx->loop()->post(Delegate(this, &cO3::onFail),o3_cast this);

		// starting the component update in the bg... 
		ctx->mgr()->pool()->post(Delegate(this, &cO3::moduleUpdating),
			o3_cast this);		
	}

	// unzip the downloaded module, validates it and put the dll in place
	bool unpackModule(const Str& name, iStream* zipped, bool update=false ) 
	{
		using namespace zip_tools;
		bool ret = false;
		siCtx ctx(m_ctx);		
		if (!ctx || !zipped)
			return false;		
		Str sigName; 
#ifdef O3_WIN32		
		Str	fileName = name + O3_VERSION_STRING;				
		sigName = fileName;		
		fileName.append(".dll");
#else
		Str fileName = Str("lib") + name + O3_VERSION_STRING;
		sigName = fileName;		
		fileName.append(".dylib");		 
#endif		
		Str path("tmp/");				
		path.appendf("%s%i",name.ptr(), ctx.ptr());
		siFs fs = ctx->mgr()->factory("fs")(0),
			components = fs->get("components"),
			tmpFolder = fs->get(path),
			unzipped = tmpFolder->get(fileName),
			signature = tmpFolder->get("signature");

		if (!components->exists())
			components->createDir();

		siStream unz_stream = unzipped->open("w");
		siStream sign_stream = signature->open("w");
		if (!unz_stream || !sign_stream)
			return false;	
		// unzipping
		siEx error;
		CentralDir central_dir;
		if (error = readCentral(zipped, central_dir))
			goto error;

		if (error = readFileFromZip(fileName,zipped,unz_stream,central_dir))
			goto error;

		if (error = readFileFromZip(sigName,zipped,sign_stream,central_dir))
			goto error;

		// validating
		unz_stream = unzipped->open("r");
		sign_stream = signature->open("r");
		if (!validateModule(unz_stream,sign_stream))
			goto error;		

		if (update) {
			// rename original dll...
			siFs original = components->get(fileName);
			Str prefix = "tmp/toRem";
			prefix.appendf("%d", ctx.ptr());
			if (original && original->exists())
				original->move(fs->get(prefix + fileName));
		}
		// move validated dll file to the root folder of o3
		unz_stream->close();
		unzipped->move(components, &error);
		ret = !error.valid();

		// if the move failed check if the file is there already 
		// (other process can finish it earlier)
		if (components->get(fileName)->exists() && !update)
			ret = true;

error:
		if (unz_stream)
			unz_stream->close();
		if (sign_stream)
			sign_stream->close();
		fs->get("tmp")->remove(true);
		return ret;
	
	}

	// checks the signiture comes with the dll for validation
	bool validateModule(iStream* data, iStream* signature)
	{
		using namespace Crypto;
		if (!data || !signature)
			return false;

		Buf hash(SHA1_SIZE),encrypted,decrypted;
		if (!hashSHA1(data, (uint8_t*) hash.ptr())) 
			return false;

		hash.resize(SHA1_SIZE);
		size_t enc_size = signature->size();
		encrypted.reserve(enc_size);
		if (enc_size != signature->read(encrypted.ptr(), enc_size))
			return false;

		encrypted.resize(enc_size);
				
		size_t size = (encrypted.size() / mod_size + 1) * mod_size;
		decrypted.reserve(size);
		size = decryptRSA((const uint8_t*) encrypted.ptr(), enc_size, 
			(uint8_t*) decrypted.ptr(), (uint8_t*) &mod, mod_size,
			(const uint8_t*) &pub_exp, pub_exp_size, true);
		
		if ((size_t)-1 == size)
			return false;

		decrypted.resize(size);
		return (size == hash.size() &&
			memEquals(decrypted.ptr(), hash.ptr(), size));
	}

	// checks if there is a new root available, then for the modules
	// downloads a zipped file containing hash files for the latest version of each components
	// we check the local versions hash against these values and update the component if needed
	void moduleUpdating(iUnk*)
	{
	
		using namespace zip_tools;
		siCtx ctx = siCtx(m_ctx);
		siMgr mgr = ctx->mgr();
		checkForMajorUpdate();

		Buf zipped = mgr->downloadUpdateInfo(ctx);
		siStream stream = o3_new(cBufStream)(zipped);
		Buf b(SHA1_SIZE);
		siStream hash = o3_new(cBufStream)(b);
		Str ver(O3_VERSION_STRING); 
		// unzipping
		siEx error;
		CentralDir central_dir;
		if (error = readCentral(stream, central_dir))
			return;

		// now let's check all the hash files in the zipped file
		// against the hash of the local version of the components
		siFs fs = ctx->mgr()->factory("fs")(0),
			components = fs->get("components");

		tMap<Str, CentralHeader>::ConstIter 
			it = central_dir.headers.begin(),
			end = central_dir.headers.end();

		for (; it!=end; ++it){
			hash->setPos(0);
			if (error = readFileFromZip((*it).key,stream,hash,central_dir))
				// zip file is corrupted, reporting error or restart?
				return;
#ifdef O3_WIN32				 
			siFs local = components->get((*it).key + ".dll");
#else			
			siFs local = components->get((*it).key + ".dylib");
#endif

			// update only components the user already have
			if (!local || !local->exists())
				continue;

			siStream stream_local = local->open("r");
			if (!stream_local)
				continue;

			Buf hash_local(SHA1_SIZE);
			hash_local.reserve(SHA1_SIZE);			
			if (SHA1_SIZE != hashSHA1(stream_local, (uint8_t*)hash_local.ptr()))
				continue;
			
			hash_local.resize(SHA1_SIZE);

			if (!memEquals(hash_local.ptr(), ((cBufStream*)hash.ptr())->m_buf.ptr(),
				SHA1_SIZE)) {
#ifdef O3_WIN32				
					Str name = (*it).key;
#else
					Str name = (*it).key.ptr()+3*sizeof(char);
#endif				
					name.findAndReplaceAll(ver.ptr(), "");
					updateComponent(name);
			}
		}
	
	}

	// if we already know that a component should be updated..,
	// downloading the latest version with some validation, 
	// rename the original, replace it with the new one
	// mark the original to be deleted, remove the temp folder
	void updateComponent( const Str& name ) 
	{
		siCtx ctx = siCtx(m_ctx);
		Buf downloaded = ctx->mgr()->downloadComponent(ctx,name,
			Delegate(), Delegate());
		siStream stream = o3_new(cBufStream)(downloaded);
		unpackModule(name, stream, true);	
	}			

	void checkForMajorUpdate() 
	{
		siCtx ctx = siCtx(m_ctx);
		siMgr mgr = ctx->mgr();
		Str latest = mgr->latestVersion(ctx);
		if (latest.empty())
			return;
		latest.findAndReplaceAll(".", "_");
		if (!strEquals(O3_VERSION_STRING, latest.ptr()))
			ctx->loop()->post(Delegate(this, &cO3::onNotification),o3_cast this);
	}


};

}

#endif // O3_C_O3_H
