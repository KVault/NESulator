#include "../utils/log.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int main(){
	lua_State *L;
	L = luaL_newstate();
	luaL_openlibs(L);

	int status = luaL_loadfile(L, "lua/impl/MainWindow.lua");
	if(status){
		//If something went wrong the message is at the top of the stack
		printf("Couldn't load file: %s\n", lua_tostring(L, -1));
		return 1;
	}

	if(lua_pcall(L, 0, 0, 0))
	{
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
	}

	lua_close(L);	//Cya, lua

	return 0;
}