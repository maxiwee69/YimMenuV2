#include "FavoritesManager.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "core/commands/Commands.hpp"
#include "core/logger/LogHelper.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

namespace YimMenu::Features
{
    void FavoritesManager::AddFavorite(joaat_t command_hash, const std::string& display_name, const std::string& category)
    {
        EnsureLoaded();
        
        // Check if already exists
        for (const auto& fav : m_Favorites)
        {
            if (fav.command_hash == command_hash)
            {
                LOG(WARNING) << "Favorite already exists: " << display_name;
                return;
            }
        }
        
        m_Favorites.emplace_back(command_hash, display_name, category);
        SaveFavorites();
        
        LOG(INFO) << "Added favorite: " << display_name << " (" << category << ")";
    }

    void FavoritesManager::RemoveFavorite(joaat_t command_hash)
    {
        EnsureLoaded();
        
        auto it = std::remove_if(m_Favorites.begin(), m_Favorites.end(),
            [command_hash](const FavoriteItem& item) {
                return item.command_hash == command_hash;
            });
            
        if (it != m_Favorites.end())
        {
            m_Favorites.erase(it, m_Favorites.end());
            SaveFavorites();
            LOG(INFO) << "Removed favorite";
        }
    }

    bool FavoritesManager::IsFavorite(joaat_t command_hash) const
    {
        const_cast<FavoritesManager*>(this)->EnsureLoaded();
        
        for (const auto& fav : m_Favorites)
        {
            if (fav.command_hash == command_hash)
                return true;
        }
        return false;
    }

    void FavoritesManager::ClearAllFavorites()
    {
        m_Favorites.clear();
        SaveFavorites();
        LOG(INFO) << "Cleared all favorites";
    }

    void FavoritesManager::SaveFavorites()
    {
        try
        {
            nlohmann::json json_data = nlohmann::json::array();
            
            for (const auto& fav : m_Favorites)
            {
                nlohmann::json item;
                item["hash"] = static_cast<std::uint32_t>(fav.command_hash);
                item["display_name"] = fav.display_name;
                item["category"] = fav.category;
                json_data.push_back(item);
            }
            
            std::string file_path = GetFavoritesFilePath();
            std::ofstream file(file_path);
            if (file.is_open())
            {
                file << json_data.dump(4);
                file.close();
                LOG(INFO) << "Saved " << m_Favorites.size() << " favorites";
            }
            else
            {
                LOG(WARNING) << "Failed to save favorites to: " << file_path;
            }
        }
        catch (const std::exception& e)
        {
            LOG(WARNING) << "Error saving favorites: " << e.what();
        }
    }

    void FavoritesManager::LoadFavorites()
    {
        try
        {
            std::string file_path = GetFavoritesFilePath();
            std::ifstream file(file_path);
            
            if (!file.is_open())
            {
                LOG(INFO) << "No favorites file found, starting fresh";
                m_Loaded = true;
                return;
            }
            
            nlohmann::json json_data;
            file >> json_data;
            file.close();
            
            m_Favorites.clear();
            
            if (json_data.is_array())
            {
                for (const auto& item : json_data)
                {
                    if (item.contains("hash") && item.contains("display_name") && item.contains("category"))
                    {
                        joaat_t hash = static_cast<joaat_t>(item["hash"].get<std::uint32_t>());
                        std::string display_name = item["display_name"];
                        std::string category = item["category"];
                        
                        m_Favorites.emplace_back(hash, display_name, category);
                    }
                }
            }
            
            LOG(INFO) << "Loaded " << m_Favorites.size() << " favorites";
        }
        catch (const std::exception& e)
        {
            LOG(WARNING) << "Error loading favorites: " << e.what();
        }
        
        m_Loaded = true;
    }

    void FavoritesManager::EnsureLoaded()
    {
        if (!m_Loaded)
        {
            LoadFavorites();
        }
    }

    std::string FavoritesManager::GetFavoritesFilePath() const
    {
        auto folder = FileMgr::GetProjectFolder("enhancements");
        folder.GetSubfolder("").EnsureExists();
        return (folder.GetFile("favorites.json")).GetPath();
    }
}