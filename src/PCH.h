#pragma once

#include <SKSE/SKSE.h>
#include <RE/Skyrim.h>

#include "TSingleton.h"
#include "RE/SkyrimEx.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

namespace WinAPI = SKSE::WinAPI;
namespace logger = SKSE::log;
namespace fs = std::filesystem;
namespace stl = SKSE::stl;
using namespace std::literals;
