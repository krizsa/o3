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

#include "o3.h"
#include "cCtx.h"
#include "fs/fs.h"
#include "sha1/sha1.h"
#include "rsa/rsa.h"
#include "prv_key.h"

#ifndef O3_WIN32
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif // O3_WIN32

int main(int argc, char **argv) {

	using namespace o3;  

	//CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); 

	if (argc<3)
		return -1;

	cSys sys;

	siMgr mgr = o3_new(cMgr)();
	siCtx ctx = o3_new(cCtx)(mgr);

	Str src(argv[1]), sign(argv[2]);
	siFs cwd = cFs::cwd();
	siFs src_file(cwd->get(src)), sign_file(cwd->get(sign));

	if (!src_file->exists())
		return -2;

	cSHA1Hash* sha = o3_new(cSHA1Hash)();
	cRSA* rsa = o3_new(cRSA)();
	
	siScr sharef(sha);
	siScr rsaref(rsa);

	Buf hash = sha->hash(src_file->blob());
	Buf signature = rsa->encrypt(hash, Buf(prv_mod,prv_mod_size), Buf(prv_exp, prv_exp_size));

	if (!sign_file->exists())
		sign_file->setData(Str::fromBase64(signature.ptr(), signature.size()));
	else {
		Str data = sign_file->data();
		data.append("\n");
		data.append(Str::fromBase64(signature.ptr(), signature.size()));
		sign_file->setData(data);
	}

	return 0;
}