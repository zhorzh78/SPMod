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

#pragma once

#include "ExtMain.hpp"

namespace SPExt::Listener
{
    int Forward(const SPMod::IForward *const fwd, int *result, bool *stop);
    void Cvar(const ICvar *const cvar, const char *old_value, const char *new_value);
    void Menu(SPMod::IMenu *const menu, SPMod::IMenuItem *const item, SPMod::IPlayer *const player);
    void MenuText(SPMod::IMenu *const menu, int key, SPMod::IPlayer *const player);
    ItemStatus MenuItemCallback(SPMod::IMenu *const menu, SPMod::IMenuItem *const item, SPMod::IPlayer *const player);
}
