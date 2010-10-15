call cd settingsFF
call "../../../tools/win32/zip" -J -r o3.xpi ./*
call mv o3.xpi ../o3.xpi
call cd ..