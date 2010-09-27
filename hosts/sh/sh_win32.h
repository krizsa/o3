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

#include <js/js.h>
//#include <test/proto_v1.h>
#include "fastxml/fastxml.h"
#include "xml/xml.h"
#include "socket/socket.h"
#include "fs/fs.h"
#include "blob/blob.h"
#include "console/console.h"
#include "http/http.h"
#include "process/process.h"
#include "protocol/protocol.h"
#include "resource/resource.h"
#include "screen/screen.h"
#include "window/window.h"
#include "tools/tools.h"
#include "process/process.h"
//#include "test/test.h" 

#include "image/image.h"
//#include "scanner/scan.h"
//#include "barcode/barcode.h"

#include "rsa/rsa.h"
#include "sha1/sha1.h"
#include "md5/md5.h"
//#include "canvas/cCanvas1_win32.h"

#include "zip/zip.h"
#include "socket/socket.h"

//int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR arg, int show){
int main(int argc, char **argv) {

    using namespace o3;  

    //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); 

    cSys sys;

    siMgr mgr = o3_new(cMgr)();
    siCtx ctx = o3_new(cJs)(mgr, --argc, ++argv,0,true);
   
    
    //mgr->addExtTraits(cCanvas1::extTraits());
    mgr->addExtTraits(cFs::extTraits());
    mgr->addExtTraits(cHttp::extTraits());
    mgr->addExtTraits(cBlob::extTraits());
    mgr->addExtTraits(cConsole::extTraits());
    mgr->addExtTraits(cFastXml::extTraits());
    //mgr->addExtTraits(cJs1::extTraits());
    mgr->addExtTraits(cSocket::extTraits());
    mgr->addExtTraits(cResource::extTraits());
    mgr->addExtTraits(cResourceBuilder::extTraits());
    mgr->addExtTraits(cScreen::extTraits());
	mgr->addExtTraits(cProcess::extTraits());
	//mgr->addExtTraits(cTest::extTraits());

	mgr->addExtTraits(cImage::extTraits());
	//mgr->addExtTraits(cBarcode::extTraits());
	//mgr->addExtTraits(cScan::extTraits());

	mgr->addExtTraits(cRSA::extTraits());
	mgr->addExtTraits(cSHA1Hash::extTraits());
	mgr->addExtTraits(cMD5Hash::extTraits());
	mgr->addExtTraits(cZip::extTraits());
	mgr->addExtTraits(cFastXml::extTraits());
	mgr->addExtTraits(cXml::extTraits());

	mgr->addFactory("fs", &cFs::rootDir);
	mgr->addFactory("http", &cHttp::factory);

    WSADATA wsd;
    WSAStartup(MAKEWORD(2,2), &wsd);
	int ret = 0;
    bool wait = true;
    {// scope the local vars        
        for(int i = 0; i < argc;i++){
            if(strEquals(argv[i],"-w")) wait = true;
        }	

		Buf buf = ((cSys*)g_sys)->resource("prelude.js");

		if (buf.size() > 0) {
			ctx->eval(Str(buf));
			if (((cJs*)ctx.ptr())->scriptError())
				return -1;
		}

		HANDLE script_file = CreateFileA(argv[0],GENERIC_READ,
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
			NULL,OPEN_EXISTING,0,NULL);		

		if (INVALID_HANDLE_VALUE == script_file) {
			return -1;
		}		

		unsigned long size,high,read;		
		Str script(size = GetFileSize(script_file, &high));
		ReadFile(script_file,script.ptr(), size, &read, 0);
		script.resize(read);
		::CloseHandle(script_file);

		Str cwd(argv[0]);
		cwd.findAndReplaceAll("\\","/");
		size_t i;
		if ( NOT_FOUND != (i = cwd.findRight("/")) && i > 1) {
			cwd.ptr()[i] = 0;
			_chdir(cwd.ptr());
		}		
		
		ctx->eval(script);
		if (((cJs*)ctx.ptr())->scriptError())
			ret = -1;
		

        siCtx1(ctx)->tear();
	}
    
    //CoUninitialize(); 

    // if(wait)
	// getc(stdin);

    WSACleanup();
    return ret;
}  

