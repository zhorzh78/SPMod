/*
 *  Copyright (C) 2018 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ExtMain.hpp"

TypeHandler<SPMod::IForward> gForwardHandlers;

// Forward(const char[] name, ExecType exectype, const char[] plgname, ParamType ...)
static cell_t ForwardCtor(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { args_num = 0, arg_name, arg_exec, arg_pluginname, arg_paramstypes };

    auto execType = static_cast<IForward::ExecType>(params[arg_exec]);
    SPMod::IForwardMngr *fwdMngr = gSPGlobal->getForwardManager();

    char *fwdName;
    ctx->LocalToString(params[arg_name], &fwdName);

    std::size_t fwdParamsNum = params[args_num] - 3;
    if (fwdParamsNum > SP_MAX_EXEC_PARAMS)
        return -1;

    std::array<IForward::ParamType, SP_MAX_EXEC_PARAMS> fwdParamsList;
    for (std::size_t i = 0; i < fwdParamsNum; ++i)
    {
        cell_t *paramType;
        ctx->LocalToPhysAddr(params[i + arg_paramstypes], &paramType);
        fwdParamsList.at(i) = static_cast<IForward::ParamType>(*paramType);
    }

    // Created forward
    SPMod::IForward *forward;

    // If single forward search for shared_ptr of that plugin
    char *pluginName;
    ctx->LocalToStringNULL(params[arg_pluginname], &pluginName);
    if (pluginName)
    {
        if (strlen(pluginName) < 4)
        {
            ctx->ReportError("Invalid plugin name");
            return 0;
        }

        std::string_view pluginNameExt(pluginName);
        pluginNameExt = pluginNameExt.substr(pluginNameExt.length() - 4);

        IPlugin *plugin;
        // If it's SourcePawn plugin lets find it here without involving core
        if (pluginNameExt == PluginMngr::pluginsExtension)
            plugin = gModuleInterface->getPluginMngrCore()->getPluginCore(pluginName).get();
        else
            plugin = gSPGlobal->getPlugin(pluginName);

        if (!plugin)
        {
            ctx->ReportError("Plugin not found");
            return -1;
        }

        forward = fwdMngr->createForward(fwdName, pluginName, fwdParamsNum, fwdParamsList.data());
    }
    else
        forward = fwdMngr->createForward(fwdName, execType, fwdParamsNum, fwdParamsList.data());

    if (!forward)
    {
        ctx->ReportError("Forward could not be created");
        return -1;
    }

    return gForwardHandlers.create(forward);
}

// bool PushCell(any cell)
static cell_t PushCell(SourcePawn::IPluginContext *ctx,
                       const cell_t *params)
{
    enum { arg_id = 1, arg_cell };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }
    
    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    return forward->pushCell(params[arg_cell]);
}

// bool PushCellRef(any &cell)
static cell_t PushCellRef(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_id = 1, arg_cellref };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    cell_t *value;
    ctx->LocalToPhysAddr(params[arg_cellref], &value);

    return forward->pushCellPtr(value, true);
}

// bool PushFloat(float real)
static cell_t PushFloat(SourcePawn::IPluginContext *ctx,
                        const cell_t *params)
{
    enum { arg_id = 1, arg_float };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    return forward->pushFloat(sp_ctof(params[arg_float]));
}

// bool PushFloatRef(float &real)
static cell_t PushFloatRef(SourcePawn::IPluginContext *ctx,
                           const cell_t *params)
{
    enum { arg_id = 1, arg_floatref };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    union
    {
        cell_t *cellptr;
        float *floatptr;
    } floatHolder;

    ctx->LocalToPhysAddr(params[arg_floatref], &floatHolder.cellptr);

    return forward->pushFloatPtr(floatHolder.floatptr, true);
}

// bool PushString(const char[] string)
static cell_t PushString(SourcePawn::IPluginContext *ctx,
                         const cell_t *params)
{
    enum { arg_id = 1, arg_string };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    char *string;
    ctx->LocalToString(params[arg_string], &string);

    return forward->pushString(string);
}

// bool PushArray(const any[] array, int size)
static cell_t PushArray(SourcePawn::IPluginContext *ctx,
                        const cell_t *params)
{
    enum { arg_id = 1, arg_array, arg_size };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    cell_t *array;
    ctx->LocalToPhysAddr(params[arg_array], &array);

    return forward->pushArray(array, params[arg_size], false);
}

// bool PushStringEx(char[] string, int length, StringFlags sflags, bool copyback)
static cell_t PushStringEx(SourcePawn::IPluginContext *ctx,
                           const cell_t *params)
{
    enum { arg_id = 1, arg_string, arg_length, arg_sflags, arg_copyback };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    char *string;
    ctx->LocalToString(params[arg_string], &string);
    auto sflags = static_cast<IForward::StringFlags>(params[arg_sflags]);

    return forward->pushStringEx(string, params[arg_length], sflags, params[arg_copyback]);
}

// bool PushArrayEx(any[] array, int size, int cpflags)
static cell_t PushArrayEx(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_id = 1, arg_array, arg_size, arg_cpflags };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    cell_t *array;
    ctx->LocalToPhysAddr(params[arg_array], &array);

    return forward->pushArray(array, params[arg_size], params[arg_cpflags]);
}

// bool PushExec(any &result = 0)
static cell_t PushExec(SourcePawn::IPluginContext *ctx,
                       const cell_t *params)
{
    enum { arg_id = 1, arg_resultref };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    cell_t *retResult;
    ctx->LocalToPhysAddr(params[arg_resultref], &retResult);

    return forward->execFunc(retResult);
}

// bool PushCancel()
static cell_t PushCancel(SourcePawn::IPluginContext *ctx,
                         const cell_t *params)
{
    enum { arg_id = 1 };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
    {
        ctx->ReportError("Forward not found");
        return 0;
    }

    forward->resetParams();
    return 1;
}

// bool Remove()
static cell_t ForwardRemove(SourcePawn::IPluginContext *ctx,
                            const cell_t *params)
{
    enum { arg_id = 1 };

    cell_t fwdId = params[arg_id];
    if (fwdId == -1)
    {
        ctx->ReportError("Invalid forward");
        return 0;
    }

    SPMod::IForward *forward = gForwardHandlers.get(fwdId);
    if (!forward)
        return 0;

    if (forward->isExecuted())
        return 0;

    gSPGlobal->getForwardManager()->deleteForward(forward);
    gForwardHandlers.free(fwdId);
    return 1;
}

sp_nativeinfo_t gForwardsNatives[] =
{
    {  "Forward.Forward",        ForwardCtor         },
    {  "Forward.PushCell",       PushCell            },
    {  "Forward.PushCellRef",    PushCellRef         },
    {  "Forward.PushFloat",      PushFloat           },
    {  "Forward.PushFloatRef",   PushFloatRef        },
    {  "Forward.PushString",     PushString          },
    {  "Forward.PushArray",      PushArray           },
    {  "Forward.PushStringEx",   PushStringEx        },
    {  "Forward.PushArrayEx",    PushArrayEx         },
    {  "Forward.PushExec",       PushExec            },
    {  "Forward.PushCancel",     PushCancel          },
    {  "Forward.Remove",         ForwardRemove       },
    {  nullptr,                  nullptr             }
};
