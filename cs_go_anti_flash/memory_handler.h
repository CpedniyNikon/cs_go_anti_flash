#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
struct PModule
{
	DWORD dwBase;
	DWORD dwSize;
};

class Memory
{
public:
	void attach(const wchar_t* processName)
	{
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 processEntry{ sizeof(processEntry) };
		Process32First(snapshot, &processEntry);
		do {

			if (lstrcmpiW(processEntry.szExeFile, processName) == 0)
			{
				_processId = processEntry.th32ProcessID;
				CloseHandle(snapshot);
				_process = OpenProcess(PROCESS_ALL_ACCESS, 0, _processId);
				std::cout << _processId << std::endl;
				return;
			}
		} while (Process32Next(snapshot, &processEntry));
		_process = nullptr;
	}

	PModule getModule(const wchar_t* moduleName)
	{
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _processId);
		MODULEENTRY32 moduleEntry{ sizeof(moduleEntry) };
		Module32First(snapshot, &moduleEntry);
		do {
			if (lstrcmpiW(moduleEntry.szModule, moduleName) == 0)
			{
				std::wcout << moduleEntry.szModule << std::endl;
				CloseHandle(snapshot);
				return PModule{ reinterpret_cast<DWORD>(moduleEntry.hModule), moduleEntry.modBaseSize };
			}
		} while (Module32Next(snapshot, &moduleEntry));

		return PModule{ 0, 0 };
	}

	template <class T>
	T read(const DWORD address)
	{
		T value;
		ReadProcessMemory(_process, LPVOID(address), &value, sizeof(T), 0);
		return value;
	}

	template <class T>
	void write(const DWORD address, const T value)
	{
		WriteProcessMemory(_process, LPVOID(address), &value, sizeof(T), 0);
	}

	~Memory()
	{
		CloseHandle(_process);
	}

private:
	HANDLE _process;
	DWORD _processId;
};