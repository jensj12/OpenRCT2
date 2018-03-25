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
#include "HookEngine.h"
#include "Plugin.h"
#include <future>
#include <memory>
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
    class ScriptExecutionInfo
    {
    private:
        std::shared_ptr<Plugin> _plugin;

    public:
        class PluginScope
        {
        private:
            ScriptExecutionInfo& _execInfo;
            std::shared_ptr<Plugin> _plugin;

        public:
            PluginScope(ScriptExecutionInfo& execInfo, std::shared_ptr<Plugin> plugin)
                : _execInfo(execInfo),
                  _plugin(plugin)
            {
                _execInfo._plugin = plugin;
            }
            PluginScope(const PluginScope&) = delete;
            ~PluginScope()
            {
                _execInfo._plugin = nullptr;
            }
        };

        std::shared_ptr<Plugin> GetCurrentPlugin() { return _plugin; }
    };

    class DukContext
    {
    private:
        duk_context * _context{};

    public:
        DukContext();
        DukContext(DukContext&) = delete;
        DukContext(DukContext&& src)
            : _context(std::move(src._context))
        {
        }
        ~DukContext();

        operator duk_context*() { return _context; }
    };

    class ScriptEngine
    {
    private:
        InteractiveConsole& _console;
        IPlatformEnvironment& _env;
        DukContext _context;
        bool _initialised{};
        std::queue<std::tuple<std::promise<void>, std::string>> _evalQueue;
        std::vector<std::shared_ptr<Plugin>> _plugins;
        uint32 _lastHotReloadCheckTick{};
        HookEngine _hookEngine;
        ScriptExecutionInfo _execInfo;

    public:
        ScriptEngine(InteractiveConsole& console, IPlatformEnvironment& env);
        ScriptEngine(ScriptEngine&) = delete;

        duk_context * GetContext() { return _context; }
        HookEngine& GetHookEngine() { return _hookEngine; }
        ScriptExecutionInfo& GetExecInfo() { return _execInfo; }

        void Update();
        std::future<void> Eval(const std::string &s);

    private:
        void Initialise();
        void LoadPlugins();
        void StartPlugins();
        void AutoReloadPlugins();
    };
}
