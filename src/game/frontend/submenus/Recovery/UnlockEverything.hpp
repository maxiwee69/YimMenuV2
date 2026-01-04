#pragma once
#include "core/frontend/manager/Category.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/features/recovery/UnlockEverything.hpp"

namespace YimMenu::Submenus
    {
    std::shared_ptr<Category> BuildUnlockEverythingMenu();
    }