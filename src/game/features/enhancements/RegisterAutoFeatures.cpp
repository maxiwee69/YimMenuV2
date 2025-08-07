#include "AutoFeatures.hpp"
#include "core/commands/Commands.hpp"

namespace YimMenu::Features
{
    // Global instances for auto features
    static AutoHeal g_AutoHeal;
    static AutoArmor g_AutoArmor;
    static AutoRepair g_AutoRepair;
    static AutoClearWanted g_AutoClearWanted;
    static AutoPassive g_AutoPassive;

    // Function to register all auto features
    void RegisterAutoFeatures()
    {
        // Auto features are automatically registered through their constructors
        // since they inherit from LoopedCommand
    }
}