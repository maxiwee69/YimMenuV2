#pragma once
#include <map>
#include <string>

namespace YimMenu::Features
{
	// Custom vehicle modification types for LSC system
	enum class CustomVehicleModType
	{
		MOD_PLATE_STYLE = 62,
		MOD_WINDOW_TINT = 63,
		MOD_WHEEL_TYPE = 64
	};

	// Plate style mappings
	inline static const std::map<int, std::string> lsc_plate_styles = {
		{0, "Blue on White 1"},
		{1, "Yellow on Black"},
		{2, "Yellow on Blue"},
		{3, "Blue on White 2"},
		{4, "Blue on White 3"},
		{5, "Yankton"}
	};

	// Window tint mappings
	inline static const std::map<int, std::string> lsc_window_tint_types = {
		{0, "None"},
		{1, "Pure Black"},
		{2, "Dark Smoke"},
		{3, "Light Smoke"},
		{4, "Stock"},
		{5, "Limo"},
		{6, "Green"}
	};

	// Wheel style mappings
	inline static const std::map<int, std::string> lsc_wheel_styles = {
		{-1, "Stock"},
		{0, "Sport"},
		{1, "Muscle"},
		{2, "Lowrider"},
		{3, "SUV"},
		{4, "Offroad"},
		{5, "Tuner"},
		{6, "Bike Wheels"},
		{7, "High End"},
		{8, "Bennys Original"},
		{9, "Bennys Bespoke"},
		{10, "Open Wheel"},
		{11, "Street"},
		{12, "Track"}
	};

	// Missing horn mappings for completion
	inline static const std::map<int, std::string> lsc_missing_horns = {
		{49, "HORN_LOWRIDER3"},
		{50, "HORN_LOWRIDER4"},
		{51, "HORN_LOWRIDER5"}
	};
}
