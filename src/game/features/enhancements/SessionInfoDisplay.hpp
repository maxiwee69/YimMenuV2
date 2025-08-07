#pragma once
#include "core/commands/LoopedCommand.hpp"
#include <chrono>
#include <string>

namespace YimMenu::Features
{
    struct SessionInfo
    {
        int player_count = 0;
        int max_players = 30;
        std::string session_type = "Unknown";
        std::string host_name = "Unknown";
        bool is_host = false;
        std::chrono::steady_clock::time_point session_start_time;
        int total_earned = 0;
        int total_spent = 0;
        
        std::string GetSessionDuration() const;
        std::string GetFormattedPlayerCount() const;
    };

    class SessionInfoDisplay : public LoopedCommand
    {
    public:
        SessionInfoDisplay();

        virtual void OnTick() override;
        virtual void OnEnable() override;
        virtual void OnDisable() override;

        const SessionInfo& GetSessionInfo() const { return m_SessionInfo; }
        bool ShouldShowOverlay() const { return m_ShowOverlay; }
        void SetShowOverlay(bool show) { m_ShowOverlay = show; }

    private:
        SessionInfo m_SessionInfo;
        bool m_ShowOverlay = false;
        std::chrono::steady_clock::time_point m_LastUpdate;
        
        void UpdateSessionInfo();
        void UpdatePlayerCount();
        void UpdateSessionType();
        void UpdateHostInfo();
        bool IsSessionHost();
    };

    // Global instance access
    SessionInfoDisplay& GetSessionInfoDisplay();
}