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

#include "ScriptEngine.h"
#include "../interface/Console.h"
#include <dukglue/dukglue.h>
#include <duktape.h>
#include <iostream>
#include <stdexcept>

#include "ScConsole.hpp"

using namespace OpenRCT2::Scripting;

static std::string Stringify(duk_context * ctx, duk_idx_t idx);

ScriptEngine::ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env) :
    _console(console),
    _env(env)
{
}

ScriptEngine::~ScriptEngine()
{
    duk_destroy_heap(_context);
}

void ScriptEngine::Initialise()
{
    _context = duk_create_heap_default();
    if (_context == nullptr)
    {
        throw std::runtime_error("Unable to initialise duktape context.");
    }

    auto ctx = _context;
    ScConsole::Register(ctx);

    auto scConsole = std::make_shared<ScConsole>(_console);
    dukglue_register_global(ctx, scConsole, "console");
}

void ScriptEngine::Update()
{
    if (!_initialised)
    {
        Initialise();
        _initialised = true;
    }
    while (_evalQueue.size() > 0)
    {
        auto item = std::move(_evalQueue.front());
        _evalQueue.pop();
        auto promise = std::move(std::get<0>(item));
        auto command = std::move(std::get<1>(item));
        if (duk_peval_string(_context, command.c_str()) != 0)
        {
            std::string result = std::string(duk_safe_to_string(_context, -1));
            _console.WriteLineError(result);
        }
        else
        {
            std::string result = Stringify(_context, -1);
            _console.WriteLine(result);
        }
        duk_pop(_context);
        // Signal the promise so caller can continue
        promise.set_value();
    }
}

std::future<void> ScriptEngine::Eval(const std::string &s)
{
    std::promise<void> barrier;
    auto future = barrier.get_future();
    _evalQueue.emplace(std::move(barrier), s);
    return future;
}

static std::string Stringify(duk_context * ctx, duk_idx_t idx)
{
    auto type = duk_get_type(ctx, idx);
    if (type == DUK_TYPE_OBJECT && !duk_is_function(ctx, idx))
    {
        return duk_json_encode(ctx, idx);
    }
    else
    {
        return duk_safe_to_string(ctx, idx);
    }
}
