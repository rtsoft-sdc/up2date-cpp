#pragma once
#include "../ddi/include/ddi.hpp"

namespace HkbClient {
	extern "C"
	{
		__declspec(dllexport) int __stdcall TestFunction1(int, int);
		__declspec(dllexport) void _stdcall BuildAndRun(void);
	}
}
