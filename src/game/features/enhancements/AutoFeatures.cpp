#include "AutoFeatures.hpp"
#include "game/gta/Natives.hpp"
#include "core/logger/LogHelper.hpp"
#include "core/frontend/Notifications.hpp"

namespace YimMenu::Features
{
    // Auto-Heal Implementation
    AutoHeal::AutoHeal() :
        LoopedCommand("autoheal"_J, "Auto Heal", "Automatically heal when health gets low", 1000)
    {
    }

    void AutoHeal::OnTick()
    {
        Ped playerPed = PLAYER::PLAYER_PED_ID();
        if (!ENTITY::DOES_ENTITY_EXIST(playerPed))
            return;
            
        float currentHealth = ENTITY::GET_ENTITY_HEALTH(playerPed);
        float maxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(playerPed);
        float healthPercent = (currentHealth / maxHealth) * 100.0f;
        
        if (healthPercent < m_HealthThreshold && currentHealth > 0)
        {
            ENTITY::SET_ENTITY_HEALTH(playerPed, static_cast<int>(maxHealth), 0, 0);
            LOG(INFO) << "Auto-heal triggered at " << healthPercent << "% health";
        }
    }

    // Auto-Armor Implementation
    AutoArmor::AutoArmor() :
        LoopedCommand("autoarmor"_J, "Auto Armor", "Automatically refill armor when it gets low", 1000)
    {
    }

    void AutoArmor::OnTick()
    {
        Ped playerPed = PLAYER::PLAYER_PED_ID();
        if (!ENTITY::DOES_ENTITY_EXIST(playerPed))
            return;
            
        int currentArmor = PED::GET_PED_ARMOUR(playerPed);
        int maxArmor = PLAYER::GET_PLAYER_MAX_ARMOUR(PLAYER::PLAYER_ID());
        float armorPercent = (static_cast<float>(currentArmor) / static_cast<float>(maxArmor)) * 100.0f;
        
        if (armorPercent < m_ArmorThreshold)
        {
            PED::SET_PED_ARMOUR(playerPed, maxArmor);
            LOG(INFO) << "Auto-armor triggered at " << armorPercent << "% armor";
        }
    }

    // Auto-Repair Implementation
    AutoRepair::AutoRepair() :
        LoopedCommand("autorepair"_J, "Auto Repair", "Automatically repair vehicle when damaged", 1000)
    {
        m_LastRepair = std::chrono::steady_clock::now();
    }

    void AutoRepair::OnTick()
    {
        Ped playerPed = PLAYER::PLAYER_PED_ID();
        if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, false))
            return;
            
        Vehicle vehicle = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);
        if (!ENTITY::DOES_ENTITY_EXIST(vehicle))
            return;
            
        auto now = std::chrono::steady_clock::now();
        if (now - m_LastRepair < m_RepairCooldown)
            return;
            
        float vehicleHealth = ENTITY::GET_ENTITY_HEALTH(vehicle);
        
        if (vehicleHealth < m_DamageThreshold)
        {
            VEHICLE::SET_VEHICLE_FIXED(vehicle);
            VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(vehicle);
            VEHICLE::SET_VEHICLE_UNDRIVEABLE(vehicle, false);
            
            m_LastRepair = now;
            LOG(INFO) << "Auto-repair triggered at " << vehicleHealth << " health";
        }
    }

    // Auto-Clear Wanted Implementation
    AutoClearWanted::AutoClearWanted() :
        LoopedCommand("autoclearwanted"_J, "Auto Clear Wanted", "Automatically clear wanted level", 1000)
    {
        m_LastClear = std::chrono::steady_clock::now();
    }

    void AutoClearWanted::OnTick()
    {
        Player player = PLAYER::PLAYER_ID();
        int wantedLevel = PLAYER::GET_PLAYER_WANTED_LEVEL(player);
        
        if (wantedLevel > 0)
        {
            auto now = std::chrono::steady_clock::now();
            if (now - m_LastClear >= m_ClearDelay)
            {
                PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player);
                m_LastClear = now;
                LOG(INFO) << "Auto-cleared wanted level: " << wantedLevel;
            }
        }
    }

    // Auto-Passive Implementation
    AutoPassive::AutoPassive() :
        LoopedCommand("autopassive"_J, "Auto Passive", "Automatically enable passive mode in populated sessions", 5000)
    {
    }

    void AutoPassive::OnTick()
    {
        if (!NETWORK::NETWORK_IS_SESSION_STARTED())
            return;
            
        int playerCount = NETWORK::NETWORK_GET_NUM_CONNECTED_PLAYERS();
        bool shouldBePassive = playerCount >= m_PlayerThreshold;
        bool isCurrentlyPassive = NETWORK::NETWORK_IS_PLAYER_IN_GHOST_MODE(PLAYER::PLAYER_ID());
        
        if (shouldBePassive && !isCurrentlyPassive && !m_WasInPassive)
        {
            // Enable passive mode
            NETWORK::NETWORK_SET_PLAYER_IN_GHOST_MODE(PLAYER::PLAYER_ID(), true);
            m_WasInPassive = true;
            
            std::string message = "Auto-passive enabled (" + std::to_string(playerCount) + " players)";
            Notifications::Show("Auto-Passive", message, NotificationType::Info);
            LOG(INFO) << message;
        }
        else if (!shouldBePassive && isCurrentlyPassive && m_WasInPassive)
        {
            // Disable passive mode
            NETWORK::NETWORK_SET_PLAYER_IN_GHOST_MODE(PLAYER::PLAYER_ID(), false);
            m_WasInPassive = false;
            
            std::string message = "Auto-passive disabled (" + std::to_string(playerCount) + " players)";
            Notifications::Show("Auto-Passive", message, NotificationType::Info);
            LOG(INFO) << message;
        }
    }
}