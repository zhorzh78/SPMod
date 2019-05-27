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

#include "spmod.hpp"

using namespace std::string_literals;

Extension::Extension(const fs::path &path)
{
#if defined SP_POSIX
    void *extHandle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
#else
    HMODULE extHandle = LoadLibrary(path.string().c_str());
#endif

    // Cannot load ext
    if (!extHandle)
    {
#if defined SP_POSIX
        throw std::runtime_error("Can't open extension: "s + path.filename().c_str());
#else
        throw std::runtime_error("Can't open extension: "s + path.filename().string());
#endif
    }

#if defined SP_POSIX
    m_queryFunc = reinterpret_cast<fnSPModQuery>(dlsym(extHandle, "SPMod_Query"));
#else
    m_queryFunc = reinterpret_cast<fnSPModQuery>(GetProcAddress(extHandle, "SPMod_Query"));
#endif

#if defined SP_POSIX
    m_initFunc = reinterpret_cast<fnSPModInit>(dlsym(extHandle, "SPMod_Init"));
#else
    m_initFunc = reinterpret_cast<fnSPModInit>(GetProcAddress(extHandle, "SPMod_Init"));
#endif

#if defined SP_POSIX
    m_endFunc = reinterpret_cast<fnSPModEnd>(dlsym(extHandle, "SPMod_End"));
#else
    m_endFunc = reinterpret_cast<fnSPModEnd>(GetProcAddress(extHandle, "SPMod_End"));
#endif

    m_extHandle = extHandle;
}

Extension::~Extension()
{
#if defined SP_POSIX
    dlclose(m_extHandle);
#else
    FreeLibrary(m_extHandle);
#endif
}

Extension::Status Extension::getStatus() const
{
    return m_status;
}

void Extension::setStatus(Status status)
{
    m_status = status;
}

fnSPModQuery Extension::getQueryFunc() const
{
    return m_queryFunc;
}

fnSPModInit Extension::getInitFunc() const
{
    return m_initFunc;
}

fnSPModEnd Extension::getEndFunc() const
{
    return m_endFunc;
}
