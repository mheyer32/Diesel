/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer
*/
#pragma once

#include "sound/SoundDevice.h"

#include <string>

SampleBufferPtr loadWAV(SoundDevice& device, const std::string& filename);
