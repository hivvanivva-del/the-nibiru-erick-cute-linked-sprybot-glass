#include "Franky.h"
/* joke module from impact lol */
Franky::Franky() : Module("Franky", "It does exactly what you think it does.", Category::WORLD) {
	registerSetting(new BoolSetting("Max Exploit", "Maximum exploitation.", &maxExploit, false));
}