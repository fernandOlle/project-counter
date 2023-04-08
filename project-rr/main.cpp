#include "memory.h"

#include <thread>
#include <iostream>
#include "memory.h"
#include "memory.h"

namespace offset {
	// client
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEA964;
	constexpr ::std::ptrdiff_t dwClientState = 0x59F19C;
	constexpr ::std::ptrdiff_t dwClientState_ViewAngles = 0x4D90;

	// entity
	constexpr ::std::ptrdiff_t m_aimPunchAngle = 0x303C;
	constexpr ::std::ptrdiff_t m_iShotsFired = 0x103E0;
}

struct Vector2 {
	float x = { }, y = { };
};

int main() {
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");

	std::cout << std::hex << "CS:GO memory address (rr) >> Ox" << client << std::dec << std::endl;

	auto oldPunch = Vector2();

	// hack loop
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto& localPlayer = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& shotsFired = memory.Read<std::int32_t>(localPlayer + offset::m_iShotsFired);

		if (shotsFired) 
			const auto& clientState = memory.Read<std::uintptr_t>(engine + offset::dwClientState);
			const auto& viewAngles = memory.Read<Vector2>(clientState + offset::dwClientState_ViewAngles);
			const auto& aimPunch = memory.Read<Vector2>(localPlayer + offset::m_aimPunchAngle);

			// create the new viewAngle
			auto newAngles = Vector2{
				viewAngles.x + oldPunch.x - aimPunch.x * 2.f,
				viewAngles.y + oldPunch.y - aimPunch.y * 2.f,
			};

			// reset X view angle, so it doesnt pass the games limit
			if (newAngles.x > 89.f)
				newAngles.x = 89.f;

			if (newAngles.x < -89.f)
				newAngles.x = -89.f;

			// reset Y view angle, so it doesnt pass the games limit 
			while (newAngles.y > 180.f)
				newAngles.y -= 360.f;

			while (newAngles.y < -180.f)
				newAngles.y += 360.f;

			// write to memory the fixed view angle consiering aimPunch
			memory.Write<Vector2>(clientState + offset::dwClientState_ViewAngles, newAngles);

			// save the current view angle
			oldPunch.x = aimPunch.x * 2.f;
			oldPunch.y = aimPunch.y * 2.f;

		}
		else {
			oldPunch.x = oldPunch.y = 0.f;
		}

	}
	return 0;
}