#include "stdafx.h"


#include <atlstr.h> // CW2A
#include <PhysicalMonitorEnumerationAPI.h>
#include <LowLevelMonitorConfigurationAPI.h>

#include <iostream>
#include <vector>
#include <string>

std::vector<PHYSICAL_MONITOR> physicalMonitors{};

BOOL CALLBACK monitorEnumProcCallback(HMONITOR hMonitor, HDC hDeviceContext, LPRECT rect, LPARAM data)
{
	DWORD numberOfPhysicalMonitors;
	BOOL success = GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &numberOfPhysicalMonitors);
	if (success) {
		auto originalSize = physicalMonitors.size();
		physicalMonitors.resize(physicalMonitors.size() + numberOfPhysicalMonitors);
		success = GetPhysicalMonitorsFromHMONITOR(hMonitor, numberOfPhysicalMonitors, physicalMonitors.data() + originalSize);
	}
	return true;
}

std::string toUtf8(wchar_t *buffer)
{
	CW2A utf8(buffer, CP_UTF8);
	const char* data = utf8.m_psz;
	return std::string{ data };
}

void detect()
{
	int i = 0;
	for (auto &physicalMonitor : physicalMonitors) {
		std::cout << "Display " << i << std::endl;
		std::cout << "\t" << " Name: " << toUtf8(physicalMonitor.szPhysicalMonitorDescription) << std::endl;
		++i;
	}
}

void capabilities() {
	for (auto &physicalMonitor : physicalMonitors) {
		auto physicalMonitorHandle = physicalMonitor.hPhysicalMonitor;

		DWORD capabilitiesStringLengthInCharacters;
		auto success = GetCapabilitiesStringLength(physicalMonitorHandle, &capabilitiesStringLengthInCharacters);
		if (!success) {
			std::cerr << "Failed to get capabilities string length" << std::endl;
			continue;
		}

		std::unique_ptr<CHAR[]> capabilitiesString{ new CHAR[capabilitiesStringLengthInCharacters] };
		success = CapabilitiesRequestAndCapabilitiesReply(physicalMonitorHandle, capabilitiesString.get(), capabilitiesStringLengthInCharacters);
		if (!success) {
			std::cerr << "Failed to get capabilities string" << std::endl;
			continue;
		}

		std::cout << std::string(capabilitiesString.get()) << std::endl;
	}
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	std::vector<std::string> args;
	for (int i = 0; i < argc; i++) {
		std::string utf8Arg = toUtf8(argv[i]);
		args.emplace_back(utf8Arg);
	}

	EnumDisplayMonitors(NULL, NULL, &monitorEnumProcCallback, 0);
	detect();
	capabilities();
	
	return 0;
}

