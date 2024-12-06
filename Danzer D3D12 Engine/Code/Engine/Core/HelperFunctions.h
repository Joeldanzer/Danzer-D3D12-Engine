#pragma once

#include <windows.h>
#include <stdafx.h>
#include <string>

static bool RequestAllocation(const uint64_t memoryAmount) {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&statex);

	const uint64_t amoutInMB = memoryAmount / (1024 * 1024);
#ifdef _DEBUG
	std::cout << "Trying to request " << amoutInMB << "MB's. "<< std::endl << "Available memory left " << statex.ullAvailPhys / (1024 * 1024) << " MB's " <<std::endl;
#endif

	if (memoryAmount < statex.ullAvailPhys) {
#ifdef _DEBUG
		std::cout << "Memory request of " << amoutInMB << "MB's Succesful!" << std::endl;
#endif
		return true;
	}

#ifdef DEBUG
	std::cout << "ERROR: Memory request of " << amoutInMB << "MB's Failed!" << std::endl;
#endif
	
	return false;
}
