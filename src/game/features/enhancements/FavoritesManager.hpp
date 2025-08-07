#pragma once
#include "core/commands/LoopedCommand.hpp"
#include "core/util/Joaat.hpp"
#include <vector>
#include <string>
#include <memory>

namespace YimMenu::Features
{
    struct FavoriteItem
    {
        joaat_t command_hash;
        std::string display_name;
        std::string category;
        bool is_enabled;
        
        FavoriteItem(joaat_t hash, const std::string& name, const std::string& cat) 
            : command_hash(hash), display_name(name), category(cat), is_enabled(false) {}
    };

    class FavoritesManager
    {
    public:
        static FavoritesManager& GetInstance()
        {
            static FavoritesManager instance;
            return instance;
        }

        void AddFavorite(joaat_t command_hash, const std::string& display_name, const std::string& category);
        void RemoveFavorite(joaat_t command_hash);
        bool IsFavorite(joaat_t command_hash) const;
        const std::vector<FavoriteItem>& GetFavorites() const { return m_Favorites; }
        
        void SaveFavorites();
        void LoadFavorites();
        
        void ClearAllFavorites();
        size_t GetFavoriteCount() const { return m_Favorites.size(); }

    private:
        std::vector<FavoriteItem> m_Favorites;
        bool m_Loaded = false;
        
        void EnsureLoaded();
        std::string GetFavoritesFilePath() const;
    };
}