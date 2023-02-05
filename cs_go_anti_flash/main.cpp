#include "memory_handler.h"
#include "offset.h"
#include <iostream>
#include <thread>

using namespace hazedumper;
using namespace netvars;
using namespace signatures;

PModule g_module;
Memory g_memory;

void remove_flash()
{
	auto localPlayer = g_memory.read<int32_t>(g_module.dwBase + dwLocalPlayer);
	g_memory.write<float>(localPlayer + m_flFlashDuration, 0.0f);
}


int main()
{
	g_memory.attach(L"csgo.exe");
	g_module = g_memory.getModule(L"client.dll");
	while (true)
	{
		remove_flash();
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
}



