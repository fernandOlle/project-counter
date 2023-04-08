#include "memory.h"

#include <thread>
#include <iostream>
#include "memory.h"

namespace offset {
	// client
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEA964;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFFFC4;
	constexpr ::std::ptrdiff_t dwForceAttack = 0x322DDFC;

	// entity
	constexpr ::std::ptrdiff_t m_iHealth = 0x100;
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_iCrosshairId = 0x11838;
	constexpr ::std::ptrdiff_t m_bDormant = 0xED;
}

int main() {
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");
	
	std::cout << std::hex << "CS:GO memory address (tb) >> Ox" << client << std::dec << std::endl;

	// hack loop
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto& localPlayer = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& localHeath = memory.Read<std::int32_t>(localPlayer + offset::m_iHealth);

		// skip if trigger kry is not down
		// TODO: add toggle movement keys
		if (GetAsyncKeyState(0x57) || GetAsyncKeyState(0x41) || GetAsyncKeyState(0x53) || GetAsyncKeyState(0x44))
			if(!GetAsyncKeyState(VK_SHIFT))
				continue;

		// skip if local player is dead
		if (!localHeath)
		continue;

		const auto& crosshairId = memory.Read<std::int32_t>(localPlayer + offset::m_iCrosshairId);

		// skip if crosshair is not on enemy
		if (!crosshairId || crosshairId > 64)
			continue;

		const auto& player = memory.Read<std::uintptr_t>(client + offset::dwEntityList + (crosshairId - 1) * 0x10);

		// skip if player is dead
		if (!memory.Read<std::int32_t>(player + offset::m_iHealth))
			continue;

		// skip if player is dormant
		if (memory.Read<bool>(player + offset::m_bDormant))
			continue;

		// skip if player is on our team
		if (memory.Read<std::int32_t>(localPlayer + offset::m_iTeamNum) ==
			memory.Read<std::int32_t>(player + offset::m_iTeamNum))
			continue;

		 //shoot
		memory.Write<std::uintptr_t>(client + offset::dwForceAttack, 6);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		memory.Write<std::uintptr_t>(client + offset::dwForceAttack, 4);
		
	}
	return 0;
}