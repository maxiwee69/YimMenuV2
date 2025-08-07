#include "ProfileManager.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "core/commands/Commands.hpp"
#include "core/logger/LogHelper.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <regex>

namespace YimMenu::Features
{
    bool Profile::IsValid() const
    {
        return !name.empty() && std::regex_match(name, std::regex("^[a-zA-Z0-9_\\-\\s]+$"));
    }

    std::string Profile::GetDisplayName() const
    {
        if (description.empty())
            return name;
        return name + " - " + description;
    }

    bool ProfileManager::CreateProfile(const std::string& name, const std::string& description)
    {
        EnsureLoaded();
        
        if (!IsValidProfileName(name))
        {
            LOG(WARNING) << "Invalid profile name: " << name;
            return false;
        }
        
        if (m_Profiles.find(name) != m_Profiles.end())
        {
            LOG(WARNING) << "Profile already exists: " << name;
            return false;
        }
        
        Profile profile;
        profile.name = name;
        profile.description = description;
        profile.created_date = GetCurrentTimestamp();
        
        // Capture current settings
        profile = CaptureCurrentSettings();
        profile.name = name;
        profile.description = description;
        profile.created_date = GetCurrentTimestamp();
        
        m_Profiles[name] = profile;
        SaveSettings();
        
        LOG(INFO) << "Created profile: " << name;
        return true;
    }

    bool ProfileManager::DeleteProfile(const std::string& name)
    {
        EnsureLoaded();
        
        if (name == "Default")
        {
            LOG(WARNING) << "Cannot delete default profile";
            return false;
        }
        
        auto it = m_Profiles.find(name);
        if (it == m_Profiles.end())
        {
            LOG(WARNING) << "Profile not found: " << name;
            return false;
        }
        
        m_Profiles.erase(it);
        
        if (m_CurrentProfile == name)
        {
            m_CurrentProfile = "Default";
        }
        
        SaveSettings();
        LOG(INFO) << "Deleted profile: " << name;
        return true;
    }

    bool ProfileManager::LoadProfile(const std::string& name)
    {
        EnsureLoaded();
        
        auto it = m_Profiles.find(name);
        if (it == m_Profiles.end())
        {
            LOG(WARNING) << "Profile not found: " << name;
            return false;
        }
        
        ApplyProfile(it->second);
        m_CurrentProfile = name;
        
        LOG(INFO) << "Loaded profile: " << name;
        return true;
    }

    bool ProfileManager::SaveCurrentProfile(const std::string& name, const std::string& description)
    {
        EnsureLoaded();
        
        if (!IsValidProfileName(name))
        {
            LOG(WARNING) << "Invalid profile name: " << name;
            return false;
        }
        
        Profile profile = CaptureCurrentSettings();
        profile.name = name;
        profile.description = description;
        
        if (m_Profiles.find(name) == m_Profiles.end())
        {
            profile.created_date = GetCurrentTimestamp();
        }
        else
        {
            // Keep original creation date
            profile.created_date = m_Profiles[name].created_date;
        }
        
        m_Profiles[name] = profile;
        m_CurrentProfile = name;
        SaveSettings();
        
        LOG(INFO) << "Saved current settings to profile: " << name;
        return true;
    }

    std::vector<std::string> ProfileManager::GetProfileNames() const
    {
        const_cast<ProfileManager*>(this)->EnsureLoaded();
        
        std::vector<std::string> names;
        for (const auto& [name, profile] : m_Profiles)
        {
            names.push_back(name);
        }
        return names;
    }

    const Profile* ProfileManager::GetProfile(const std::string& name) const
    {
        const_cast<ProfileManager*>(this)->EnsureLoaded();
        
        auto it = m_Profiles.find(name);
        return (it != m_Profiles.end()) ? &it->second : nullptr;
    }

    void ProfileManager::CheckAutoSave()
    {
        if (!m_AutoSaveEnabled)
            return;
            
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - m_LastAutoSave);
        
        if (duration.count() >= m_AutoSaveInterval)
        {
            SaveCurrentProfile(m_CurrentProfile, "Auto-saved at " + GetCurrentTimestamp());
            m_LastAutoSave = now;
            LOG(INFO) << "Auto-saved profile: " << m_CurrentProfile;
        }
    }

    void ProfileManager::SaveSettings()
    {
        try
        {
            nlohmann::json json_data;
            json_data["current_profile"] = m_CurrentProfile;
            json_data["auto_save_enabled"] = m_AutoSaveEnabled;
            json_data["auto_save_interval"] = m_AutoSaveInterval;
            
            nlohmann::json profiles_array = nlohmann::json::array();
            for (const auto& [name, profile] : m_Profiles)
            {
                nlohmann::json profile_json;
                profile_json["name"] = profile.name;
                profile_json["description"] = profile.description;
                profile_json["created_date"] = profile.created_date;
                profile_json["bool_settings"] = profile.bool_settings;
                profile_json["int_settings"] = profile.int_settings;
                profile_json["float_settings"] = profile.float_settings;
                profile_json["string_settings"] = profile.string_settings;
                profiles_array.push_back(profile_json);
            }
            json_data["profiles"] = profiles_array;
            
            std::string file_path = GetProfilesFilePath();
            std::ofstream file(file_path);
            if (file.is_open())
            {
                file << json_data.dump(4);
                file.close();
                LOG(INFO) << "Saved " << m_Profiles.size() << " profiles";
            }
            else
            {
                LOG(WARNING) << "Failed to save profiles to: " << file_path;
            }
        }
        catch (const std::exception& e)
        {
            LOG(WARNING) << "Error saving profiles: " << e.what();
        }
    }

    void ProfileManager::LoadSettings()
    {
        try
        {
            std::string file_path = GetProfilesFilePath();
            std::ifstream file(file_path);
            
            if (!file.is_open())
            {
                LOG(INFO) << "No profiles file found, creating default profile";
                
                // Create default profile
                Profile defaultProfile = CaptureCurrentSettings();
                defaultProfile.name = "Default";
                defaultProfile.description = "Default settings";
                defaultProfile.created_date = GetCurrentTimestamp();
                m_Profiles["Default"] = defaultProfile;
                
                m_Loaded = true;
                SaveSettings();
                return;
            }
            
            nlohmann::json json_data;
            file >> json_data;
            file.close();
            
            if (json_data.contains("current_profile"))
                m_CurrentProfile = json_data["current_profile"];
            if (json_data.contains("auto_save_enabled"))
                m_AutoSaveEnabled = json_data["auto_save_enabled"];
            if (json_data.contains("auto_save_interval"))
                m_AutoSaveInterval = json_data["auto_save_interval"];
            
            m_Profiles.clear();
            
            if (json_data.contains("profiles") && json_data["profiles"].is_array())
            {
                for (const auto& profile_json : json_data["profiles"])
                {
                    Profile profile;
                    profile.name = profile_json["name"];
                    profile.description = profile_json["description"];
                    profile.created_date = profile_json["created_date"];
                    
                    if (profile_json.contains("bool_settings"))
                        profile.bool_settings = profile_json["bool_settings"];
                    if (profile_json.contains("int_settings"))
                        profile.int_settings = profile_json["int_settings"];
                    if (profile_json.contains("float_settings"))
                        profile.float_settings = profile_json["float_settings"];
                    if (profile_json.contains("string_settings"))
                        profile.string_settings = profile_json["string_settings"];
                    
                    m_Profiles[profile.name] = profile;
                }
            }
            
            LOG(INFO) << "Loaded " << m_Profiles.size() << " profiles";
            m_LastAutoSave = std::chrono::steady_clock::now();
        }
        catch (const std::exception& e)
        {
            LOG(WARNING) << "Error loading profiles: " << e.what();
        }
        
        m_Loaded = true;
    }

    void ProfileManager::EnsureLoaded()
    {
        if (!m_Loaded)
        {
            LoadSettings();
        }
    }

    std::string ProfileManager::GetProfilesFilePath() const
    {
        auto folder = FileMgr::GetProjectFolder("enhancements");
        folder.GetSubfolder("").EnsureExists();
        return (folder.GetFile("profiles.json")).GetPath();
    }

    std::string ProfileManager::GetCurrentTimestamp() const
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    Profile ProfileManager::CaptureCurrentSettings() const
    {
        Profile profile;
        
        // This would capture all current command states
        // For now, we'll just capture a few example settings
        // In a real implementation, this would iterate through all commands
        // and capture their current states
        
        // Example: capture some common boolean settings
        const std::vector<std::string> bool_commands = {
            "godmode", "invis", "noidlekick", "formatmoney", "neverwanted"
        };
        
        for (const auto& cmd_name : bool_commands)
        {
            joaat_t hash = joaat(cmd_name);
            if (auto command = Commands::GetCommand(hash))
            {
                // This would get the actual command state
                // For demonstration purposes, we'll just set a placeholder
                profile.bool_settings[cmd_name] = false;
            }
        }
        
        return profile;
    }

    void ProfileManager::ApplyProfile(const Profile& profile)
    {
        // Apply boolean settings
        for (const auto& [cmd_name, value] : profile.bool_settings)
        {
            joaat_t hash = joaat(cmd_name);
            if (auto command = Commands::GetCommand(hash))
            {
                // This would actually apply the setting to the command
                // For demonstration purposes, we'll just log it
                LOG(INFO) << "Applying setting: " << cmd_name << " = " << value;
            }
        }
        
        // Apply other setting types similarly...
        LOG(INFO) << "Applied profile settings: " << profile.name;
    }

    bool ProfileManager::IsValidProfileName(const std::string& name) const
    {
        if (name.empty() || name.length() > 50)
            return false;
            
        // Check for valid characters only
        return std::regex_match(name, std::regex("^[a-zA-Z0-9_\\-\\s]+$"));
    }
}