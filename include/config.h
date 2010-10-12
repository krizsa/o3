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
#ifndef O3_CONFIG_H
#define O3_CONFIG_H

#define __O3__

#if defined(__APPLE__)
#define O3_APPLE
#elif defined(__linux__)
#define O3_LINUX
#elif defined(_WIN32)
#define O3_WIN32
#elif defined(__CYGWIN__)
#define O3_LINUX
#endif

#define O3_AUTO_CAPACITY    11
#define O3_TICK_SIZE        10
#define O3_CLS_TRAIT_COUNT  100
#define O3_EXT_TRAIT_COUNT  100
#define O3_VALUE_OFFSET     1000

#if defined(O3_APPLE)
#define O3_POSIX
#elif defined(O3_LINUX)
#define O3_POSIX
#elif defined(O3_WIN32)
#define _WIN32_DCOM
#define _CRT_SECURE_NO_WARNINGS
#endif

#define O3_DEBUG

#if defined(O3_DEBUG)
#define O3_TRACE    0
#define O3_ASSERT
#define O3_LOG
#endif

#define O3_VERSION 0.9
#define O3_VERSION_STRING "v0_9"
#define O3_AJAX_DOMAIN "www.ajax.org"

#define O3_BASE_URL "http://www.ajax.org/o3test"
//"http://127.0.0.1:8000"

#define O3_UI_URL "http://www.ajax.org/o3test"

#ifdef O3_WIN32
#define O3_PLATFORM "win"
#define O3_PLUGIN_NAME
#define O3_PLUGIN_UPDATER "o3update.exe"
#define O3_PLUGIN_INSTALLER "o3plugin-win32d.exe"
#define O3_PLUGIN_VERSION "o3plugin-win32d.exe.version"
#endif

#ifdef O3_APPLE
#define O3_PLATFORM "osx"
#define O3_PLUGIN_NAME "npplugin"
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

#endif // O3_CONFIG_H
