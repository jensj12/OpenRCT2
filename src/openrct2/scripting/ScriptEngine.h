#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"
#include <future>
#include <queue>
#include <string>

struct duk_hthread;
typedef struct duk_hthread duk_context;

class InteractiveConsole;

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

namespace OpenRCT2::Scripting
{
    class ScriptEngine
    {
    private:
        InteractiveConsole& _console;
        IPlatformEnvironment& _env;
        duk_context * _context;
        std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;

    public:
        ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env);
        ScriptEngine(ScriptEngine&&) = delete;
        ~ScriptEngine();

        void Update();
        std::future<void> Eval(const std::string &s);
    };
}