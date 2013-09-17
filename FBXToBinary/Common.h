///////////////////////////////////////////////////////////////////////////////////////
//
//	Common.h
//	Precompiled Header file which includes the fbxsdk, used stl headers, the logger,
//	useful typedefs, windows, and the chunk file writer.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <fbxsdk.h>
#include <vector>
#include <set>
#include <fstream>
#include <string>
#include <queue>
#include <cassert>

typedef unsigned int uint;
typedef unsigned char byte;
typedef unsigned int u32;
typedef float f32;
#include "ChunkFileWriter.h"
#include "Logger.h"

// prevent min/max from being #defined
#define NOMINMAX
// prevent infrequently used stuff
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>