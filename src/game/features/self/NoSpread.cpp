#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
    class NoSpread : public LoopedCommand
    {
        using LoopedCommand::LoopedCommand;

        int m_originalAccuracy = 0;
        bool m_initialized = false;

        virtual void OnTick() override
        {
            auto ped = Self::GetPed();
            if (!ped)
                return;

            if (!m_initialized)
            {
                m_originalAccuracy = ped.GetAccuracy();
                m_initialized = true;
            }

            ped.SetAccuracy(100);
        }

        virtual void OnDisable() override
        {
            auto ped = Self::GetPed();
            if (ped && m_initialized)
            {
                ped.SetAccuracy(m_originalAccuracy);
            }
            m_initialized = false;
        }
    };

    static NoSpread _NoSpread{"nospread", "No Spread", "Eliminates bullet spread when shooting"};
}
