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
#define O3_STATIC
//#define O3_WITH_LIBEVENT
#include <js/js.h>
#ifdef O3_STATIC
#include <console/console.h>
//#include <socket/socket.h>
#include <fs/fs.h>
#include <process/process.h>
#include <zip/zip.h>
#include <sha1/sha1.h>
#include <rsa/rsa.h>
#include <md5/md5.h>
#include <blob/blob.h>
//#include <http/http.h>
#include <xml/xml.h>
#endif // O3_STATIC

using namespace o3;

Str readFile(const char* path) 
{
    FILE*   stream;

    stream = fopen(path, "r");
    if (!stream) {
		exit(-1);
	}	
    return Str(Buf(siStream(o3_new(cStream)(stream)).ptr()));
}

void run(iCtx* ctx, const Str& script)
{
    siEx    ex;
	
    ctx->eval(script, &ex);
    if (ex) {
        fprintf(stderr, "%s\n", ex->message().ptr());
        exit(-1);
    }
}

int main(int argc, char** argv, char** envp)
{
#ifdef O3_WITH_LIBEVENT
	event_init();
#endif

    cSys    sys;
    siMgr   mgr = o3_new(cMgr)();
    siCtx   ctx = o3_new(cJs1)(mgr, argc - 1, argv + 1, envp);

    mgr->addExtTraits(cConsole1::extTraits());
    //mgr->addExtTraits(cSocket1::extTraits());
	//mgr->addExtTraits(cHttp1::extTraits());
	mgr->addExtTraits(cXml1::extTraits());
    mgr->addExtTraits(cBlob1::extTraits());
	mgr->addExtTraits(cFs1::extTraits());
    mgr->addExtTraits(cProcess1::extTraits());
	mgr->addExtTraits(cZip1::extTraits());
	mgr->addExtTraits(cRSA1::extTraits());
	mgr->addExtTraits(cSHA1Hash1::extTraits());
	mgr->addExtTraits(cMD5Hash1::extTraits());
	
	if (argc < 2)
        return -1;
    
	Str pre = readFile("/bin/prelude.js"),
		script = readFile(argv[1]);
	
	Str cwd(argv[1]);
	::size_t i;
	if ( NOT_FOUND != (i = cwd.findRight("/")) && i > 1) {
		cwd.ptr()[i] = 0;
		chdir(cwd.ptr());
	}	
	
	run(ctx, pre);
	run(ctx, script);
	
	return 0;
}
