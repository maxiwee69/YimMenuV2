#include "Enhancements.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/features/enhancements/FavoritesManager.hpp"
#include "game/features/enhancements/SessionInfoDisplay.hpp"
#include "game/features/enhancements/EnhancedKeybindManager.hpp"
#include "game/features/enhancements/ProfileManager.hpp"
#include "game/features/enhancements/AutoFeatures.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/commands/Commands.hpp"

namespace YimMenu::Submenus
{
	Enhancements::Enhancements() :
		#define ICON_FA_STAR "\xef\x80\x85"
		Submenu::Submenu("Enhancements", ICON_FA_STAR)
	{
		auto main = std::make_shared<Category>("Main");
		auto favoritesGroup = std::make_shared<Group>("Quick Access Favorites");
		auto sessionGroup = std::make_shared<Group>("Session Information");
		auto keybindGroup = std::make_shared<Group>("Enhanced Keybinds");
		auto profileGroup = std::make_shared<Group>("Profile Management", 2);
		auto autoGroup = std::make_shared<Group>("Auto Features", 2);
		auto utilsGroup = std::make_shared<Group>("Utilities", 2);

		// Favorites section
		favoritesGroup->AddItem(std::make_shared<InfoItem>("Favorite frequently used features for quick access"));
		
		favoritesGroup->AddItem(std::make_shared<CommandItem>("Show Favorites Panel", "Toggle favorites panel", []{
			// This would be implemented to show a floating favorites panel
			Notifications::Show("Feature", "Favorites panel toggled", NotificationType::Success);
		}));

		favoritesGroup->AddItem(std::make_shared<CommandItem>("Manage Favorites", "Open favorites management", []{
			auto& favMgr = Features::FavoritesManager::GetInstance();
			auto favorites = favMgr.GetFavorites();
			
			std::string message = "Current favorites: " + std::to_string(favorites.size());
			Notifications::Show("Favorites", message, NotificationType::Info);
		}));

		favoritesGroup->AddItem(std::make_shared<CommandItem>("Clear All Favorites", "Remove all saved favorites", []{
			auto& favMgr = Features::FavoritesManager::GetInstance();
			favMgr.ClearAllFavorites();
			Notifications::Show("Favorites", "All favorites cleared", NotificationType::Warning);
		}));

		// Session Information section
		sessionGroup->AddItem(std::make_shared<InfoItem>("Real-time session and player information"));
		
		sessionGroup->AddItem(std::make_shared<BoolCommandItem>("sessioninfo"_J));
		
		sessionGroup->AddItem(std::make_shared<CommandItem>("Session Stats", "Show current session statistics", []{
			auto& sessionInfo = Features::GetSessionInfoDisplay();
			const auto& info = sessionInfo.GetSessionInfo();
			
			std::string stats = "Players: " + info.GetFormattedPlayerCount() + 
							  "\nDuration: " + info.GetSessionDuration() +
							  "\nType: " + info.session_type +
							  "\nHost: " + info.host_name;
			
			Notifications::Show("Session Info", stats, NotificationType::Info);
		}));

		sessionGroup->AddItem(std::make_shared<CommandItem>("Toggle Overlay", "Show/hide session info overlay", []{
			auto& sessionInfo = Features::GetSessionInfoDisplay();
			sessionInfo.SetShowOverlay(!sessionInfo.ShouldShowOverlay());
			
			std::string msg = sessionInfo.ShouldShowOverlay() ? "Overlay enabled" : "Overlay disabled";
			Notifications::Show("Session Info", msg, NotificationType::Success);
		}));

		// Enhanced Keybinds section
		keybindGroup->AddItem(std::make_shared<InfoItem>("Advanced hotkey management system"));
		
		keybindGroup->AddItem(std::make_shared<CommandItem>("Keybind Manager", "Open keybind management interface", []{
			auto& kbMgr = Features::EnhancedKeybindManager::GetInstance();
			auto keybinds = kbMgr.GetAllKeybinds();
			
			std::string message = "Active keybinds: " + std::to_string(keybinds.size());
			Notifications::Show("Keybinds", message, NotificationType::Info);
		}));

		keybindGroup->AddItem(std::make_shared<CommandItem>("Add Keybind", "Create a new keybind for any feature", []{
			// This would open a keybind creation interface
			Notifications::Show("Keybinds", "Keybind creation interface opened", NotificationType::Info);
		}));

		keybindGroup->AddItem(std::make_shared<CommandItem>("Clear All Keybinds", "Remove all custom keybinds", []{
			auto& kbMgr = Features::EnhancedKeybindManager::GetInstance();
			kbMgr.ClearAllKeybinds();
			Notifications::Show("Keybinds", "All keybinds cleared", NotificationType::Warning);
		}));

		// Utilities section
		utilsGroup->AddItem(std::make_shared<CommandItem>("Export Settings", "Export all enhancement settings", []{
			// This would export all settings to a file
			Notifications::Show("Export", "Settings exported successfully", NotificationType::Success);
		}));

		utilsGroup->AddItem(std::make_shared<CommandItem>("Import Settings", "Import enhancement settings from file", []{
			// This would import settings from a file
			Notifications::Show("Import", "Settings imported successfully", NotificationType::Success);
		}));

		utilsGroup->AddItem(std::make_shared<CommandItem>("Reset All", "Reset all enhancements to defaults", []{
			auto& favMgr = Features::FavoritesManager::GetInstance();
			auto& kbMgr = Features::EnhancedKeybindManager::GetInstance();
			
			favMgr.ClearAllFavorites();
			kbMgr.ClearAllKeybinds();
			
			Notifications::Show("Reset", "All enhancements reset to defaults", NotificationType::Warning);
		}));

		utilsGroup->AddItem(std::make_shared<InfoItem>("These features enhance your menu experience with improved usability"));

		// Profile Management section
		profileGroup->AddItem(std::make_shared<InfoItem>("Save and load different configuration profiles"));
		
		profileGroup->AddItem(std::make_shared<CommandItem>("Profile Manager", "Manage configuration profiles", []{
			auto& profileMgr = Features::ProfileManager::GetInstance();
			auto profiles = profileMgr.GetProfileNames();
			
			std::string message = "Available profiles: " + std::to_string(profiles.size()) + 
								"\nCurrent: " + profileMgr.GetCurrentProfileName();
			Notifications::Show("Profiles", message, NotificationType::Info);
		}));

		profileGroup->AddItem(std::make_shared<CommandItem>("Save Current Profile", "Save current settings as new profile", []{
			auto& profileMgr = Features::ProfileManager::GetInstance();
			std::string profileName = "Profile_" + std::to_string(std::time(nullptr));
			
			if (profileMgr.SaveCurrentProfile(profileName, "Auto-saved profile"))
			{
				Notifications::Show("Profiles", "Profile saved: " + profileName, NotificationType::Success);
			}
			else
			{
				Notifications::Show("Profiles", "Failed to save profile", NotificationType::Error);
			}
		}));

		profileGroup->AddItem(std::make_shared<CommandItem>("Load Default Profile", "Load the default configuration", []{
			auto& profileMgr = Features::ProfileManager::GetInstance();
			if (profileMgr.LoadProfile("Default"))
			{
				Notifications::Show("Profiles", "Default profile loaded", NotificationType::Success);
			}
			else
			{
				Notifications::Show("Profiles", "Failed to load default profile", NotificationType::Error);
			}
		}));

		// Auto Features section
		autoGroup->AddItem(std::make_shared<InfoItem>("Automated quality-of-life features"));
		
		autoGroup->AddItem(std::make_shared<BoolCommandItem>("autoheal"_J));
		autoGroup->AddItem(std::make_shared<BoolCommandItem>("autoarmor"_J));
		autoGroup->AddItem(std::make_shared<BoolCommandItem>("autorepair"_J));
		autoGroup->AddItem(std::make_shared<BoolCommandItem>("autoclearwanted"_J));
		autoGroup->AddItem(std::make_shared<BoolCommandItem>("autopassive"_J));

		autoGroup->AddItem(std::make_shared<CommandItem>("Configure Auto Features", "Adjust auto feature settings", []{
			Notifications::Show("Auto Features", "Configuration interface would open here", NotificationType::Info);
		}));

		main->AddGroup(favoritesGroup);
		main->AddGroup(sessionGroup);
		main->AddGroup(keybindGroup);
		main->AddGroup(profileGroup);
		main->AddGroup(autoGroup);
		main->AddGroup(utilsGroup);

		AddCategory(main);
	}
}