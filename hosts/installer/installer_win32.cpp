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

#include <o3.h>
#include <js/js.h>

#include "fs/fs.h"
#include "blob/blob.h"
#include "resource/resource.h"
#include "screen/screen.h"
#include "window/window.h"
#include "tools/tools.h"
#include "process/process.h"
#include "zip/zip.h"

#include "commctrl.h"

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR arg, int show)
//int main(int argc, char **argv) {
{
    using namespace o3;  

    cSys sys;


    //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); 

    INITCOMMONCONTROLSEX cc = {sizeof( INITCOMMONCONTROLSEX ), ICC_WIN95_CLASSES /*| ICC_STANDARD_CLASSES*/};
    InitCommonControlsEx(&cc); 


    int ret = 0;
    {// scope the local vars
        
        siMgr mgr = o3_new(cMgr)();    
        mgr->addExtTraits(cFs::extTraits());
        mgr->addExtTraits(cBlob::extTraits());
        mgr->addExtTraits(cJs::extTraits());
        mgr->addExtTraits(cResource::extTraits());
        mgr->addExtTraits(cResourceBuilder::extTraits());
        mgr->addExtTraits(cScreen::extTraits());
        mgr->addExtTraits(cWindow::extTraits());
        mgr->addExtTraits(cButton::extTraits());
        mgr->addExtTraits(cStaticCtrl::extTraits());
        mgr->addExtTraits(cTools::extTraits());
        mgr->addExtTraits(cProcess::extTraits());
        mgr->addExtTraits(cZip::extTraits());
		mgr->addExtTraits(cUnzip1::extTraits());
		char x[MAX_PATH];
        GetModuleFileNameA(0, x, MAX_PATH);
        char* args[3];
        args[0] = x;
        // TODO: need a command line parser...
        args[1] = arg;
		args[2] = 0;

        siCtx js = o3_new(cJs)(mgr, 2, args, 0, true);
        Buf buf = ((cSys*)g_sys)->resource("installer.js");
        Str script(buf);
        Var rval;
        rval = js->eval(script);  
        Str err = rval.toStr();
		Var exitCode = js->value("exitCode");
        ret = exitCode.toInt32();
		((cJs*)js.ptr())->tear();
    }
    
    //CoUninitialize(); 
    return ret;
}  
