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
 
var mode = process.argv.length > 2 ? process.argv[2] : false;
var args = [];
args.push(
	"-j",
	"resource.zip",
	"installer_header.bmp",
	"installer_left.bmp",
	mode ?"../../build/" + mode + "/plugin/npplugin.dll"
		: "../plugin/npplugin.dll",
	"installer.js",
	"install.ico",
	"o3settings.crx",
	"o3.xpi"
);

var tgt = mode ? "../../build/" + mode + "/installer/o3plugin.exe" 
	: "o3plugin.exe";
	
var sys   = require('sys'),
	fs = require('fs'),
	s = require('child_process'),
    child;	

var p = s.spawn("../../tools/win32/zip", args);

p.stdout.on('data',function(data){
	sys.puts(data);
})	
	
	
p.stderr.on('data',function(data){
	sys.puts(data);
})	
	

p.on('exit', function (code) {
    if (code !== 0) {
      console.log('exec error: ' + code);
    }
	
	var	buf1 = new Buffer([parseInt('0x01',16), parseInt('0x5c', 16), parseInt('0xbc', 16), parseInt('0x00', 16)]),
		fd_tgt = fs.openSync(__dirname + "/" + tgt, 'a+'),
		size = fs.statSync(__dirname + "/" + tgt).size,	
		fd_data = fs.openSync(__dirname + "/resource.zip", 'r'),
		data_size = fs.statSync(__dirname + "/resource.zip").size, 
		data_buf = new Buffer(data_size),
		a, b, c, d;
	
	fs.readSync(fd_data, data_buf, 0, data_size, 0),
	
	a = size & parseInt('0xff',16);
	b = size & parseInt('0xff00',16);
	c = size & parseInt('0xff0000',16);
	d = size & parseInt('0xff000000',16); 	

	var buf2 = new Buffer([a,b/parseInt('0x100',16),c/parseInt('0x10000',16),d/parseInt('0x1000000',16)])
	
	fs.writeSync(fd_tgt, buf1, 0, 4, null);
	fs.writeSync(fd_tgt, data_buf, 0, data_size, null);
	fs.writeSync(fd_tgt, buf2, 0, 4, null);
	fs.closeSync(fd_tgt);
	fs.closeSync(fd_data);
});

	
	
	
	
	
	