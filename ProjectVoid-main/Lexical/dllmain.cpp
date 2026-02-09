#include <Windows.h>
#include "Client/Client.h"
#include "Renderer/D2D.h"
#include "Renderer/MCR.h"

DWORD WINAPI initClient(LPVOID lpParameter) {
    Client::init();
    
    while (Client::isInitialized()) {
        ModuleManager::onClientTick();
        Sleep(50);
    }

    Sleep(25);

    FreeLibraryAndExitThread((HMODULE)lpParameter, 1);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)initClient, hModule, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        HookManager::shutdown();
        ModuleManager::shutdown();
        CommandManager::shutdown();
        D2D::Clean();

        Client::DisplayClientMessage("%sEjected", MCTF::RED);
        break;
    }
    return TRUE;
}

