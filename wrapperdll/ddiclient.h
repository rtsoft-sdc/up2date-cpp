#pragma once
#include "ddi.hpp"
#include "dllexport.h"

namespace HkbClient {
	extern "C"
	{
		WDLL_EXPORT int __stdcall TestFunction1(int, int);
		WDLL_EXPORT void _stdcall BuildAndRun(void);
	}
}
