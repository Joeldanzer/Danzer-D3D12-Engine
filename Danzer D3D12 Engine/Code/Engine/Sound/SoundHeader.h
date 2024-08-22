#pragma once

#include <Windows.h>

#include "Fmod/fmod.h"
#include "Fmod/fmod_common.h"
#include "Fmod/fmod_errors.h"


static void FmodResultCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		throw;
	}
}