#include "MemoryUtil.h"
#include "Logger.h"
#include "../Libs/Obfuscator.h"
#include <libhat/Scanner.hpp>
#include <atomic>

uintptr_t MemoryUtil::getGameAddress() {
	static uintptr_t address = reinterpret_cast<uintptr_t>(GetModuleHandleA("Minecraft.Windows.exe"));
	return address;
}

uintptr_t MemoryUtil::findSignature(std::string_view sig) {
	auto address = hat::parse_signature(sig);

	if (!address.has_value()) {
		logF("Sig dead: %s", sig.data())
		return 0;
	}

	auto result = hat::find_pattern(address.value());

	if (result.has_result())
		return reinterpret_cast<uintptr_t>(result.get());

	logF("Sig dead: %s", sig.data());
	return 0;
}

uintptr_t** MemoryUtil::getVtableFromSig(std::string_view sig) {
	uintptr_t address = findSignature(sig);

	if (address == 0x0) 
		return nullptr;

	int finalOffset = *reinterpret_cast<int*>((address + 3));
	return reinterpret_cast<uintptr_t**>(address + finalOffset + 7);
}

std::string MemoryUtil::getHWID() {
	HKEY hKey;
	char value[255];
	DWORD value_length = sizeof(value);
	std::string res = "";

	// Usamos OBF_STR para ofuscar rutas de registro
	const char* regPath = OBF_STR("SOFTWARE\\Microsoft\\Cryptography");
	const char* regVal = OBF_STR("MachineGuid");

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath, 0, KEY_READ | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {
		if (RegQueryValueExA(hKey, regVal, NULL, NULL, (LPBYTE)&value, &value_length) == ERROR_SUCCESS) {
			res = value;
		}
		RegCloseKey(hKey);
	}
	
	// Barrera anti-análisis
	std::atomic_signal_fence(std::memory_order_seq_cst);
	
	return res;
}