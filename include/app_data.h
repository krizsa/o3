#define O3_VERSION "0.9"
#define O3_VERSION_STRING "v0_9"
#define O3_AJAX_DOMAIN "o3.ajax.org"

#define O3_BASE_URL "http://o3.ajax.org"

#define O3_UI_URL "http://o3.ajax.org"

#ifdef O3_WIN32
#define O3_PLATFORM "win"
#define O3_PLUGIN_NAME
#define O3_PLUGIN_UPDATER "o3update.exe"
#define O3_PLUGIN_INSTALLER "o3plugin-win32.exe"
#define O3_PLUGIN_VERSION "o3plugin-win32.exe.version"
#endif

#ifdef O3_APPLE
#define O3_PLATFORM "osx"
#define O3_PLUGIN_NAME "o3"
#define O3_PLUGIN_UPDATER "update_installer"
#define O3_PLUGIN_INSTALLER "o3plugin-osx32.dmg"
#define O3_PLUGIN_VERSION "o3plugin-osx32.dmg.version"
#endif

#ifdef O3_LINUX
#define O3_PLATFORM "linux"
#endif

#ifdef O3_POSIX
#define O3_PATH_TMP "/usr/tmp/o3"
#define O3_PATH_LIB "/usr/lib/o3"
#define O3_PATH_ETC "/etc/o3"
#endif