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

var mode = o3.args[1];

function checkFile(file) {
    if (!file.exists) {    
        o3.print("file not found: " + file.path);
        return false;
    }
    return true;
}

var src = o3.cwd.get("prelude.js");
var tgt = o3.cwd.get("../../build/" + mode + "/sh/o3.exe");

checkFile(src);
checkFile(tgt);

var builder = o3.resourceBuilder();
builder.addAsResource(src);
builder.buildAndAppend(tgt); 