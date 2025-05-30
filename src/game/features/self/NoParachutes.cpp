#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
    class NoParachutes : public LoopedCommand
    {
        using LoopedCommand::LoopedCommand;

        virtual void OnTick() override
        {
            // Remove any reserve parachute
            // There's no clear native, so just keep removing parachute weapon
            if (auto ped = Self::GetPed())
            {
                if (ped.HasWeapon("GADGET_PARACHUTE"_J))
                    ped.RemoveWeapon("GADGET_PARACHUTE"_J);
            }
        }
    };

    // Static instance of the feature
    static NoParachutes _NoParachutes{"noparachutes", "No Parachutes", "Never have a parachute"};
}
