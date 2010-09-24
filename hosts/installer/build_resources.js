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
 
var mode = o3.args.length > 1 ? o3.args[1] : false;
var src = [];
src.push(
	o3.cwd.get("installer_header.bmp"),
	o3.cwd.get("installer_left.bmp"),
	mode ? o3.cwd.get("../../build/" + mode + "/plugin/npplugin.dll")
		: o3.cwd.get("../plugin/npplugin.dll"),
	o3.cwd.get("installer.js"),
	o3.cwd.get("install.ico"),
	o3.cwd.get("o3settings.crx"),
	o3.cwd.get("o3.xpi")
	//o3.cwd.get("lnkmaker.vbs"),
	//o3.cwd.get("../plugin/approve.html"),
	//o3.cwd.get("../plugin/settings.html"),
	//o3.cwd.get("../plugin/settings_impl.html"),
	//o3.cwd.get("../../samples/o3.js")
);
	
var tgt = mode ? o3.cwd.get("../../build/" + mode + "/installer/o3plugin.exe") 
	: o3.cwd.get("o3plugin.exe");

function checkFile(file) {    
    if (!file.exists) {
        o3.print("file not found: " + file.path);
        return false;
    }
    return true;
}

var builder = o3.resourceBuilder();
for (var v = 0; v<src.length; v++) {
	checkFile(src[v]);	
	if (src[v].name.indexOf("plugin") >= 0) {
		builder.addAsResource(src[v], "npplugin.dll");
	}
	else
		builder.addAsResource(src[v]);
}

checkFile(tgt);
builder.buildAndAppend(tgt); 