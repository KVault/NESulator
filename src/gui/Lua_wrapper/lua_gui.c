#include "lua_gui.h"

ROM rom;
patterntable patterntable_backbuffer;

//library to be registered
static const struct luaL_Reg GUIulator[] = {
		{"get_cycle_count", get_cycle_count},
		{"stop_emulation", stop_emulation},
		{"open_rom", open_rom},
		{"get_patterntable", get_patterntable},
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

static int get_patterntable(lua_State *state){
	patterntable_backbuffer = *fill_patterntable();
	return 0;
}

/**
 * This is a ver y bad way of doing it. But we just need to get something showing up in Lua.
 *
 * TODO refactor this so that is uses userdata and metatables. Ideallu just one call to the C backend
 */
static int get_patterntable_pixel(lua_State *state){
	//The parameter is at the top of the stack
	int pos = (int) lua_tonumber(state, 1);
	/* This is pushed in order BGR, so it can be read as RGB, making it easier to read */
	for(int i = 2; i >= 0; --i){
		byte pixel_colour = patterntable_backbuffer.buffer[pos + i];
		lua_pushnumber(state, pixel_colour);
	}

	return 3;// amount of values in the stack. (in RGB order, ready to read)
}


static int stop_backgroumd_emulation(lua_State *state){
	stop_emulation();
	return 0;
}
