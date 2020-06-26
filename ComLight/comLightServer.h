#pragma once
#include "comLightClient.h"

#include "server/ObjectRoot.hpp"
#include "server/interfaceMap.h"
#include "server/Object.hpp"

#ifdef _MSC_VER
// On Windows, it's controlled by library.def module definition file. There's __declspec(dllexport), but it adds underscore, I don't like that.
#define DLLEXPORT extern "C"
#else
#define DLLEXPORT extern "C" __attribute__((visibility("default")))
#endif