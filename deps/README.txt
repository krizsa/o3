O3 Dependencies,

We have stripped the opensource projects in here of any files not needed to build O3 (examples, etc)
All license files and README's for these projects have been left in place.

Almost all dependencies are included using a full header approach (#include) except for LibXML and Curl.
These libs are almost always present on unix machines,
but not on windows so we have provided prebuilt binaries for these libs in win32lib.
