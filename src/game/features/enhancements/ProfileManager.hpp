#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace YimMenu::Features
{
    struct Profile
    {
        std::string name;
        std::string description;
        std::string created_date;
        std::unordered_map<std::string, bool> bool_settings;
        std::unordered_map<std::string, int> int_settings;
        std::unordered_map<std::string, float> float_settings;
        std::unordered_map<std::string, std::string> string_settings;
        
        bool IsValid() const;
        std::string GetDisplayName() const;
    };

    class ProfileManager
    {
    public:
        static ProfileManager& GetInstance()
        {
            static ProfileManager instance;
            return instance;
        }

        // Profile operations
        bool CreateProfile(const std::string& name, const std::string& description = "");
        bool DeleteProfile(const std::string& name);
        bool LoadProfile(const std::string& name);
        bool SaveCurrentProfile(const std::string& name, const std::string& description = "");
        
        // Profile management
        std::vector<std::string> GetProfileNames() const;
        const Profile* GetProfile(const std::string& name) const;
        std::string GetCurrentProfileName() const { return m_CurrentProfile; }
        
        // Auto-save functionality
        void EnableAutoSave(bool enable) { m_AutoSaveEnabled = enable; }
        bool IsAutoSaveEnabled() const { return m_AutoSaveEnabled; }
        void SetAutoSaveInterval(int seconds) { m_AutoSaveInterval = seconds; }
        int GetAutoSaveInterval() const { return m_AutoSaveInterval; }
        
        void CheckAutoSave();
        void SaveSettings();
        void LoadSettings();
        
        size_t GetProfileCount() const { return m_Profiles.size(); }

    private:
        std::unordered_map<std::string, Profile> m_Profiles;
        std::string m_CurrentProfile = "Default";
        bool m_AutoSaveEnabled = true;
        int m_AutoSaveInterval = 300; // 5 minutes
        std::chrono::steady_clock::time_point m_LastAutoSave;
        bool m_Loaded = false;
        
        void EnsureLoaded();
        std::string GetProfilesFilePath() const;
        std::string GetCurrentTimestamp() const;
        Profile CaptureCurrentSettings() const;
        void ApplyProfile(const Profile& profile);
        bool IsValidProfileName(const std::string& name) const;
    };
}