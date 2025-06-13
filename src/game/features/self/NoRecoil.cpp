#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
    class NoRecoil : public LoopedCommand
    {
        using LoopedCommand::LoopedCommand;

        float m_originalMultiplier = 1.0f;
        bool m_initialized = false;

        virtual void OnTick() override
        {
            // Disable recoil by setting multiplier to 0.0
            if (!m_initialized)
            {
                m_originalMultiplier = Natives::GET_WEAPON_RECOIL_MULTIPLIER(); // placeholder
                m_initialized = true;
            }
            Natives::SET_PED_WEAPON_RECOIL_MULTIPLIER(PLAYER::PLAYER_PED_ID(), 0.0f);
        }

        virtual void OnDisable() override
        {
            if (m_initialized)
            {
                Natives::SET_PED_WEAPON_RECOIL_MULTIPLIER(PLAYER::PLAYER_PED_ID(), m_originalMultiplier);
                m_initialized = false;
            }
        }
    };

    static NoRecoil _NoRecoil{"norecoil", "No Recoil", "Removes weapon recoil for the local player"};
}
