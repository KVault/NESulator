#include "gui.h"

ROM rom;

//library to be registered
static const struct luaL_Reg GUIulator[] = {
		{"get_cycle_count", get_cycle_count},
		{"stop_emulation", stop_emulation},
		{"open_rom", open_rom},
		{NULL, NULL}  /* sentinel */
};

//name of this function is not flexible
int luaopen_libGUItulator (lua_State *L){
	luaL_register(L,"libGUItulator", GUIulator);
	return 1;
}

static int get_cycle_count(lua_State *state) {
	lua_pushnumber(state, cpu_cyclesLastSec);
	return 1;// number of results;
}

static int open_rom(lua_State *state){
	rom = *insertCartridge("C:\\dev\\NESulator\\NESulator\\rom\\donkey_kong.nes");

	if(rom.numPRGPages == 0){ //If this is 0 we can assume the ROM is wrong...
		log_error("An unknown error happened when trying to open the ROM. Do something about it!");
		return 0;
	}

	configure();
	load_ROM(&rom);
	start_emulation();

	return 0;
}

static int stop_backgroumd_emulation(lua_State *state){
	stop_emulation();
	return 0;
}
