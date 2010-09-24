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
#ifndef O3_C_FS1_WIN32_H
#define O3_C_FS1_WIN32_H

#include "cFsBase.h"
#include "tools_glue.h"
#include <shlwapi.h>

namespace o3 {
    

    //utility:
    Str flattenPath(const char* path) {        
        Str result;
        size_t from(0),del_from,l = strLen(path);
        bool after_dotdot(false);    

        for (size_t i = 0; i<l; i++) {
            if (path[i] == '/' || i==l-1) {

                if (i+1<l && path[i+1] == '/')
                    continue;

                if ( (i+1<l) && path[i+1] == '.'){                    
                    if ( ((i+2<l) && path[i+2] == '.') && (i+3==l || path[i+3] == '/') ){
                    //remove last part (..)
                        if (after_dotdot) {
                            del_from = findRight(result, result.size(), '/');
                            if (del_from == NOT_FOUND)
                                del_from = 0;
                            
                            result.remove(del_from, result.size()-del_from);
                        }
                        after_dotdot = true;
                        from = i + 3;
                        i+=2;
                        continue;
                    }
                    else if ( (i+2 == l) || path[i+2] == '/') {                     
                    //ignore (.)       
                        result.append(path+from, i - from );
                        from = (i++) + 2;
                        after_dotdot = false;
                        continue;
                    }                    
                }

                //add the next chunk from 'from' to the last character before the current
                //'\' or if this was the last char and there is no trailer '\' then to the
                //last char
                result.append(path+from, (path[i] == '/') ? i-from : i-from+1);                
                from = i;                
                after_dotdot = false;
            }
        }        
        return result;
    }

    bool validateWinPath(const char*) {
        //TODO: implement
        return true;
    }


    struct cFs : cFsBase {
        typedef WIN32_FILE_ATTRIBUTE_DATA FAttrib;

        //root_path have to be in the right format already 
        cFs(const char* local_path, const char* root_path) 
            : m_root_path(root_path),m_local_path()
        {
            Str full_path;
			if (local_path && local_path[0] == '/')
				full_path = ++local_path;
			else if ( !root_path || !(*root_path))
				full_path = local_path;
            else 
                (local_path && *local_path) ? 
                    full_path.appendf("%s%s%s", root_path, "/", local_path)
                    : full_path.appendf("%s", root_path);
            
            //validate path, set m_local_path
            m_valid = parseFullPath(full_path);            			
        }

        //used only from childNodes()
private:
        cFs(cFs* parent, const wchar_t* name) 
            : m_root_path(parent->m_root_path)
            , m_local_path(parent->m_local_path)
            , m_valid(true)
        {
            if (!m_local_path.empty())
                m_local_path.append("/");
            m_local_path.append(Str(name));
            m_valid = true;
        }

public:
        o3_begin_class(cFsBase)            
            o3_add_iface(iFs)
        o3_end_class()

        o3_glue_gen()

        bool        m_valid;
        Str         m_local_path;
        Str         m_root_path;

        int64_t     m_mod_time;
        siHandle    m_change;
        siListener  m_listener;

        WStr        m_win_path;        


        bool parseFullPath(const char* full_path) {
            if (!validateWinPath(full_path)) {                
                return false;
            }

            m_local_path = flattenPath(full_path);
            //if (m_local_path.empty()) {
            //    return false;
            //}

            if (strCaseCompare(m_local_path.ptr(), m_root_path.ptr(), m_root_path.size())) {
                return false;
            }

            size_t l = m_root_path.size();
            if (l == m_local_path.size())
                m_local_path.clear();
            else if (l)
                m_local_path.remove(0, m_root_path.size()+1);            
            
            return true;
        }

        inline WStr winPath() {
            if (m_win_path.empty()) {
               (!m_root_path.empty() && !m_local_path.empty()) ? 
                    m_win_path.appendf(L"%s%s%s", WStr(m_root_path).ptr(), L"/", WStr(m_local_path).ptr())
                    : m_win_path.appendf(L"%s%s", WStr(m_root_path).ptr(), WStr(m_local_path).ptr());
                
                wchar_t* path = m_win_path.ptr();
                while (*path) {
                    if (*path == L'/')
                        *path = L'\\';
                    ++path;
                }
            }
            return m_win_path;
        }

        bool createParents() {            
            if (exists())
                return true;


            siFs parent = get("..");
            cFs* cparent = (cFs*) parent.ptr();
            if (!parent || !cparent->valid())
                return false;
                
			// if the parent is not the root and can not be created:
            if (cparent->m_local_path.size() && !cparent->createParents())
                return false;

            if (!cparent->exists())
                return ::CreateDirectoryW( cparent->winPath().ptr(), NULL ) ? true : false;
            
            return true;
        }


        bool getAttributes(FAttrib& fa) 
        {            
            return ::GetFileAttributesExW( 
                winPath().ptr(),GetFileExInfoStandard, &fa) ? true : false;
        }

public:
		static o3_ext("cO3") o3_get siFs fs(iCtx* ctx)
		{
			ctx;
			return o3_new(cFs("", ""));
		}

        static o3_ext("cO3") o3_get siFs fsSafe(iCtx* ctx) 
        {     
			siFs ret = o3_new(cFs("", ctx->mgr()->root()));
			if (!ret->exists()) {
				// if the root folder does not exists yet, let's
				// create another node, that is able to create
				// the not existing parent folders as well
				cFs* root;
				siFs root2 = root = o3_new(cFs(ctx->mgr()->root(), ""));
				root->createParents();
				root->createDir();
			}
            return ret;
        }

		// the following factory functions are not allowed for the plugin,
		// for security reasons:

        static o3_ext("cO3") o3_get siFs cwd() 
        {
			siFs ret;
#ifndef O3_PLUGIN
            Str cwd = cwdPath();
            cwd.findAndReplaceAll("\\", "/");
            ret = o3_new(cFs(cwd, ""));
#endif
			return ret;
        }

        static o3_ext("cO3") o3_get siFs programFiles() 
        {
			siFs ret;
#ifndef O3_PLUGIN            
			Str path = programFilesPath();
            path.findAndReplaceAll("\\", "/");
            ret = o3_new(cFs)(path, "");
#endif
			return ret;
        }

        static o3_ext("cO3") o3_get siFs appData() 
        {
			siFs ret;
#ifndef O3_PLUGIN
            Str path = appDataPath();
            path.findAndReplaceAll("\\", "/");
            ret = o3_new(cFs)(path, "");            
#endif
			return ret;
        }

		static o3_ext("cO3") o3_get siFs tmpDir()
		{
			siFs ret;
#ifndef O3_PLUGIN
			Str path = tmpPath();
			path.findAndReplaceAll("\\", "/");
			ret = o3_new(cFs)(path, "");            
#endif
			return ret;

		}


		static siUnk settingsDir(iCtx*) 
		{
			Str path = tmpPath();
			path.findAndReplaceAll("\\", "/");
			path.appendf("o3_%s%s", O3_VERSION_STRING, "/settings");						
			siFs ret = o3_new(cFs("", path.ptr()));
			if (!ret->exists()) {
				// if the root folder does not exists yet, let's
				// create another node, that is able to create
				// the no existing parent folders as well
				cFs* root;
				siFs root2 = root = o3_new(cFs(path.ptr(),""));
				root->createParents();
				root->createDir();
			}

			return ret;
		}

		static o3_ext("cO3") o3_fun siFs selectFolder()
		{
			siFs ret;
			Str path = openFolderDialog();
			path.findAndReplaceAll("\\", "/");
			if (!path.size())
				return ret;
			ret = o3_new(cFs)("", path);
			return ret;
		}

		static o3_ext("cO3") o3_fun siFs openFileDialog(const Str& filter) 
		{		
			siFs ret;
			Str path = openFileByDialog( filter );
			path.findAndReplaceAll("\\", "/");
			if (!path.size())
				return ret;
			ret = o3_new(cFs)("", path);
			return ret;
		}

		static o3_ext("cO3") o3_fun Str saveAsFile(const Str& data, const Str& defaultFile) 
		{		
			return saveAsByDialog( data, "All [*.*]", defaultFile );
		}

        virtual bool valid() {
            return m_valid;
        }
	    
        virtual bool exists() {
            if (!m_valid) 
                return false;

            FAttrib fa;
            return getAttributes(fa);
        }		    
        
        virtual int64_t accessedTime() {
            FAttrib fa; 			
		    if ( !m_valid || !getAttributes(fa)) 
                return -1;
            
            ULARGE_INTEGER ret;
            ret.HighPart = fa.ftLastAccessTime.dwHighDateTime;
            ret.LowPart = fa.ftLastAccessTime.dwLowDateTime;
            return ret.QuadPart;
        }
	    
        virtual int64_t createdTime() {
            FAttrib fa; 			
		    if ( !m_valid || !getAttributes(fa)) 
                return -1;
            
            ULARGE_INTEGER ret;
            ret.HighPart = fa.ftCreationTime.dwHighDateTime;
            ret.LowPart = fa.ftCreationTime.dwLowDateTime;
            return ret.QuadPart;            
        }

        virtual int64_t modifiedTime() {
            FAttrib fa; 			
		    if ( !m_valid || !getAttributes(fa)) 
                return -1;
            
            ULARGE_INTEGER ret;
            ret.HighPart = fa.ftLastWriteTime.dwHighDateTime;
            ret.LowPart = fa.ftLastWriteTime.dwLowDateTime;
            return ret.QuadPart;   
        }
        
	    virtual size_t size() {
            FAttrib fa;
		    if ( !m_valid || !getAttributes(fa)) 
                return 0;

            ULARGE_INTEGER ret;
            ret.HighPart = fa.nFileSizeHigh;
            ret.LowPart = fa.nFileSizeLow;
            return (size_t) ret.QuadPart;
        }

		HANDLE writeAttribute()
		{
			return  CreateFileW(	winPath(),
				FILE_GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				0,
				NULL);

		}

		virtual int64_t setAccessedTime(int64_t time)
		{
			FAttrib fa;
			if (!m_valid || !getAttributes(fa))
				return time; // exception here...

			ULARGE_INTEGER t;
			t.QuadPart = time;
			fa.ftLastAccessTime.dwHighDateTime = t.HighPart;
			fa.ftLastAccessTime.dwHighDateTime = t.LowPart;

			HANDLE h = writeAttribute();
			if (INVALID_HANDLE_VALUE == h)
				return time; // exception here...

			::SetFileTime(h,&fa.ftCreationTime,
				&fa.ftLastAccessTime,&fa.ftLastWriteTime);
			::CloseHandle(h);
			return time;
		}

		virtual int64_t setModifiedTime(int64_t time)
		{
			FAttrib fa;
			if (!m_valid || !getAttributes(fa))
				return time; // exception here...

			ULARGE_INTEGER t;
			t.QuadPart = time;
			fa.ftLastWriteTime.dwHighDateTime = t.HighPart;
			fa.ftLastWriteTime.dwHighDateTime = t.LowPart;

			HANDLE h = writeAttribute();
			if (INVALID_HANDLE_VALUE == h)
				return time; // exception here...

			::SetFileTime(h,&fa.ftCreationTime,
				&fa.ftLastAccessTime,&fa.ftLastWriteTime);
			::CloseHandle(h);
			return time;
		}

		virtual int64_t setCreatedTime(int64_t time)
		{
			FAttrib fa;
			if (!m_valid || !getAttributes(fa))
				return time; // exception here...

			ULARGE_INTEGER t;
			t.QuadPart = time;
			fa.ftCreationTime.dwHighDateTime = t.HighPart;
			fa.ftCreationTime.dwHighDateTime = t.LowPart;

			HANDLE h = writeAttribute();
			if (INVALID_HANDLE_VALUE == h)
				return time; // exception here...

			::SetFileTime(h,&fa.ftCreationTime,
				&fa.ftLastAccessTime,&fa.ftLastWriteTime);
			::CloseHandle(h);
			return time;	
		}

        virtual Str path() {
            Str path("/");
            path.append(m_local_path);
            return path;
        }

		Str fullPath() {
			Str ret(winPath());
			ret.findAndReplaceAll("\\", "/");
			return ret;
		}


        virtual int permissions() {
            //!TODO: implement
            return 0;
        }
  
	    virtual Type type() {
            FAttrib fa;
		    if ( !m_valid || !getAttributes(fa)) 
                return TYPE_INVALID;

            if (fa.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
                return TYPE_DIR;

            return TYPE_FILE;
        }

        virtual siStream open(const char* mode, siEx* ex) {
            if (!m_valid)
                return siStream();

            DWORD desired_access;
			DWORD share_mode;
			DWORD creation;
			bool append_mode = false;

            if(*mode == 'r'){				
			    desired_access = GENERIC_READ;
			    share_mode = FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;//FILE_SHARE_READ;
			    creation = OPEN_EXISTING;
		    }		
		    else if(*mode == 'w'){
			    desired_access = GENERIC_WRITE;
		        share_mode = FILE_SHARE_READ;				
				
                if(*(++mode) == 'a')
                    append_mode = true;
                
                creation = append_mode ? OPEN_EXISTING : CREATE_ALWAYS;	
                if (!exists() && !createFile()) {
                    o3_set_ex("Can not create file.");
                    return siFs();
                }
            
            } else {
                o3_set_ex("Invalid access mode.");
                return siFs();
            }

            HANDLE hFile = CreateFileW(	    winPath(),
										    desired_access,
										    share_mode,
										    NULL,
										    creation,
										    0,
										    NULL);

            if (INVALID_HANDLE_VALUE == hFile) {
                Str e = getLastError();
				o3_set_ex(e);
                return siFs();
            }

            siStream s( o3_new(cStream)(hFile) );            
            return s;
        }
        
        virtual siFs parent() {
            return get("..");
        }

	    virtual bool hasChildren() {
            return listChildren<tVec<Str>>(true).size() 
                ? true : false;
        }

    	inline void getNextChild(WIN32_FIND_DATAW find_data, tVec<siFs>& vect) {
			
		    if (	!strEquals(find_data.cFileName, L".") 
				    && !strEquals(find_data.cFileName, L".."))
		    {			
                vect.push(siFs( o3_new(cFs(this, find_data.cFileName) ) ));
		    }			
	    }

      	inline void getNextChild(WIN32_FIND_DATAW find_data, tVec<Str>& vect) {
			
		    if (	!strEquals(find_data.cFileName, L".") 
				    && !strEquals(find_data.cFileName, L".."))
		    {			
                vect.push( Str(find_data.cFileName) );
		    }			
	    }

        //if only_first is set it find only the first real child
        template<typename T>
        inline T listChildren(bool only_first = false) {
	        T ret;
            if (type() != TYPE_DIR)
                return ret;

            WIN32_FIND_DATAW find_data;
	        HANDLE hFind;
	        WStr location(winPath());
	        location.append(L"\\*");

            hFind = ::FindFirstFileW(location.ptr(), &find_data);
	        if (hFind == INVALID_HANDLE_VALUE) 
		        return ret;

	        getNextChild(find_data, ret);							
    		
            while ( ::FindNextFileW( hFind,&find_data ) 
                && (!only_first || ret.size()<1)) {
		        
                    getNextChild(find_data, ret);									
	        }

            ::FindClose(hFind);                               
            return ret;
        }

        virtual tVec<siFs> children() {
            return listChildren<tVec<siFs>>();
        }

        virtual tVec<Str> scandir(const char* path = 0) {            
            if (path)
                return ((cFs*) get(path).ptr())->scandir();
            else
                return listChildren<tVec<Str>>();
        }
        
        virtual siFs get(const char* path) {
            if (!m_valid)
                return siFs();

            //input.findAndReplaceAll((size_t)0,  '\\', '/');
            Str local_path;
            if (path[0] == '/')
                local_path.appendf(path); 
            else {
                local_path.append(m_local_path); 

                if (local_path.size())
                    local_path.append('/');

                local_path.append(path); 
            }
           
            return siFs( o3_new(cFs)(local_path, m_root_path) );
        }

	    virtual bool createFile() {
            if (!createParents())
                return false;

		    HANDLE hFile = CreateFileW(	winPath().ptr(),
										    GENERIC_WRITE,
										    0,
										    NULL,
										    CREATE_ALWAYS,
										    FILE_FLAG_SEQUENTIAL_SCAN,
										    NULL );

		    if (INVALID_HANDLE_VALUE == hFile) 
			    return false;		    
            
            ::CloseHandle(hFile);
            return true;
        }

	    virtual bool createDir() {
            if (exists())
                return type() == TYPE_DIR;
            if (!m_local_path.empty() && !createParents())
                return false;

            return ::CreateDirectoryW(winPath(), NULL) ? true : false;
                
        }

	    virtual bool createLink(iFs* to) {
            to;
            return false;
        }

		virtual siFs move(iFs* to, siEx* ex=0) {
			
			tVec<siFs> nodes;
			siFs to1=to;

			switch (type()) {
				case TYPE_DIR:
					to->createDir();
					nodes = children();
					for (size_t i = 0; i < nodes.size(); ++i) { 
						nodes[i]->move(to->get(nodes[i]->name()), ex);
						// stop moving files on the first failure:
						if (ex && *ex)
							break;
					}
					break;
				case TYPE_FILE:
					if (to->isDir())
						to1 = to->get(name()).ptr();
					
					if (!MoveFileExW(winPath().ptr(), 
						((cFs*) to1.ptr())->winPath().ptr(), 
						MOVEFILE_COPY_ALLOWED|MOVEFILE_WRITE_THROUGH) && ex)
							*ex = o3_new(cEx)(getLastError());
					break;
				default:
					return 0;
			}
			return to1; 
		}

	    virtual bool remove(bool deep = false) {
            const size_t max_children = 20;
            switch (type()) {
                case TYPE_FILE:
                    if (! ::DeleteFileW(winPath()))
                        return false;
                    break;
                case TYPE_DIR:
                    if (deep){
                        tVec<siFs> nodes(children());                        
                        size_t cno = nodes.size(); 
                        WStr message;
                        if (cno > max_children) {
                            message.appendf(L"%s%s%s%d",L"Do you really want to remove ",
                                winPath().ptr(),L" and all of its children? ",(int)cno);
                            
                            if (IDYES != MessageBoxW(NULL,message,L"Warning!", 
                                MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2))
                                    return false;
                        }

                        for (size_t i=0; i < cno; i++)
                            nodes[i].ptr()->remove(true);
                        
                        if (! ::RemoveDirectoryW(winPath()))
                            return false;                            

                    } else {
                        if (! RemoveDirectoryW(winPath()))
                            return false;
                    }
                    break;
                default:                    
                    return false;
            } 
            return true;
        }
        
        virtual void onChangeNotification(iUnk*) 
        {
            if (!m_change)
                return;

            FindNextChangeNotification(m_change->handle());            
            switch(type()){    
                 case TYPE_DIR:
					 m_onchange_delegate(this); 
                     break;
                 case TYPE_FILE:					
                    int64_t modt = modifiedTime();                                             
					if (modt != -1 && m_mod_time != modt){
						m_mod_time = modt;                   
                        m_onchange_delegate(this);
                    }                                         
                    break;
            }       
        }

        void startListening() 
        {
            if (!exists()) {
                // o3_set_ex(ex_file_not_found);
                return;
            }

            siFs to_monitor;
            switch(type()){    
                case TYPE_DIR:
			        to_monitor = this;
                    break;
                case TYPE_FILE:
                    to_monitor = parent();
                    m_mod_time = modifiedTime();
					break;
                default:
                    // o3_set_ex(ex_invalid_op);
                    return;        
            }

            DWORD f = FILE_NOTIFY_CHANGE_LAST_WRITE 
	            | FILE_NOTIFY_CHANGE_FILE_NAME 
                | FILE_NOTIFY_CHANGE_DIR_NAME
	            | FILE_NOTIFY_CHANGE_SIZE ;            

            WStr path_to_monitor = ((cFs*)to_monitor.ptr())->winPath();
            HANDLE handle = FindFirstChangeNotificationW(
                path_to_monitor.ptr(),false,f);

            if (INVALID_HANDLE_VALUE == handle){                
                // o3_set_ex(getLastError());
                m_change = 0, m_mod_time = 0;
		        return;
	        }

            m_change = o3_new(cHandle)(handle, cHandle::TYPE_DIRCHANGE);
            siMessageLoop ml = siCtx(m_ctx)->loop();
            m_listener = ml->createListener(m_change.ptr(), 0, 
                Delegate(this, &cFs::onChangeNotification));
		}

        void stopListening() 
        {
            m_listener = 0;
            m_change = 0;
            m_mod_time = 0;            
        }              

		static siUnk rootDir(iCtx*) 
		{
			Str path = tmpPath();
			path.findAndReplaceAll("\\", "/");
			path.appendf("o3_%s", O3_VERSION_STRING);			
			siFs ret = o3_new(cFs("", path.ptr()));
			if (!ret->exists()) {
				// if the root folder does not exists yet, let's
				// create another node, that is able to create
				// the no existing parent folders as well
				cFs* root;
				siFs root2 = root = o3_new(cFs(path.ptr(),""));
				root->createParents();
				root->createDir();
			}

			return ret;
		}

		typedef HRESULT (__stdcall *AssocQueryStringWType)(
			DWORD,DWORD,LPCWSTR,LPCWSTR,LPWSTR,DWORD*);

		o3_fun void openDoc() {
			wchar_t buf1[1024];
			WStr  buf2, buf3;
			// reserve
			WStr ext;

			WStr wname = name();
			//this should be a real find extension function!!!
			size_t p = wname.find(0,L".");
			if (NOT_FOUND != p)
				ext = WStr(wname.substr(p,wname.size()-p));

			if (!ext.empty()) {
				HMODULE module= LoadLibraryW(L"shlwapi.dll");
				AssocQueryStringWType AssocQueryStringWD = 
					(AssocQueryStringWType)GetProcAddress(module, "AssocQueryStringW");
				if(AssocQueryStringWD){                   
					//Lets read the associated executable path from the registry 
					//buf1.reserve(1024);
					DWORD size = 1024;
					AssocQueryStringWD(0, 2, ext.ptr(), L"open", buf1, &size);
					//buf1.resize(size);
					buf2 = L"\"";
					buf2.append(buf1);
					buf2.append(L"\"");
					buf3 = winPath();
				}
				else {
					//if shlwapi.dll could not have be found, we can still try the regular way
					//(fingers crossed on Vista...)
					buf2 = winPath();
				}

				if (module)
					FreeLibrary(module);
			}

			

			SHELLEXECUTEINFOW shex;

			memset( &shex, 0, sizeof( shex) );

			shex.cbSize        = sizeof( SHELLEXECUTEINFOW );
			shex.fMask        = 0;
			shex.hwnd        = 0;
			shex.lpVerb        = L"open";
			shex.lpFile        = buf2.ptr();               
			shex.lpParameters  = buf3.ptr();
			shex.lpDirectory    = 0;
			shex.nShow        = SW_NORMAL;

			::ShellExecuteExW( &shex );
		}

	};

}

#endif // O3_C_FS1_WIN32_H
