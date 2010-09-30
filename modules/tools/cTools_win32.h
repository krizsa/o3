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
#ifndef O3_C_TOOLS1_WIN32_H
#define O3_C_TOOLS1_WIN32_H

#include <tools_win32.h>

namespace o3 {

struct cTools : cScr{
    cTools(){}
    virtual ~cTools(){}

    o3_begin_class(cScr)
    o3_end_class()

    o3_glue_gen()

    typedef void (WINAPI *REGSVRCUST)(bool, wchar_t*);
    typedef void (WINAPI *UNREGSVRCUST)(bool);    

    static Var getProperty(iCtx* ctx, iScr* obj, const char* prop_name)
    {
        if (!obj || !ctx)
            return Var(Var::TYPE_VOID);

        Var ret;
        int scr_id = obj->resolve(ctx, prop_name, false);
        obj->invoke(ctx, ACCESS_GET, scr_id, 0, 0, &ret);
        return ret;
    }

    static o3_ext("cO3") o3_get Str tempPath()
    {
        Str ret = tmpPath(false);            
        ret.findAndReplaceAll("\\", "/");
        return ret;
    }

    static o3_ext("cO3") o3_get Str selfPath() 
    {
        Str ret = getSelfPath();
        ret.findAndReplaceAll("\\", "/");
        return ret;
    }

    static o3_ext("cO3") o3_fun bool checkIfInstalled(const Str& app)
    {
        WStr app_name(app);
        return o3::checkIfInstalled(app_name);
    }

    static o3_ext("cO3") o3_fun bool regDll(const Str& name, bool all_usr)
    {
        //Var rval(g_sys);  
        //getProperty(scr_iscr(0),rval,"name");
        WStr wpath(name);
        HMODULE hModule = LoadLibraryW(wpath.ptr());            
	    REGSVRCUST lpProc = (REGSVRCUST) GetProcAddress(hModule, "DllRegisterServerCust");
        if(lpProc) 
            lpProc(all_usr, wpath.ptr()); 
        else return false;
        FreeLibrary(hModule);
        return true;
    }

    static o3_ext("cO3") o3_fun bool unregDll(const Str& name, bool all_usr)
    {
        //WStr wpath(name);
        HMODULE hModule = LoadLibraryA(name.ptr());            
	    UNREGSVRCUST lpProc = (UNREGSVRCUST) GetProcAddress(hModule, "DllUnregisterServerCust");
        if(lpProc) 
            lpProc(all_usr);             
        else return false;
	    FreeLibrary(hModule);
        return true;     
    }

    static o3_ext("cO3") o3_fun bool regUninstaller(iCtx* ctx, bool all_usr, iScr* iobj) 
    {
        siScr obj(iobj);
        if (!obj)
            return false;

        WStr name = getProperty(ctx,obj,"name").toWStr();
        WStr exe = getProperty(ctx,obj,"exe").toWStr();
        WStr args = getProperty(ctx,obj,"args").toWStr(); 
        WStr icon = getProperty(ctx,obj,"icon").toWStr();
        WStr path = getProperty(ctx,obj,"path").toWStr();
        WStr dispname = getProperty(ctx,obj,"displayname").toWStr();
        WStr helplink = getProperty(ctx,obj,"helplink").toWStr();
        WStr publisher = getProperty(ctx,obj,"publisher").toWStr(); 
        WStr major = getProperty(ctx,obj,"major").toWStr();
        WStr minor = getProperty(ctx,obj,"minor").toWStr();    

        return addUninstallerEntry( all_usr, name, exe, args, icon, path, dispname, 
            helplink, publisher, major, minor );
    }

    static o3_ext("cO3") o3_fun bool unregUninstaller(bool all_usr, const Str& name) 
    {         
        WStr wname = WStr(name);
        return removeUninstallerEntry(all_usr, wname);
    }

    static o3_ext("cO3") o3_fun Str getUninstPath(const Str& name) 
    {
        WStr wname = WStr(name);
        return uninstallerString(wname);    
    }

    static o3_ext("cO3") o3_fun bool regMozillaPlugin(iCtx* ctx, bool all_usr, iScr* iobj) 
    {
        siScr obj(iobj);
        if (!obj)
            return false;

        Var rval(g_sys);
        WStr company = getProperty(ctx,obj,"company").toWStr();
        WStr appname = getProperty(ctx,obj,"appname").toWStr();
        WStr version = getProperty(ctx,obj,"version").toWStr();
        WStr path = getProperty(ctx,obj,"path").toWStr();
        WStr product = getProperty(ctx,obj,"product").toWStr();
        WStr description = getProperty(ctx,obj,"description").toWStr();
        WStr mimetype = getProperty(ctx,obj,"mimetype").toWStr();

        return regMozillaEntry(all_usr, company, appname, version, path,
            product, description, mimetype );
    }

    static o3_ext("cO3") o3_fun bool unregMozillaPlugin(bool all_usr, const Str& company, 
        const Str& app, const Str& version) 
    {
        return removeMozillaEntry(all_usr, WStr(company), WStr(app), WStr(version));            
    }

    static o3_ext("cO3") o3_get bool adminUser() 
    {
        return (TRUE==IsUserAnAdmin());
    }

    static o3_ext("cO3") o3_get int winVersionMajor()
    {

        OSVERSIONINFO osvi;

        ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&osvi);
        return (int)osvi.dwMajorVersion;
    } 

	static o3_ext("cO3") o3_set int exitCode(iCtx* ctx, int code)
	{
		ctx->setValue("exitCode", Var(code));
		return code;
	}

	static o3_ext("cO3") o3_fun bool regMozillaEntry(bool all_usr, const wchar_t* company, 
		const wchar_t* appname, const wchar_t* version, const wchar_t* path,
		const wchar_t* prodname, const wchar_t* descr, const wchar_t* mime) 
	{
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

	static o3_ext("cO3") o3_fun bool unregMozillaEntry(bool all_usr, const wchar_t* company, 
		const wchar_t* appname, const wchar_t* version) 
	{
		WStr base(L"Software\\MozillaPlugins\\@");
		base.append(company); base.append(L"/");
		base.append(appname); base.append(L";version=");
		base.append(version);
		recursiveDeleteKey(all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, base);
		return true; 
	}

	static o3_ext("cO3") o3_fun bool regIEExtension(bool all_usr, const wchar_t* id, const wchar_t* o3id, 
		 const wchar_t* text)
	{
		WStr reg("Software\\Microsoft\\Internet Explorer\\Extensions\\");
		WStr id2 = WStr("{") + id + L"}";
		HKEY hkey1(0);
		if (ERROR_SUCCESS != regCreate(
			all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, reg + id2, hkey1))
		{
			RegCloseKey(hkey1);
			return false;
		}

		regSet(hkey1,L"CLSID",L"{1FBA04EE-3024-11d2-8F1F-0000F87ABD16}");
		regSet(hkey1,L"ClsidExtension",WStr("{") + o3id + L"}");
		regSet(hkey1,L"MenuText",text);
		RegCloseKey(hkey1);
		return true;
	}

	static o3_ext("cO3") o3_fun bool unregIEExtension(bool all_usr, const wchar_t* id)
	{
		WStr reg("Software\\Microsoft\\Internet Explorer\\Extensions\\");
		reg.append(WStr("{") + id + L"}");
		recursiveDeleteKey(all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, reg);
		return true;
	}

	static o3_ext("cO3") o3_fun bool regFFExtension(bool all_usr, const wchar_t* name, const wchar_t* path)
	{
		WStr reg("Software\\Mozilla\\Firefox\\Extensions");
		HKEY hkey1(0);
		if (ERROR_SUCCESS != regCreate(
			all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, reg, hkey1))
		{
			RegCloseKey(hkey1);
			return false;
		}

		regSet(hkey1,name,path);
		return true;
	}

	static o3_ext("cO3") o3_fun bool unregFFExtension(bool all_usr, const wchar_t* name)
	{
		WStr reg("Software\\Mozilla\\Firefox\\Extensions");
		HKEY hkey1(0);
		if (ERROR_SUCCESS != RegOpenKeyExW(all_usr ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, 
			reg, 0, KEY_SET_VALUE, &hkey1))
		{
			RegCloseKey(hkey1);
			return false;
		}
		RegDeleteValueW(hkey1,name);
		return false;
	}

	static o3_ext("cO3") o3_fun bool regChromeExtension(bool all_usr, const wchar_t* id, 
		const wchar_t* path, const wchar_t* version)
	{
		all_usr;
		WStr reg("SOFTWARE\\Google\\Chrome\\Extensions\\");
		reg.append(id);
		HKEY hkey1(0);
		// currently chrome api is buggy and only detects this registry under 
		// HKEY_LOCAL_MACHINE
		if (ERROR_SUCCESS != regCreate(HKEY_LOCAL_MACHINE, reg, hkey1))
		{
			RegCloseKey(hkey1);
			return false;
		}

		regSet(hkey1,L"path",path);
		regSet(hkey1,L"version",version);
		return true;
	}

	static o3_ext("cO3") o3_fun bool unregChromeExtension(bool all_usr, const wchar_t* id)
	{
		all_usr;
		WStr reg("Software\\Google\\Chrome\\Extensions\\");
		reg.append(id);
		recursiveDeleteKey(HKEY_LOCAL_MACHINE,reg);
		return true;
	}

	static o3_ext("cO3") o3_fun bool createShortcut(const wchar_t* linkto, const wchar_t* store, const wchar_t* desc)
	{
		return S_OK == createLink(linkto, store, desc);
	}
	
	static o3_ext("cO3") o3_fun Str startMenuPath(bool all_usr)
	{
		WStr path; path.reserve(MAX_PATH);

		if (!SHGetSpecialFolderPathW( NULL, path.ptr(),
			all_usr ? CSIDL_COMMON_STARTMENU : CSIDL_PROGRAMS, FALSE ))
			return Str();

		path.resize(strLen(path.ptr()));
		return Str(path);
	}

    };
}

#endif // O3_C_TOOLS1_WIN32_H
