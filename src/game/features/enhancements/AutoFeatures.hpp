#pragma once
#include "core/commands/LoopedCommand.hpp"
#include <chrono>

namespace YimMenu::Features
{
    // Auto-Health: Automatically heal when health gets low
    class AutoHeal : public LoopedCommand
    {
    public:
        AutoHeal();
        virtual void OnTick() override;
        
    private:
        float m_HealthThreshold = 50.0f; // Heal when below 50%
    };

    // Auto-Armor: Automatically refill armor when it gets low
    class AutoArmor : public LoopedCommand
    {
    public:
        AutoArmor();
        virtual void OnTick() override;
        
    private:
        float m_ArmorThreshold = 30.0f; // Refill when below 30%
    };

    // Auto-Repair: Automatically repair vehicle when damaged
    class AutoRepair : public LoopedCommand
    {
    public:
        AutoRepair();
        virtual void OnTick() override;
        
    private:
        float m_DamageThreshold = 800.0f; // Repair when health below 800
        std::chrono::steady_clock::time_point m_LastRepair;
        std::chrono::milliseconds m_RepairCooldown{5000}; // 5 second cooldown
    };

    // Auto-Wanted: Automatically clear wanted level
    class AutoClearWanted : public LoopedCommand
    {
    public:
        AutoClearWanted();
        virtual void OnTick() override;
        
    private:
        std::chrono::steady_clock::time_point m_LastClear;
        std::chrono::milliseconds m_ClearDelay{2000}; // 2 second delay
    };

    // Auto-Passive: Automatically enable passive mode in populated sessions
    class AutoPassive : public LoopedCommand
    {
    public:
        AutoPassive();
        virtual void OnTick() override;
        
    private:
        int m_PlayerThreshold = 15; // Enable when 15+ players in session
        bool m_WasInPassive = false;
    };
}