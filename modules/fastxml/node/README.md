# node-o3-fastxml

This is a high speed W3C-DOM XML Library for NodeJS with XPath, (currently) without namespace support.
The library is an attempt to create a faster, less memory intensive XML library for node compared to node-o3-xml.
Since LibXML2 is a beast in terms of complexity and performance, and nodeJS is all about speed we decided to create another XML-DOM component, this time based on the (second) fastest XML parser in the world: PugiXML, which was chosen because the fastest one does not have XPath support and this is crucial for the DOM to be useful.

This library has not been tested as much yet as ajaxorg/node-o3-xml, however it is much faster and should use much less memory. What it currently lacks is DOM namespace suppport, and it will never support whitespaces in your XML tree as the standard requires. However for most actual uses of XML on the server this is perfectly acceptable.

If you need a tested XML library go use node-o3-xml, but if you want a much faster library try this one.
The API should be the identical W3C-DOM as in the browser and the node-o3-xml component.

To use this library simply clone the repo, and require('node-o3-fastxml') to return the parser object.
This repository is a generated build for node 0.2.2 stable, from the o3 repository (http://github.com/ajaxorg/o3)

Binaries included for:

* win32 (through cygwin)

* lin32 

* osx64

Other platforms and bit-ness (32/64) will be added to the automated build VM incrementally.
If you need to build this node-o3-xml module yourself or want to contribute to the source, please look at the main o3 repository.

If you are looking for the accompanying binary builds of NodeJS check out the 
(http://github.com/ajaxorg/node-builds) repository