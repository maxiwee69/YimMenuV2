# YimMenuV2 Enhancement Features

This document describes the new enhancement features added to YimMenuV2 to improve user experience and add quality-of-life improvements.

## New Features Overview

### 1. Quick Access Favorites System
**Location**: Enhancements > Quick Access Favorites

The favorites system allows you to bookmark frequently used features for quick access.

**Features:**
- Add any menu item to your favorites list
- Quick access panel for favorited features
- Persistent storage of favorites
- Category-based organization
- Easy management interface

**Usage:**
- Navigate to any feature in the menu
- Right-click or use the context menu to "Add to Favorites"
- Access favorites through the Enhancements menu
- Use "Show Favorites Panel" for a floating quick-access window

### 2. Enhanced Session Information Display
**Location**: Enhancements > Session Information

Real-time session monitoring and information display.

**Features:**
- Live player count and session capacity
- Session duration tracking
- Session type detection (Public, Solo, Activity, etc.)
- Host information display
- Optional overlay display
- Session statistics tracking

**Usage:**
- Enable "Session Info Display" to start monitoring
- Use "Toggle Overlay" to show/hide the information overlay
- View detailed session stats with "Session Stats"

### 3. Advanced Keybind Manager
**Location**: Enhancements > Enhanced Keybinds

Enhanced hotkey system with advanced features beyond the basic menu keybinds.

**Features:**
- Custom keybinds for any menu feature
- Modifier key support (Ctrl, Alt, Shift)
- Multiple trigger states (Press, Release, Hold)
- Persistent keybind storage
- Conflict detection and resolution
- Easy keybind management interface

**Usage:**
- Open "Keybind Manager" to view all active keybinds
- Use "Add Keybind" to create new hotkeys
- Assign keys like F1, Ctrl+G, Alt+Shift+H, etc.
- Set different trigger modes for different behaviors

### 4. Profile Management System
**Location**: Enhancements > Profile Management

Save and load different configuration profiles for different playstyles or situations.

**Features:**
- Save current menu settings as named profiles
- Load previously saved profiles
- Auto-save functionality with configurable intervals
- Profile descriptions and timestamps
- Import/export profiles for sharing
- Backup and restore capabilities

**Usage:**
- Use "Save Current Profile" to capture your current settings
- Create profiles for different scenarios (PvP, Grinding, Casual, etc.)
- Switch between profiles using "Profile Manager"
- Enable auto-save to automatically backup your settings

### 5. Auto Features
**Location**: Enhancements > Auto Features

Automated quality-of-life features that work in the background.

**Available Auto Features:**

#### Auto Heal
- Automatically restores health when it drops below a threshold
- Configurable health percentage trigger (default: 50%)
- Works seamlessly in the background

#### Auto Armor
- Automatically refills armor when it gets low
- Configurable armor percentage trigger (default: 30%)
- Ensures you're always protected

#### Auto Repair
- Automatically repairs your vehicle when damaged
- Configurable damage threshold
- Built-in cooldown to prevent spam

#### Auto Clear Wanted
- Automatically clears wanted level when you get stars
- Configurable delay before clearing
- Helps maintain low profile during activities

#### Auto Passive
- Automatically enables passive mode in crowded sessions
- Configurable player count threshold (default: 15+ players)
- Automatically disables when session becomes less crowded

**Usage:**
- Enable individual auto features as needed
- Configure thresholds and behaviors through the settings
- Monitor auto feature activity through notifications

## Technical Implementation

### File Structure
```
src/game/features/enhancements/
├── FavoritesManager.hpp/.cpp         # Favorites system
├── SessionInfoDisplay.hpp/.cpp       # Session monitoring
├── EnhancedKeybindManager.hpp/.cpp    # Advanced keybinds
├── ProfileManager.hpp/.cpp            # Profile management
└── AutoFeatures.hpp/.cpp             # Automated features

src/game/frontend/submenus/
├── Enhancements.hpp/.cpp             # UI for all enhancement features
```

### Data Storage
- All enhancement data is stored in JSON format
- Files are saved in the `enhancements/` subfolder
- Automatic backup and recovery mechanisms
- Cross-session persistence

### Integration Points
- Seamlessly integrated with existing command system
- Works with all existing menu features
- Non-intrusive implementation
- Optional features - can be disabled individually

## Configuration Files

### favorites.json
Stores favorited menu items with their display names and categories.

### keybinds.json
Stores custom keybind configurations with key codes and modifiers.

### profiles.json
Stores saved configuration profiles with all settings.

### session_info.json
Stores session statistics and preferences.

## Benefits

1. **Improved Efficiency**: Quick access to frequently used features
2. **Better Organization**: Favorites and profiles help organize workflows
3. **Enhanced Control**: Advanced keybinds provide more control options
4. **Automation**: Auto features reduce repetitive tasks
5. **Customization**: Profiles allow different setups for different scenarios
6. **Information**: Session monitoring provides better situational awareness

## Future Enhancements

Potential future additions to the enhancement system:

1. **Macro System**: Record and playback sequences of actions
2. **Smart Notifications**: Context-aware notifications and alerts
3. **Statistics Tracking**: Detailed usage and performance statistics
4. **Theme System**: Custom UI themes and color schemes
5. **Backup Cloud Sync**: Synchronize settings across different installations
6. **Community Profiles**: Share and download community-created profiles

## Compatibility

- Compatible with all existing YimMenuV2 features
- Works with all supported GTA V versions
- Non-breaking implementation - existing functionality unchanged
- Graceful degradation if features are disabled

## Support

These enhancement features are designed to be intuitive and self-explanatory. If you encounter any issues:

1. Check the console logs for error messages
2. Verify file permissions in the enhancements folder
3. Try resetting individual features through the utilities menu
4. Use the "Reset All" option as a last resort

The enhancement system is built with stability and performance in mind, ensuring smooth operation alongside all existing menu features.