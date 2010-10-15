var fs = require('fs'),
	sys = require('sys');


var GUID = '8a66ecac-63fd-4afa-9d42-3034d18c88f4',  
BASENAME = 'o3', 
FULLNAME = BASENAME + '-' + GUID,
IEEXTGUID = '218BAC91-2A05-4372-BD7F-5AE59C149D3E',
CHROMEGUID = 'npaodepeeohfoeeglfcafddnjhejcgeg', 
FFEXTGUID = BASENAME + '@ajax.org',
DOMAIN = 'o3.ajax.org',
URL = 'http://' + DOMAIN,
VERSION = '0.9', 
VERSIONSTRING = 'v' + VERSION.replace('.','_'),
DESCRIPTION = 'Ajax.org o3 plugin', 
MIMEDESCRIPTION = 'Ajax.org o3 plugin', 
TYPEDESCRIPTION = 'Ajax.org o3 plugin',
COMPANY = 'Ajax.org',
FILEDESCRIPTION = 'Ajax.org o3 plugin',
IDENTIFIER = 'org.ajax.o3-plugin',
LEGALCOPYRIGHT = 'Copyright © 2008-2010 Ajax.org (http://www.ajax.org)',
OSXUPDATER = 'update_installer',
OSXINSTALLER = BASENAME + 'plugin-osx32.dmg', 
OSXVERSION = OSXINSTALLER + '.version', 
WINUPDATER = BASENAME + 'update.exe',
WININSTALLER = BASENAME + 'plugin-win32.exe',
WINVERSION = WININSTALLER + '.version',
GUIDDEF = '0x' + GUID.substring(0,8) + ',0x' + GUID.substring(9,13) 
     + ',0x' + GUID.substring(14,18) + ',{0x' + GUID.substring(19,21)
     + ',0x' + GUID.substring(21,23) + ',0x' + GUID.substring(24,26)
     + ',0x' + GUID.substring(26,28) + ',0x' + GUID.substring(28,30)
     + ',0x' + GUID.substring(30,32) + ',0x' + GUID.substring(32,34)
     + ',0x' + GUID.substring(34,36) + '}'; 

RegexpMap = [
[/#GUID#/g,GUID],  
[/#BASENAME#/g,BASENAME], 
[/#FULLNAME#/g,FULLNAME],
[/#IEEXTGUID#/g,IEEXTGUID],
[/#CHROMEGUID#/g,CHROMEGUID], 
[/#FFEXTGUID#/g,FFEXTGUID],
[/#DOMAIN#/g,DOMAIN],
[/#URL#/g,URL],
[/#VERSION#/g,VERSION], 
[/#VERSIONSTRING#/g,VERSIONSTRING],
[/#DESCRIPTION#/g,DESCRIPTION], 
[/#MIMEDESCRIPTION#/g,MIMEDESCRIPTION], 
[/#TYPEDESCRIPTION#/g,TYPEDESCRIPTION],
[/#COMPANY#/g,COMPANY],
[/#FILEDESCRIPTION#/g,FILEDESCRIPTION],
[/#LEGALCOPYRIGHT#/g,LEGALCOPYRIGHT],
[/#IDENTIFIER#/g,IDENTIFIER],
[/#OSXUPDATER#/g,OSXUPDATER],
[/#OSXINSTALLER#/g,OSXINSTALLER],
[/#OSXVERSION#/g,OSXVERSION],
[/#WINUPDATER#/g,WINUPDATER],
[/#WININSTALLER#/g,WININSTALLER],
[/#WINVERSION#/g,WINVERSION],
[/#GUIDDEF#/g, GUIDDEF]
],

files = [
	'hosts/installer/installer.js',
	'hosts/plugin/Info.plist',
	'hosts/plugin/plugin.r',
	'hosts/plugin/plugin.rc',
	'include/guid.h',
	'include/app_data.h',	
	'hosts/installer/osx/distribution.dist'
];

function readFile(file) {
    var size = fs.statSync(file).size,
		buf = new Buffer(size),
		fd = fs.openSync(file, 'r');
	if (!size)
		return "";
	fs.readSync(fd, buf, 0, size, 0);
	fs.closeSync(fd);
	return buf.toString();
}

function writeFile(file, data) {
    var size = data.length,
		buf = new Buffer(data),
		fd = fs.openSync(file, 'w');
		
	fs.writeSync(fd, buf, 0, size, 0);
	fs.closeSync(fd);
}

function patchFile(file) {
	var data = readFile(file + '.template');
	for (var v=0; v < RegexpMap.length; v++) 
		data = data.replace(RegexpMap[v][0],RegexpMap[v][1]);

	writeFile(file, data);	
}

for (var f=0; f < files.length; f++) 
	patchFile(files[f]);
	