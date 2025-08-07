#pragma once
#include "core/util/Joaat.hpp"
#include <unordered_map>
#include <string>
#include <vector>

namespace YimMenu::Features
{
    enum class KeyState
    {
        PRESSED,
        RELEASED,
        HELD
    };

    struct KeyBind
    {
        int key_code;
        bool ctrl_modifier = false;
        bool alt_modifier = false;
        bool shift_modifier = false;
        joaat_t command_hash;
        std::string command_name;
        KeyState trigger_state = KeyState::PRESSED;
        
        std::string GetDisplayString() const;
        bool IsPressed() const;
        std::uint32_t GetUniqueId() const;
    };

    class EnhancedKeybindManager
    {
    public:
        static EnhancedKeybindManager& GetInstance()
        {
            static EnhancedKeybindManager instance;
            return instance;
        }

        void AddKeybind(const KeyBind& keybind);
        void RemoveKeybind(std::uint32_t keybind_id);
        void RemoveKeybindsForCommand(joaat_t command_hash);
        
        bool HasKeybind(joaat_t command_hash) const;
        std::vector<KeyBind> GetKeybindsForCommand(joaat_t command_hash) const;
        const std::unordered_map<std::uint32_t, KeyBind>& GetAllKeybinds() const { return m_Keybinds; }
        
        void ProcessKeybinds();
        void SaveKeybinds();
        void LoadKeybinds();
        
        void ClearAllKeybinds();
        size_t GetKeybindCount() const { return m_Keybinds.size(); }

        // Key name utilities
        static std::string GetKeyName(int key_code);
        static std::vector<std::pair<int, std::string>> GetAvailableKeys();

    private:
        std::unordered_map<std::uint32_t, KeyBind> m_Keybinds;
        std::unordered_map<int, bool> m_PreviousKeyStates;
        bool m_Loaded = false;
        
        void EnsureLoaded();
        std::string GetKeybindsFilePath() const;
        bool IsKeyPressed(int key_code) const;
        bool IsModifierPressed(int modifier_key) const;
        void ExecuteCommand(joaat_t command_hash);
    };
}