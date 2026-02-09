#pragma once
#include "../../ModuleBase/Module.h"

class AntiDesync : public Module {
public:
	AntiDesync() : Module("AntiDesync", "Prevents eating failure.", Category::MISC) {

	}
};