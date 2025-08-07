#include "SessionInfoDisplay.hpp"
#include "game/gta/Natives.hpp"
#include "game/backend/Players.hpp"
#include "core/logger/LogHelper.hpp"
#include <iomanip>
#include <sstream>

namespace YimMenu::Features
{
    static SessionInfoDisplay* g_SessionInfoDisplay = nullptr;

    SessionInfoDisplay::SessionInfoDisplay() :
        LoopedCommand("sessioninfo"_J, "Session Info Display", "Display real-time session information", 1000)
    {
        m_SessionInfo.session_start_time = std::chrono::steady_clock::now();
        m_LastUpdate = std::chrono::steady_clock::now();
    }

    void SessionInfoDisplay::OnTick()
    {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastUpdate).count() < 500)
            return;

        m_LastUpdate = now;
        UpdateSessionInfo();
    }

    void SessionInfoDisplay::OnEnable()
    {
        m_SessionInfo.session_start_time = std::chrono::steady_clock::now();
        LOG(INFO) << "Session Info Display enabled";
    }

    void SessionInfoDisplay::OnDisable()
    {
        LOG(INFO) << "Session Info Display disabled";
    }

    void SessionInfoDisplay::UpdateSessionInfo()
    {
        UpdatePlayerCount();
        UpdateSessionType();
        UpdateHostInfo();
    }

    void SessionInfoDisplay::UpdatePlayerCount()
    {
        if (NETWORK::NETWORK_IS_SESSION_STARTED())
        {
            m_SessionInfo.player_count = NETWORK::NETWORK_GET_NUM_CONNECTED_PLAYERS();
            m_SessionInfo.max_players = NETWORK::NETWORK_GET_MAX_FRIENDS();
        }
        else
        {
            m_SessionInfo.player_count = 1; // Solo session
            m_SessionInfo.max_players = 1;
        }
    }

    void SessionInfoDisplay::UpdateSessionType()
    {
        if (!NETWORK::NETWORK_IS_SESSION_STARTED())
        {
            m_SessionInfo.session_type = "Solo Session";
            return;
        }

        if (NETWORK::NETWORK_IS_SESSION_ACTIVE())
        {
            if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
            {
                m_SessionInfo.session_type = "Activity Session";
            }
            else if (NETWORK::NETWORK_IS_IN_SPECTATOR_MODE())
            {
                m_SessionInfo.session_type = "Spectator Mode";
            }
            else
            {
                m_SessionInfo.session_type = "Public Session";
            }
        }
        else
        {
            m_SessionInfo.session_type = "Loading...";
        }
    }

    void SessionInfoDisplay::UpdateHostInfo()
    {
        m_SessionInfo.is_host = IsSessionHost();
        
        if (m_SessionInfo.is_host)
        {
            m_SessionInfo.host_name = "You";
        }
        else
        {
            // Try to get host name from players list
            auto host_id = NETWORK::NETWORK_GET_HOST_OF_THIS_SCRIPT();
            if (auto player = Players::GetByID(host_id))
            {
                m_SessionInfo.host_name = player->GetName();
            }
            else
            {
                m_SessionInfo.host_name = "Unknown";
            }
        }
    }

    bool SessionInfoDisplay::IsSessionHost()
    {
        if (!NETWORK::NETWORK_IS_SESSION_STARTED())
            return true; // Solo session = host

        return NETWORK::NETWORK_IS_HOST();
    }

    std::string SessionInfo::GetSessionDuration() const
    {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - session_start_time);
        
        auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1));
        auto seconds = duration % std::chrono::minutes(1);
        
        std::ostringstream oss;
        if (hours.count() > 0)
        {
            oss << std::setfill('0') << std::setw(2) << hours.count() << ":"
                << std::setw(2) << minutes.count() << ":"
                << std::setw(2) << seconds.count();
        }
        else
        {
            oss << std::setfill('0') << std::setw(2) << minutes.count() << ":"
                << std::setw(2) << seconds.count();
        }
        
        return oss.str();
    }

    std::string SessionInfo::GetFormattedPlayerCount() const
    {
        return std::to_string(player_count) + "/" + std::to_string(max_players);
    }

    SessionInfoDisplay& GetSessionInfoDisplay()
    {
        if (!g_SessionInfoDisplay)
        {
            g_SessionInfoDisplay = new SessionInfoDisplay();
        }
        return *g_SessionInfoDisplay;
    }
}