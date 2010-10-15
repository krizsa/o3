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

#ifndef J_TOOLS_WIN32_H
#define J_TOOLS_WIN32_H

#include <wininet.h>
#include <initguid.h>
#include <ocidl.h>
#include <dispex.h>

#include <objsafe.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlguid.h>

#include <direct.h> //for _getcwd

#include <Commdlg.h>

// These includes are necessary because you use cStream *inside* cSys
#include <iHttp.h>
#include <cMgr.h>
#include <cScr.h>
#include <cScrBuf.h>
#include <cStreamBase.h>
#include <cStream.h>
#include <guid.h>


namespace o3 {

    o3_iid(iWindow, 0x929003cd, 0xd146, 0x44b2, 0xb6, 0xbc, 0xe5, 0x91, 0x8e, 
       0xf8, 0x58, 0x97);


struct iWindow : iUnk 
{
    enum FontStyles{FONT_BOLD=1, FONT_ITALIC=2, FONT_UNDERLINE=4, FONT_STRIKEOUT=8};
    virtual int x() = 0;
    virtual int y() = 0;   
    virtual int clientX() = 0;
    virtual int clientY() = 0;
    virtual void* handle() = 0;
};



    // microsoft COM helpers
    struct _Com {
        _Com() {
            ref_count = 0;
        }

        volatile int32_t ref_count;
    };

    #define mscom_begin(cname) \
        const char* _className() { \
            return #cname; \
        } \
        _Com _m_com; \
        \
        virtual ULONG STDMETHODCALLTYPE AddRef() \
        { \
            return (ULONG)atomicInc( _m_com.ref_count ); \
        } \
        \
        virtual ULONG STDMETHODCALLTYPE Release() \
        { \
            if( atomicDec( _m_com.ref_count ) == 0){ \
                this->~cname(); \
                g_sys->free(this); \
            } \
            return (ULONG)_m_com.ref_count; \
        } \
        \
        HRESULT STDMETHODCALLTYPE QueryInterface( \
                REFIID riid, void** ppvObject) \
		{ \
		    if( memcmp( &IID_IUnknown, &riid, sizeof(GUID)) == 0 ){ \
			    *ppvObject = (IUnknown*)((void*)this); \
			    AddRef(); \
			    return S_OK; \
		    }

    #define mscom_begin_debug(cname) \
        const char* _className() { \
            return #cname; \
        } \
        _Com _m_com; \
        \
        HRESULT STDMETHODCALLTYPE QueryInterface( \
                REFIID riid, void** ppvObject) \
		{ \
		    if( memcmp( &IID_IUnknown, &riid, sizeof(GUID)) == 0 ){ \
			    *ppvObject = (IUnknown*)((void*)this); \
			    AddRef(); \
			    return S_OK; \
		    }

    #define mscom_add_dinterface(iid) \
        if( memcmp( &DIID_##iid, &riid, sizeof(GUID)) == 0 ){ \
            *ppvObject = (iid*)this; \
            AddRef(); \
            return S_OK; \
        } \

    #define mscom_add_interface(iid) \
        if( memcmp( &IID_##iid, &riid, sizeof(GUID)) == 0 ){ \
            *ppvObject = (iid*)this; \
            AddRef(); \
            return S_OK; \
        } \

    #define mscom_add_iid_iface(iid,iface) \
        if( memcmp( &IID_##iid, &riid, sizeof(GUID)) == 0 ){ \
            *ppvObject = (iface*)this; \
            AddRef(); \
            return S_OK; \
        } \

    #define mscom_end() \
            return E_NOINTERFACE; \
        } \

    #define mscom_ptr(IFACE) \
    \
    inline const IID& msiid(IFACE*) \
    { \
        return IID_##IFACE; \
    } \
    \
    typedef tMSComPtr<IFACE> S##IFACE;

	#define mscom_ptr_d(IFACE) \
	\
	inline const IID& msiid(IFACE*) \
	{ \
	return DIID_##IFACE; \
	} \
	\
	typedef tMSComPtr<IFACE> S##IFACE;


    template<typename T> struct tMSComPtr{
        tMSComPtr() : m_object(0) {}
        tMSComPtr(T *o) : m_object(0) { 
            copyFrom(o);     
        }

        tMSComPtr(const tMSComPtr &o) : m_object(0) { 
            copyFrom(o);    
        }

        template<typename T1>
        tMSComPtr(const tMSComPtr<T1>& that) : m_object(0)
        {
            if (that.m_object)
                that.m_object->QueryInterface(msiid((T*)0), (void**) &m_object);
        }

        ~tMSComPtr() { 
            Release(); 
        }

        void copyFrom(T *o) {
            Release();
            m_object = o;
            if(m_object)m_object->AddRef();
        }
        void copyFrom(const tMSComPtr &o){
            Release();
            m_object = o.m_object;
            if(m_object)m_object->AddRef();
        }

        void Release(){
            if(m_object)m_object->Release();
            m_object = 0;
        }

        tMSComPtr& operator=(const tMSComPtr &o) {
            copyFrom(o); return *this;
        }

        tMSComPtr& operator=(T *o) {
            copyFrom(o); return *this;
        }

        bool operator!=(tMSComPtr& o) const
        {
            return m_object != o.m_object;
        }

        bool operator==(tMSComPtr& o) const
        {
            return m_object == o.m_object;
        }

        bool operator==(int o) const
        {
            return m_object != 0;
        }

        T* operator->() {
            return m_object ;
        }

        T** operator&() {
            return &m_object ;
        }

        operator T*() {
            return m_object;
        }

        operator bool() const
        { 
            return m_object!=0; 
        }

        T* ptr()
        {
            return m_object;
        }

        T *m_object;
    };
    
    template<typename T,const IID* piid = &__uuidof(T)> 
    struct tMSComQIPtr: tMSComPtr<T>{
        tMSComQIPtr() : tMSComPtr() {}

        tMSComQIPtr(IUnknown *o) : tMSComPtr() {
                copyFrom(o);
        }

        tMSComQIPtr(const tMSComPtr &o) : m_object(0) {
                copyFrom((IUnknown*)o.m_object);
        }

        void copyFrom(IUnknown *o) {
            Release();
            if(o)o->QueryInterface( *piid, (void**)&m_object);
        }

        tMSComQIPtr& operator=(IUnknown *o) {
            copyFrom(o); return *this;
        }

        tMSComQIPtr& operator=(const tMSComPtr &o) {
            copyFrom((IUnknown*)o.m_object); return *this;
        }
    };

    inline HRESULT mscomAdviseFind(IUnknown* pUnkCP, IConnectionPoint **p, const IID& iid) {
        if(!pUnkCP) 
            return S_FALSE;
        tMSComPtr<IConnectionPointContainer> pCPC;
        if (SUCCEEDED(pUnkCP->QueryInterface(__uuidof(IConnectionPointContainer), (void**)&pCPC)))
            return pCPC->FindConnectionPoint(iid, p);
        return S_FALSE;
    }

    inline HRESULT mscomAdvise(IUnknown* pUnkCP, IUnknown* pUnk, const IID& iid, LPDWORD pdw) {
        tMSComPtr<IConnectionPoint> pCP;
        if(SUCCEEDED(mscomAdviseFind(pUnkCP,&pCP,iid)))
            return pCP->Advise(pUnk, pdw);
        return S_FALSE;
    }

    inline HRESULT mscomUnadvise(IUnknown* pUnkCP, const IID& iid, DWORD dw) {
        tMSComPtr<IConnectionPoint> pCP;
        if(SUCCEEDED(mscomAdviseFind(pUnkCP,&pCP,iid)))
            return pCP->Unadvise(dw);
        return S_FALSE;
    }

    #ifndef _T
        #define _T(x)  L##x
    #endif



    inline BOOL APIENTRY createPipeEx(   OUT LPHANDLE lpReadPipe,
                                           OUT LPHANDLE lpWritePipe,
                                           IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
                                           IN DWORD nSize,
                                           DWORD dwReadMode,
                                           DWORD dwWriteMode)
    {

     static ULONG PipeSerialNumber = 1;
     HANDLE ReadPipeHandle, WritePipeHandle;
     DWORD dwError;
     Str PipeNameBuffer;

     // Only one valid OpenMode flag - FILE_FLAG_OVERLAPPED
     if ((dwReadMode | dwWriteMode) & (~FILE_FLAG_OVERLAPPED)) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
     }

     //  Set the default timeout to 120 seconds
     if (nSize == 0) {
      nSize = 4096;
     }

     //sprintf
     PipeNameBuffer.appendf("\\\\.\\Pipe\\TruthPipe.%d.%d",
              GetCurrentProcessId(),
              PipeSerialNumber++);

     ReadPipeHandle = CreateNamedPipeA( PipeNameBuffer,
                                        PIPE_ACCESS_INBOUND | dwReadMode,
                                        PIPE_TYPE_BYTE | PIPE_WAIT,
                                        1,             // Number of pipes
                                        nSize,         // Out buffer size
                                        nSize,         // In buffer size
                                        120 * 1000,    // Timeout in ms
                                        lpPipeAttributes
      );

     if (! ReadPipeHandle) 
        return FALSE;
     
     WritePipeHandle = CreateFileA( PipeNameBuffer,
                                    GENERIC_WRITE,
                                    0,                         // No sharing
                                    lpPipeAttributes,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL | dwWriteMode,
                                    NULL                       // Template file
      );

     if (INVALID_HANDLE_VALUE == WritePipeHandle) {
        dwError = GetLastError();
        CloseHandle( ReadPipeHandle );
        SetLastError(dwError);
        return FALSE;
     }

     *lpReadPipe = ReadPipeHandle;
     *lpWritePipe = WritePipeHandle;
     return( TRUE );
    }

    // Http
    WStr urlHost(const wchar_t *str) {
		// find first two // and third slash
		int32_t b = 0, e = 0;
		for ( b = 0; str[b]!=0; b++ ) if ( str[b]==L'/' && str[b+1]!=L'/' )break; b++;
		if ( str[b]==0 ) return WStr();
		for ( e = b; str[e]!=0; e++ ) if ( str[e]==L'/')break;

		return WStr(str+b,e-b);
	}

    WStr urlPath(const wchar_t *str) {
        // sub/dir/bla.htm
		int32_t b = 0;
		for( b = 0; str[b]!=0; b++ ) if ( str[b]==L'/' && str[b+1]!=L'/' )break; b++;
		if ( str[b]==0) return WStr();
		for( ;str[b]!=0; b++ ) if ( str[b]==L'/')break;b++;
		if ( str[b]==0 ) return WStr();
		return WStr(str + b);
	} 
    


    LONG regCreate(HKEY to, const wchar_t* key, HKEY& out){

        return RegCreateKeyExW(    to, key, 0, NULL, REG_OPTION_NON_VOLATILE,
                                KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
                                NULL, &out, NULL );
    }


    LONG regSet(HKEY to, const wchar_t* key, const wchar_t* value){
        return RegSetValueExW(    to, key, 0, REG_SZ,
                                (const BYTE*) value,
                                (DWORD)sizeof(const wchar_t) * wcslen(value) );

    }

    LONG regSetLoc(HKEY to, const wchar_t* key, const wchar_t* value){
        return RegSetValueExW(    to, key, 0, REG_EXPAND_SZ,
                                (const BYTE*) value,
                                (DWORD)sizeof(const wchar_t) * wcslen(value) );

    }

    LONG regSetDW(HKEY to, const wchar_t* key, DWORD value){
        return RegSetValueExW(    to, key, 0, REG_DWORD,
                                (const BYTE*) &value,
                                (DWORD)sizeof(DWORD));

    }

    //
    // Delete a key and all of its descendents.
    //
    LONG recursiveDeleteKey(HKEY key_parent,           // Parent of key to delete
                            const wchar_t* key_to_del)  // Key to delete
    {
        // Open the child.
        HKEY key_child ;
        LONG lRes = RegOpenKeyExW(key_parent, key_to_del, 0,
                                 KEY_ALL_ACCESS, &key_child) ;
        if (lRes != ERROR_SUCCESS) {
            return lRes ;
        }

        // Enumerate all of the decendents of this child.
        FILETIME time ;
        wchar_t szBuffer[256] ;
        DWORD dwSize = 256 ;
        while (RegEnumKeyExW(key_child, 0, szBuffer, &dwSize, NULL,
                            NULL, NULL, &time) == S_OK) {
            // Delete the decendents of this child.
            lRes = recursiveDeleteKey(key_child, szBuffer) ;
            if (lRes != ERROR_SUCCESS) {
                // Cleanup before exiting.
                RegCloseKey(key_child) ;
                return lRes;
            }
            dwSize = 256 ;
        }

        // Close the child.
        RegCloseKey(key_child) ;

        // Delete this child.
        return RegDeleteKeyW(key_parent, key_to_del) ;
    }

	bool addUninstallerEntry(bool all_usr, const wchar_t *name, const wchar_t *exe, const wchar_t *args, 
		const wchar_t *iicon, const wchar_t *path, const wchar_t *displayname, const wchar_t *helplink, 
		const wchar_t *publisher, const wchar_t *major, const wchar_t *minor)
	{
		iicon;
		displayname;

		WStr base(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
		base.append(name); 	    

		HKEY hkey = 0; 
		WStr buf;
		if (ERROR_SUCCESS != regCreate ( all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, base, hkey)) 
			return false;


        buf.appendf(L"%s%s%s", major, L".", minor);
        regSet(hkey, L"DisplayVersion", buf);
        regSet(hkey, L"DisplayName", name);
        regSet(hkey, L"HelpLink", helplink);
        regSet(hkey, L"InstallPath", path);
        regSet(hkey, L"InstallSource", path);
        regSet(hkey, L"Publisher", publisher);
        buf = WStr();        
        buf.appendf(L"%s%s%s", exe, L" ", args);
        //swprintf( buf2, L"%s%s%s", exe, L" ", args);

        regSet(hkey, L"UninstallString", buf.ptr());
        regSet(hkey, L"URLUpdateInfo", helplink);
        regSet(hkey, L"VersionMajor", major);
        regSet(hkey, L"VersionMinor", minor);
        buf = WStr();
        buf.appendf(L"%s%s", exe, L",1");
	    regSet(hkey, L"DisplayIcon", minor);
        RegCloseKey(hkey);
	    return true;
    }

	bool removeUninstallerEntry(bool all_usr, const wchar_t* name) {
		//!TODO: precheck and validation
		WStr base(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
		base.append(name); 
		recursiveDeleteKey(all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, base);
		return true;
    }

    WStr uninstallerString(const wchar_t* name) {
        WStr ret, base(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
        ret.reserve(MAX_PATH);
        base.append(name);
        
        DWORD mp = MAX_PATH, type;
        HKEY hkey = 0, hkey2 = 0;
        RegOpenKeyExW(HKEY_LOCAL_MACHINE, base.ptr(), 0, KEY_READ, &hkey);
        if (hkey){
            RegQueryValueExW( hkey,L"UninstallString",0,&type, (LPBYTE)ret.ptr(), &mp);
            ret.resize(strLen(ret.ptr()));
            RegCloseKey(hkey);
            return ret;
        }
        
        hkey = 0;            
        RegOpenCurrentUser(KEY_READ, &hkey2);
        RegOpenKeyExW(hkey2, base.ptr(), 0, KEY_READ, &hkey);
       // LONG error; 
        if(hkey && hkey2) {
            RegQueryValueExW( hkey,L"UninstallString",0,&type, (LPBYTE)ret.ptr(), &mp);
            ret.resize(strLen(ret.ptr()));
        }else ret.resize(0);
                
        if (hkey) 
            RegCloseKey(hkey);
        if (hkey2)
            RegCloseKey(hkey2);   
        
        return ret;    
    }

	WStr installDirPath()
	{
		WStr full_app_name;
		full_app_name.appendf(L"%s%s%s",O3_APP_NAME,L"-",O3_PLUGIN_GUID);
		WStr inst_string = uninstallerString(full_app_name); 				
		if (inst_string.empty())
			return inst_string;

		wchar_t* s=inst_string.ptr();
		wchar_t* i=&inst_string.ptr()[inst_string.size()-1];
		for (;i!=s;i--)
			if (*i==L'\\')
				break;
		return WStr(s,i-s);
	}
	
	Str pluginName() 
	{
		return Str("np-") + O3_APP_NAME + "-" +  O3_PLUGIN_GUID + ".dll";
	}

	//Str pluginPath() 
	//{
	//	WStr ret, base = WStr(L"software\\Classes\\CLSID\\{") + stemGUID + L"}";
	//	ret.reserve(MAX_PATH);

	//	DWORD mp = MAX_PATH, type;
	//	HKEY hkey = 0, hkey2 = 0;
	//	RegOpenKeyExW(HKEY_LOCAL_MACHINE, base.ptr(), 0, KEY_READ, &hkey);
	//	if (hkey){
	//		RegQueryValueExW( hkey,L"InProcServer32",0,&type, (LPBYTE)ret.ptr(), &mp);
	//		ret.resize(strLen(ret.ptr()));
	//		RegCloseKey(hkey);
	//		return ret;
	//	}

	//	hkey = 0;            
	//	RegOpenCurrentUser(KEY_READ, &hkey2);
	//	RegOpenKeyExW(hkey2, base.ptr(), 0, KEY_READ, &hkey);
	//	// LONG error; 
	//	if(hkey && hkey2) {
	//		RegQueryValueExW( hkey,L"InProcServer32",0,&type, (LPBYTE)ret.ptr(), &mp);
	//		ret.resize(strLen(ret.ptr()));
	//	}else ret.resize(0);

	//	if (hkey) 
	//		RegCloseKey(hkey);
	//	if (hkey2)
	//		RegCloseKey(hkey2);   

	//	return ret; 
	//}

    bool checkIfInstalled(const wchar_t* name) {
        WStr base(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
        base.append(name);
            
        HKEY hkey = 0, hkey2 = 0;
        RegOpenKeyExW(HKEY_LOCAL_MACHINE, base.ptr(), 0, KEY_READ, &hkey);
        if (hkey){
            RegCloseKey(hkey);
            return true;
        }
        
        hkey = 0;            
        RegOpenCurrentUser(KEY_READ, &hkey2);
        RegOpenKeyExW(hkey2, base.ptr(), 0, KEY_READ, &hkey);
        if (hkey) 
            RegCloseKey(hkey);
        if (hkey2)
            RegCloseKey(hkey2);
        if(hkey && hkey2) 
            return true;      
        
        return false;
    }

	bool addMozillaEntry(bool all_usr, const wchar_t* company, const wchar_t* appname, const wchar_t* version, const wchar_t* path,
		const wchar_t* prodname, const wchar_t* descr, const wchar_t* mime) {
			WStr base(L"Software\\MozillaPlugins\\@");
			base.append(company); base.append(L"/");
			base.append(appname); base.append(L";version=");
			base.append(version);

        HKEY hkey = 0, hkey2 = 0, hkey3 = 0; 
        if (ERROR_SUCCESS != regCreate ( all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, base, hkey)) 
            return false;

        regSet(hkey, L"Path", path);
        regSet(hkey, L"ProductName", prodname);
        regSet(hkey, L"Version", version);
        regSet(hkey, L"Vendor", company);
        regSet(hkey, L"Description", descr);

        if (ERROR_SUCCESS != regCreate ( hkey, L"MimeTypes", hkey2)) {
            RegCloseKey( hkey );
            return false;
        } 

        if (ERROR_SUCCESS != regCreate ( hkey2, mime, hkey3)) {
            RegCloseKey( hkey2 );
            RegCloseKey( hkey );
            return false;
        }

        RegCloseKey( hkey3 );
        RegCloseKey( hkey2 );
        RegCloseKey( hkey );
        return true;
    }

	bool removeMozillaEntry(bool all_usr, const wchar_t* company, const wchar_t* appname, const wchar_t* version) {
		WStr base(L"Software\\MozillaPlugins\\@");
		base.append(company); base.append(L"/");
		base.append(appname); base.append(L";version=");
		base.append(version);
		recursiveDeleteKey(all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, base);
		return true; 
	}

	// web browser control start in IE7 mode even if IE8 is installed by default, we must change the registry
	// to run it in IE8 mode...
	void ssb_setIE8mode(const Str& ssb_name) {
		WStr base(L"SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION");

		WStr wssb_name = ssb_name;
		HKEY hkey = 0;
		HKEY hkey2 = 0;
		RegOpenCurrentUser(KEY_READ, &hkey);
		RegOpenKeyExW(hkey, base.ptr(), 0, KEY_READ, &hkey2);
		
		if(hkey && !hkey2) {
			regCreate(hkey,base.ptr(),hkey2);
		}

		// read the value, if it can not be found open the key in write mode and set the value:
		DWORD ret,data=0,type=0,mp=2;
		if(hkey && hkey2) {
			if (ERROR_FILE_NOT_FOUND 
				== RegQueryValueExW( hkey2,wssb_name.ptr(),0,&type, (LPBYTE)&data, &mp))
			{
				RegCloseKey(hkey2);
				hkey2 = 0;
				RegOpenKeyExW(hkey, base.ptr(), 0, KEY_WRITE, &hkey2);
				if (hkey2)
					ret = regSetDW(hkey2,wssb_name.ptr(),8000);
			}
		}			

		if (hkey) 
			RegCloseKey(hkey);
		if (hkey2)
			RegCloseKey(hkey2);

	}

    size_t findRight(const char* string, size_t pos, char C) {
        for (size_t i=pos; i>0; i--) {
           if (string[i] == C)
               return i;
        }
        if (string[0] == C) 
            return 0;
        return NOT_FOUND;
    }

    WStr getSelfPath() {
        WStr path;
        path.reserve(MAX_PATH);
        size_t l = GetModuleFileNameW ( GetModuleHandle(0), path.ptr(), MAX_PATH );
        path.resize(l);
        return path;
    }

    Str cwdPath()
    {
        Str buffer;
		buffer.reserve(MAX_PATH);
        /* Get the current working directory: */
        char* p = buffer.ptr();
		if( _getcwd( p, _MAX_PATH ) == NULL )
            return Str();
		buffer.resize(strLen(p));

        return buffer;
    }

    WStr appDataPath() 
    {
        WStr path; path.reserve(MAX_PATH);
        if(!SUCCEEDED(SHGetFolderPathW(NULL, 
                                 CSIDL_APPDATA, 
                                 NULL, 
                                 0, 
                                 path.ptr() ))) 
            return WStr();
        path.resize(strLen(path.ptr()));
        return path;
    }

    WStr programFilesPath ()
    {
        WStr path; path.reserve(MAX_PATH);
        if(!SUCCEEDED(SHGetFolderPathW(NULL, 
                                 CSIDL_PROGRAM_FILES, 
                                 NULL, 
                                 0, 
                                 path.ptr() ))) 
            return WStr();
        path.resize(strLen(path.ptr()));
        return path;
    }


    WStr tmpPath(bool low = true)
    {
        WStr tmpPath; tmpPath.reserve(MAX_PATH);
        DWORD ret = GetTempPathW(MAX_PATH,tmpPath.ptr());
        if (ret < 0) 
            return WStr();
    
        tmpPath.resize(ret);
		wchar_t* p = tmpPath.ptr();
		if (p[ret-1]!=L'\\' && p[ret-1]!=L'/')
			tmpPath.append(L"\\");

		if (low)
			if (NOT_FOUND == tmpPath.find(L"Low"))
				tmpPath.append(L"Low\\");        

		return tmpPath;
    }
    
    struct cWinFind
    {
	    WStr    caption;
	    WStr    classnm;
	    DWORD	pid;
	    HWND	hwnd;
    };

    static BOOL CALLBACK EnumWindowsProc(HWND hwnd,  LPARAM lParam)
    {
	    cWinFind *win = (cWinFind*)lParam;
		
	    DWORD pid;
	    GetWindowThreadProcessId(hwnd,&pid);
	    if(win->pid==0|| pid== win->pid)
	    {
		    wchar_t buf[1024];GetClassNameW(hwnd,buf,1024);
            if (win->classnm.empty() || strEquals(win->classnm.ptr(),buf))
		    {
			    GetWindowTextW(hwnd,buf,1024);
                if(win->caption.empty() || strEquals(win->caption.ptr(),buf))
			    {
				    win->hwnd = hwnd;
                    return FALSE;
			    }
		    }
	    }
	    return TRUE;
    }

    inline HWND  SearchWindow(const wchar_t *caption, const wchar_t* classnm, int32_t pid, HWND parent = 0)
    {
	    cWinFind find;

	    if(caption) find.caption = caption;
	    if(classnm) find.classnm = classnm;
	    find.pid = pid;
	    find.hwnd = 0;
	    if(!parent)EnumWindows(EnumWindowsProc,(LONG_PTR)&find);
	    else EnumChildWindows(parent,EnumWindowsProc,(LONG_PTR)&find);
	    return find.hwnd;
    }

    inline Str getLastError() {
        LPVOID lpMsgBuf;        
        DWORD dw = GetLastError(); 

        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR) &lpMsgBuf,
            0, NULL );

         WStr ret((wchar_t*)lpMsgBuf);   
         LocalFree(lpMsgBuf);
         return ret;   
    }

    o3_iid(iWindowProc, 0x519b94ad, 0xdb38, 0x4134, 0xb8, 0x16, 0xc9, 0xde, 0x47, 
        0xc9, 0x76, 0x4f);

    struct iWindowProc : iUnk 
    {
        virtual LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, 
            WPARAM wParam, LPARAM lParam) = 0;
    };

    // this is an agregator message handler, for each created window a pointer to 
    // the related cWnd1 is stored in the window related user data, so each 
    // message can be passed to the actuall class that needs to handle it
    // that pointer is set AND used here
    static LRESULT CALLBACK _WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam){
	    if (message == WM_CREATE) {
            // store the pointer
            CREATESTRUCT &p = *(CREATESTRUCT*)lparam;
            SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)p.lpCreateParams );
        }
        // if the pointer is set, let's use it to send the message to the class
        iWindowProc *wnd = ((iWindowProc*)GetWindowLongPtr( hwnd, GWLP_USERDATA ));
        if(!wnd) // or just use the default...
            return DefWindowProc(hwnd, message, wparam, lparam);         
        
        return wnd->wndProc(hwnd,message,wparam,lparam);
    }

    static wchar_t o3_wnd_class_name[] = L"O3Wnd";                

    void regWndClass(WNDCLASSW& wnd_class) {
        // register the window class, or get its classinfo
        
        if (!::GetClassInfoW( ::GetModuleHandle(0), o3_wnd_class_name, &wnd_class) ) {
	        WNDCLASSEXW wc = { 
                sizeof(WNDCLASSEXW), 
                CS_DBLCLKS, 
                _WndProc,
                0L, 0L, 
                GetModuleHandle(0),
                0, 
                0,
                (HBRUSH)COLOR_WINDOW,0,
                o3_wnd_class_name, 
                NULL };

            ::RegisterClassExW(&wc);
        }      
    }

	// Retrieves the UIObject interface for the specified full PIDL
	static HRESULT SHGetUIObjectFromFullPIDL(LPCITEMIDLIST pidl, HWND hwnd, REFIID riid, void **ppv)
	{
		LPCITEMIDLIST pidlChild;
		IShellFolder* psf;
		*ppv = NULL;
		HRESULT hr = SHBindToParent(pidl, IID_IShellFolder, (LPVOID*)&psf, &pidlChild);
		if (SUCCEEDED(hr))
		{
			hr = psf->GetUIObjectOf(hwnd, 1, &pidlChild, riid, NULL, ppv);
			psf->Release();
		}
		return hr;
	}
	static HRESULT SHILClone(LPCITEMIDLIST pidl, LPITEMIDLIST *ppidl)
	{
		DWORD cbTotal = 0;
		if (pidl)
		{
			LPCITEMIDLIST pidl_temp = pidl;
			cbTotal += pidl_temp->mkid.cb;
			while (pidl_temp->mkid.cb) 
			{
				cbTotal += pidl_temp->mkid.cb;
				pidl_temp = ILNext(pidl_temp);
			}
		}

		*ppidl = (LPITEMIDLIST)CoTaskMemAlloc(cbTotal);

		if (*ppidl)
			CopyMemory(*ppidl, pidl, cbTotal);

		return  *ppidl ? S_OK: E_OUTOFMEMORY;
	}

	// Get the target PIDL for a folder PIDL. This also deals with cases of a folder  
	// shortcut or an alias to a real folder.
	static HRESULT SHGetTargetFolderIDList(LPCITEMIDLIST pidlFolder, LPITEMIDLIST *ppidl)
	{
		IShellLink *psl;

		*ppidl = NULL;

		HRESULT hr = SHGetUIObjectFromFullPIDL(pidlFolder, NULL, IID_IShellLink, (LPVOID*)&psl);

		if (SUCCEEDED(hr))
		{
			hr = psl->GetIDList(ppidl);
			psl->Release();
		}

		// It's not a folder shortcut so get the PIDL normally.
		if (FAILED(hr))
			hr = SHILClone(pidlFolder, ppidl);

		return hr;
	}
	// Get the target folder for a folder PIDL. This deals with cases where a folder
	// is an alias to a real folder, folder shortcuts, the My Documents folder, etc.
	STDAPI SHGetTargetFolderPath(LPCITEMIDLIST pidlFolder, LPWSTR pszPath)
	{
		LPITEMIDLIST pidlTarget;

		*pszPath = 0;
		HRESULT hr = SHGetTargetFolderIDList(pidlFolder, &pidlTarget);

		if (SUCCEEDED(hr))
		{
			SHGetPathFromIDListW(pidlTarget, pszPath);   // Make sure it is a path
			CoTaskMemFree(pidlTarget);
		}

		return *pszPath ? S_OK : E_FAIL;
	}


	Str openFolderDialog()
	{
		wchar_t buf[MAX_PATH];
		wchar_t buf2[MAX_PATH];
		BROWSEINFOW bi;
		ZeroMemory(&bi,sizeof(bi));
		bi.pszDisplayName = buf;
		bi.lpszTitle = L"Ajax.org open directory dialog";
		bi.ulFlags = BIF_RETURNONLYFSDIRS|BIF_DONTGOBELOWDOMAIN;
		
		PIDLIST_ABSOLUTE pid = SHBrowseForFolderW(&bi);
		if (!pid)
			return Str();

		if (S_OK != SHGetTargetFolderPath(pid, buf2))
			return Str();

		CoTaskMemFree(pid);
		return WStr(buf2);
	}


    WStr openFileDialog(bool open, const Str& types, const Str& default=Str(), HWND hwnd = 0)
    {
		WStr filter = types;
		Buf buf(filter.size() * sizeof(wchar_t) * 2);

		wchar_t* src = filter.ptr();
		wchar_t* dest = (wchar_t*) buf.ptr();
		wchar_t* sdest = dest;

		while(*src){
			switch(*src){
				case L'[':
					src++; *dest++ = L'\0';
					while(*src != L']'){						
						if(!*src)
							return Str();

						*dest++ = *src++;
					}
					src++; *dest++ = L'\0';
					break;
				
				case L'\n': case L'\r': 
					src++;
					break;
				
				default:
					*dest++=*src++;
			}
		}
		*dest=L'\0';

		buf.resize(dest-sdest);

		WStr selectedPath = default;
		selectedPath.reserve(MAX_PATH);			
        OPENFILENAMEW ofn;       // common dialog box structure                

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = selectedPath.ptr();
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = (wchar_t*) buf.ptr();
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST; // selectedPath
        
        // Display the Open dialog box. 
		int res = open ? GetOpenFileNameW(&ofn) : GetSaveFileNameW(&ofn);
        if (res==TRUE) 
            selectedPath.resize(strLen(selectedPath.ptr()));            
        else
            selectedPath.resize(0);

		if (!selectedPath.size())
			return Str();

		return selectedPath;
    }

	Str openFileByDialog(const Str& filter)
	{
		return openFileDialog(true, filter);			
	}

	Str saveAsByDialog(const Str& data, const Str& filter, const Str& default)
	{
		WStr selectedPath = openFileDialog(false, filter, default);	

		HANDLE hFile = CreateFileW( selectedPath.ptr(),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			0,
			NULL);

		if (INVALID_HANDLE_VALUE == hFile)
			return false;

		siStream ret = o3_new(cStream)(hFile);		
		ret->write(data.ptr(), data.size());
		return selectedPath;
	}

    siStream openSelf()
    {        
        WStr path;
        path.reserve(MAX_PATH);
        size_t l = GetModuleFileNameW ( GetModuleHandle(0), path.ptr(), MAX_PATH );
        path.resize(l);
        
        HANDLE hFile = CreateFileW( path.ptr(),
									GENERIC_READ,
									FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
									NULL,
									OPEN_EXISTING,
									0,
									NULL);
            
        if (INVALID_HANDLE_VALUE == hFile) 
            return siStream();
              
        siStream ret = o3_new(cStream)(hFile);

        return ret;
    }

    WStr tempFile(const Buf& data)
    {
        WStr path,full; 
        path.reserve(MAX_PATH);
        full.reserve(MAX_PATH);
        
        path.resize(GetTempPathW(MAX_PATH,path));
        GetTempFileNameW(path, L"o3", 0, full);
        full.resize(strLen(full.ptr()));
        if (!path.size() || !full.size())
            return WStr();

        HANDLE hFile = CreateFileW( full,
									GENERIC_WRITE,
									FILE_SHARE_READ,
									NULL,
                                    OPEN_ALWAYS,
									0,
									NULL);
        
        if (INVALID_HANDLE_VALUE == hFile) 
            return WStr();
        
        siStream stream = o3_new(cStream)(hFile);
        stream->write(data.ptr(), data.size());
        return full;
    }

	// CreateLink - uses the Shell's IShellLink and IPersistFile interfaces 
	//              to create and store a shortcut to the specified object. 
	//
	// Returns the result of calling the member functions of the interfaces. 
	//
	// Parameters:
	// lpszPathObj  - address of a buffer containing the path of the object. 
	// lpszPathLink - address of a buffer containing the path where the 
	//                Shell link is to be stored. 
	// lpszDesc     - address of a buffer containing the description of the 
	//                Shell link. 

	HRESULT createLink(LPCWSTR path_obj, LPCWSTR path_link, LPCWSTR desc) 
	{ 
		HRESULT hres; 
		IShellLink* psl; 

		// Get a pointer to the IShellLink interface. 
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
			IID_IShellLink, (LPVOID*)&psl); 
		if (SUCCEEDED(hres)) 
		{ 
			IPersistFile* ppf; 

			// Set the path to the shortcut target and add the description. 
			psl->SetPath(path_obj); 
			psl->SetDescription(desc); 

			// Query IShellLink for the IPersistFile interface for saving the 
			// shortcut in persistent storage. 
			hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 

			if (SUCCEEDED(hres)) 
			{ 
				hres = ppf->Save(path_link, TRUE); 
				ppf->Release(); 
			} 
			psl->Release(); 
		} 
		return hres; 
	}


    struct HiddenWindow{
        HiddenWindow() : m_hwnd(0)
        {
        
        }

        ~HiddenWindow()
        {
            destroy();
        }

        void create (iCtx* ctx)
        {
			WNDCLASSEXW wnd_class;
			if (!::GetClassInfoExW( ::GetModuleHandle(0), L"O3_HIDDEN", &wnd_class) ) {
				WNDCLASSEXW wc = { 
					sizeof(WNDCLASSEXW), 
					0, 
					WndProc,
					0L, 0L, 
					GetModuleHandle(0),
					0,0,
					0,0,
					L"O3_HIDDEN", 
					NULL };

					::RegisterClassExW(&wc);
			} 

			int e = GetLastError();

            m_hwnd = CreateWindowExW(0,L"O3_HIDDEN", L"O3_HIDDEN_WND", 0, 0, 0, 0, 0,
                                   GetDesktopWindow(), 0, GetModuleHandle(0),
                                   0);
			e = GetLastError();
			if (m_hwnd) {
				SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR) ctx);
				SetTimer(m_hwnd, (UINT_PTR) m_hwnd, 200, 0);        
			}
        }

        void destroy()
        {
            if (m_hwnd)
                DestroyWindow(m_hwnd);
            m_hwnd = 0;
        }
       
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) 
        {
            iCtx* ctx = (iCtx*) GetWindowLongPtr(hWnd, GWLP_USERDATA);			
            if (g_sys && ctx) {             				
                switch (msg) {
                case WM_TIMER:
					// to prevent the wait function to be called again while
					// the previous call have not returned yet...
					SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) 0);
                    ctx->loop()->wait(1);
                    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) ctx);
					break;
                }


            }
        
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        HWND    m_hwnd;

    };

    
    struct CDropTarget : 
        IDropTarget
    {
        CDropTarget() 
        {
        }

        virtual ~CDropTarget()
        {        
        }

        mscom_begin(CDropTarget)
			mscom_add_interface(IDropTarget)
		mscom_end();

        //IDropTarget
        HRESULT STDMETHODCALLTYPE DragEnter( IDataObject * pDataObject, DWORD grfKeyState, 
            POINTL pt, DWORD * pdwEffect )
        {
            *pdwEffect = DROPEFFECT_COPY;
            return NOERROR;
        }

        HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD * pdwEffect )
        {
            *pdwEffect = DROPEFFECT_COPY;
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DragLeave(void)
        { 
            return E_NOTIMPL; 
        }
        
        HRESULT STDMETHODCALLTYPE Drop( IDataObject * pDataObject, DWORD grfKeyState,
            POINTL pt, DWORD * pdwEffect )
        {
            WStr     file_names;

            FORMATETC fmtetc =
                { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stgmed;            
                                    
            if (S_OK == pDataObject->GetData(&fmtetc, &stgmed)) {
                HDROP hdrop = (HDROP)GlobalLock(stgmed.hGlobal);

                if (NULL != hdrop)
                {
                    UINT nFiles = DragQueryFile(hdrop, (UINT)-1, NULL, 0);

                    for(UINT nNames = 0; nNames < nFiles; nNames++)
                    {
                        WStr next_file;
                        next_file.reserve(_MAX_PATH);
                        DragQueryFile
                            (hdrop, nNames, next_file, _MAX_PATH);
                        
                        file_names.append(next_file);
                        file_names.append(L"\n");
                    }
                    GlobalUnlock(hdrop);

                }
                ReleaseStgMedium(&stgmed);

                MessageBoxW(
                    NULL,
                    file_names,
                    L"Files were dropped!",
                    MB_ICONWARNING | MB_OK 
                );
                return (S_OK);
            }
  
            return (S_FALSE);
        }
    };

	bool runElevated( const char* cpath, const char* cparameters = NULL, const char* cdir = NULL ) 
	{
		WStr path(cpath), parameters(cparameters ? cparameters : ""), dir(cdir ? cdir : "");
		SHELLEXECUTEINFOW shex;

		memset( &shex, 0, sizeof( shex) );

		shex.cbSize        = sizeof( SHELLEXECUTEINFOW );
		shex.fMask        = SEE_MASK_NOCLOSEPROCESS;
		shex.hwnd        = 0;
		shex.lpVerb        = L"runas";
		shex.lpFile        = path;
		shex.lpParameters  = parameters.size() ? parameters.ptr() : NULL;
		shex.lpDirectory    = dir.size() ? dir.ptr() : NULL;
		shex.nShow        = SW_NORMAL;

		::ShellExecuteExW( &shex );

		return (int)shex.hInstApp > 32;
	} 

	void runSimple(const char* ccmd) 
	{
		WStr cmd(ccmd ? ccmd : "");
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		// Start the child process. 
		CreateProcessW( NULL,   // No module name (use command line)
			cmd,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi );           // Pointer to PROCESS_INFORMATION structure

		//m_p_info = pi;
	}

}

#endif // J_TOOLS_WIN32_H
