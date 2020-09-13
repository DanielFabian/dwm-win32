#include "display.h"

#include <stdint.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <windows.h>

static int moddisplay_getDisplays(lua_State *L);

static const struct  luaL_reg dwmdisplaymod[] = {
	{ "getDisplays", moddisplay_getDisplays },
	{ NULL, NULL }
};

int dwmmod_opendisplay(lua_State *L) {
	luaL_register(L, "dwm.display", dwmdisplaymod);
	return 1;
}

static int moddisplay_getDisplays(lua_State *L) {
	lua_newtable(L);

	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);

	DWORD devicenum = 0;

	DEVMODE dm;

	uint32_t index = 0;

	while(EnumDisplayDevices(NULL, devicenum, &dd, 0)) {
		if (!(dd.StateFlags && DISPLAY_DEVICE_ACTIVE)) continue;
		DISPLAY_DEVICE newdd = {0};
		newdd.cb = sizeof(DISPLAY_DEVICE);
		DWORD monitornum = 0;
		while(EnumDisplayDevices(dd.DeviceName, monitornum, &newdd, 0)) {
			lua_pushnumber(L, ++index);
			lua_newtable(L);

			lua_pushstring(L, "id");
			lua_pushstring(L, newdd.DeviceKey);
			lua_settable(L, -3);

			dm.dmSize = sizeof(dm);
			dm.dmScale = sizeof(dm);
			dm.dmDriverExtra = 0;

			if (EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm)) {
				lua_pushstring(L, "x");
				lua_pushnumber(L, dm.dmPosition.x);
				lua_settable(L, -3);

				lua_pushstring(L, "y");
				lua_pushnumber(L, dm.dmPosition.y);
				lua_settable(L, -3);

				lua_pushstring(L, "width");
				lua_pushnumber(L, dm.dmPelsWidth);
				lua_settable(L, -3);

				lua_pushstring(L, "height");
				lua_pushnumber(L, dm.dmPelsHeight);
				lua_settable(L, -3);
			}

			lua_settable(L, -3);

			++monitornum;
		}
		++devicenum;
	}

	return 1; /* number of results */
}