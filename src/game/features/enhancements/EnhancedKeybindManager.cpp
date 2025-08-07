#include "EnhancedKeybindManager.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "core/commands/Commands.hpp"
#include "core/logger/LogHelper.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <Windows.h>

namespace YimMenu::Features
{
    std::string KeyBind::GetDisplayString() const
    {
        std::string result;
        
        if (ctrl_modifier) result += "Ctrl+";
        if (alt_modifier) result += "Alt+";
        if (shift_modifier) result += "Shift+";
        
        result += EnhancedKeybindManager::GetKeyName(key_code);
        
        return result;
    }

    bool KeyBind::IsPressed() const
    {
        bool ctrl_ok = !ctrl_modifier || (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        bool alt_ok = !alt_modifier || (GetAsyncKeyState(VK_MENU) & 0x8000);
        bool shift_ok = !shift_modifier || (GetAsyncKeyState(VK_SHIFT) & 0x8000);
        
        return ctrl_ok && alt_ok && shift_ok && (GetAsyncKeyState(key_code) & 0x8000);
    }

    std::uint32_t KeyBind::GetUniqueId() const
    {
        std::uint32_t id = static_cast<std::uint32_t>(key_code);
        id |= (ctrl_modifier ? 1 : 0) << 16;
        id |= (alt_modifier ? 1 : 0) << 17;
        id |= (shift_modifier ? 1 : 0) << 18;
        id |= (static_cast<std::uint32_t>(trigger_state) & 0x3) << 19;
        return id;
    }

    void EnhancedKeybindManager::AddKeybind(const KeyBind& keybind)
    {
        EnsureLoaded();
        
        std::uint32_t id = keybind.GetUniqueId();
        m_Keybinds[id] = keybind;
        SaveKeybinds();
        
        LOG(INFO) << "Added keybind: " << keybind.GetDisplayString() << " -> " << keybind.command_name;
    }

    void EnhancedKeybindManager::RemoveKeybind(std::uint32_t keybind_id)
    {
        EnsureLoaded();
        
        auto it = m_Keybinds.find(keybind_id);
        if (it != m_Keybinds.end())
        {
            LOG(INFO) << "Removed keybind: " << it->second.GetDisplayString();
            m_Keybinds.erase(it);
            SaveKeybinds();
        }
    }

    void EnhancedKeybindManager::RemoveKeybindsForCommand(joaat_t command_hash)
    {
        EnsureLoaded();
        
        auto it = m_Keybinds.begin();
        int removed_count = 0;
        
        while (it != m_Keybinds.end())
        {
            if (it->second.command_hash == command_hash)
            {
                it = m_Keybinds.erase(it);
                removed_count++;
            }
            else
            {
                ++it;
            }
        }
        
        if (removed_count > 0)
        {
            SaveKeybinds();
            LOG(INFO) << "Removed " << removed_count << " keybinds for command";
        }
    }

    bool EnhancedKeybindManager::HasKeybind(joaat_t command_hash) const
    {
        const_cast<EnhancedKeybindManager*>(this)->EnsureLoaded();
        
        for (const auto& [id, keybind] : m_Keybinds)
        {
            if (keybind.command_hash == command_hash)
                return true;
        }
        return false;
    }

    std::vector<KeyBind> EnhancedKeybindManager::GetKeybindsForCommand(joaat_t command_hash) const
    {
        const_cast<EnhancedKeybindManager*>(this)->EnsureLoaded();
        
        std::vector<KeyBind> result;
        for (const auto& [id, keybind] : m_Keybinds)
        {
            if (keybind.command_hash == command_hash)
                result.push_back(keybind);
        }
        return result;
    }

    void EnhancedKeybindManager::ProcessKeybinds()
    {
        EnsureLoaded();
        
        for (const auto& [id, keybind] : m_Keybinds)
        {
            bool is_pressed = IsKeyPressed(keybind.key_code);
            bool was_pressed = m_PreviousKeyStates[keybind.key_code];
            
            bool should_trigger = false;
            
            switch (keybind.trigger_state)
            {
                case KeyState::PRESSED:
                    should_trigger = is_pressed && !was_pressed;
                    break;
                case KeyState::RELEASED:
                    should_trigger = !is_pressed && was_pressed;
                    break;
                case KeyState::HELD:
                    should_trigger = is_pressed;
                    break;
            }
            
            if (should_trigger && keybind.IsPressed())
            {
                ExecuteCommand(keybind.command_hash);
            }
            
            m_PreviousKeyStates[keybind.key_code] = is_pressed;
        }
    }

    void EnhancedKeybindManager::SaveKeybinds()
    {
        try
        {
            nlohmann::json json_data = nlohmann::json::array();
            
            for (const auto& [id, keybind] : m_Keybinds)
            {
                nlohmann::json item;
                item["key_code"] = keybind.key_code;
                item["ctrl_modifier"] = keybind.ctrl_modifier;
                item["alt_modifier"] = keybind.alt_modifier;
                item["shift_modifier"] = keybind.shift_modifier;
                item["command_hash"] = static_cast<std::uint32_t>(keybind.command_hash);
                item["command_name"] = keybind.command_name;
                item["trigger_state"] = static_cast<int>(keybind.trigger_state);
                json_data.push_back(item);
            }
            
            std::string file_path = GetKeybindsFilePath();
            std::ofstream file(file_path);
            if (file.is_open())
            {
                file << json_data.dump(4);
                file.close();
                LOG(INFO) << "Saved " << m_Keybinds.size() << " keybinds";
            }
            else
            {
                LOG(WARNING) << "Failed to save keybinds to: " << file_path;
            }
        }
        catch (const std::exception& e)
        {
            LOG(WARNING) << "Error saving keybinds: " << e.what();
        }
    }

    void EnhancedKeybindManager::LoadKeybinds()
    {
        try
        {
            std::string file_path = GetKeybindsFilePath();
            std::ifstream file(file_path);
            
            if (!file.is_open())
            {
                LOG(INFO) << "No keybinds file found, starting fresh";
                m_Loaded = true;
                return;
            }
            
            nlohmann::json json_data;
            file >> json_data;
            file.close();
            
            m_Keybinds.clear();
            
            if (json_data.is_array())
            {
                for (const auto& item : json_data)
                {
                    KeyBind keybind;
                    keybind.key_code = item["key_code"];
                    keybind.ctrl_modifier = item["ctrl_modifier"];
                    keybind.alt_modifier = item["alt_modifier"];
                    keybind.shift_modifier = item["shift_modifier"];
                    keybind.command_hash = static_cast<joaat_t>(item["command_hash"].get<std::uint32_t>());
                    keybind.command_name = item["command_name"];
                    keybind.trigger_state = static_cast<KeyState>(item["trigger_state"].get<int>());
                    
                    m_Keybinds[keybind.GetUniqueId()] = keybind;
                }
            }
            
            LOG(INFO) << "Loaded " << m_Keybinds.size() << " keybinds";
        }
        catch (const std::exception& e)
        {
            LOG(WARNING) << "Error loading keybinds: " << e.what();
        }
        
        m_Loaded = true;
    }

    void EnhancedKeybindManager::ClearAllKeybinds()
    {
        m_Keybinds.clear();
        SaveKeybinds();
        LOG(INFO) << "Cleared all keybinds";
    }

    void EnhancedKeybindManager::EnsureLoaded()
    {
        if (!m_Loaded)
        {
            LoadKeybinds();
        }
    }

    std::string EnhancedKeybindManager::GetKeybindsFilePath() const
    {
        auto folder = FileMgr::GetProjectFolder("enhancements");
        folder.GetSubfolder("").EnsureExists();
        return (folder.GetFile("keybinds.json")).GetPath();
    }

    bool EnhancedKeybindManager::IsKeyPressed(int key_code) const
    {
        return GetAsyncKeyState(key_code) & 0x8000;
    }

    bool EnhancedKeybindManager::IsModifierPressed(int modifier_key) const
    {
        return GetAsyncKeyState(modifier_key) & 0x8000;
    }

    void EnhancedKeybindManager::ExecuteCommand(joaat_t command_hash)
    {
        if (auto command = Commands::GetCommand(command_hash))
        {
            command->Call();
        }
    }

    std::string EnhancedKeybindManager::GetKeyName(int key_code)
    {
        // Basic key name mapping - extend as needed
        static std::unordered_map<int, std::string> key_names = {
            {VK_F1, "F1"}, {VK_F2, "F2"}, {VK_F3, "F3"}, {VK_F4, "F4"},
            {VK_F5, "F5"}, {VK_F6, "F6"}, {VK_F7, "F7"}, {VK_F8, "F8"},
            {VK_F9, "F9"}, {VK_F10, "F10"}, {VK_F11, "F11"}, {VK_F12, "F12"},
            {VK_INSERT, "Insert"}, {VK_DELETE, "Delete"}, {VK_HOME, "Home"},
            {VK_END, "End"}, {VK_PRIOR, "Page Up"}, {VK_NEXT, "Page Down"},
            {VK_UP, "Up"}, {VK_DOWN, "Down"}, {VK_LEFT, "Left"}, {VK_RIGHT, "Right"},
            {VK_NUMPAD0, "Num 0"}, {VK_NUMPAD1, "Num 1"}, {VK_NUMPAD2, "Num 2"},
            {VK_NUMPAD3, "Num 3"}, {VK_NUMPAD4, "Num 4"}, {VK_NUMPAD5, "Num 5"},
            {VK_NUMPAD6, "Num 6"}, {VK_NUMPAD7, "Num 7"}, {VK_NUMPAD8, "Num 8"},
            {VK_NUMPAD9, "Num 9"}, {VK_MULTIPLY, "Num *"}, {VK_ADD, "Num +"},
            {VK_SUBTRACT, "Num -"}, {VK_DECIMAL, "Num ."}, {VK_DIVIDE, "Num /"}
        };

        auto it = key_names.find(key_code);
        if (it != key_names.end())
            return it->second;

        // For letter and number keys
        if (key_code >= 'A' && key_code <= 'Z')
            return std::string(1, static_cast<char>(key_code));
        if (key_code >= '0' && key_code <= '9')
            return std::string(1, static_cast<char>(key_code));

        return "Key " + std::to_string(key_code);
    }

    std::vector<std::pair<int, std::string>> EnhancedKeybindManager::GetAvailableKeys()
    {
        std::vector<std::pair<int, std::string>> keys;

        // Function keys
        for (int i = VK_F1; i <= VK_F12; ++i)
            keys.emplace_back(i, GetKeyName(i));

        // Letter keys
        for (int i = 'A'; i <= 'Z'; ++i)
            keys.emplace_back(i, GetKeyName(i));

        // Number keys
        for (int i = '0'; i <= '9'; ++i)
            keys.emplace_back(i, GetKeyName(i));

        // Special keys
        keys.emplace_back(VK_INSERT, "Insert");
        keys.emplace_back(VK_DELETE, "Delete");
        keys.emplace_back(VK_HOME, "Home");
        keys.emplace_back(VK_END, "End");
        keys.emplace_back(VK_PRIOR, "Page Up");
        keys.emplace_back(VK_NEXT, "Page Down");

        // Arrow keys
        keys.emplace_back(VK_UP, "Up");
        keys.emplace_back(VK_DOWN, "Down");
        keys.emplace_back(VK_LEFT, "Left");
        keys.emplace_back(VK_RIGHT, "Right");

        return keys;
    }
}