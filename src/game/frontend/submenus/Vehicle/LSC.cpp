#include "LSC.hpp"
#include "core/frontend/manager/Category.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/features/vehicle/lsc/LSC.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "imgui.h"

using namespace YimMenu;

namespace YimMenu::Submenus
{
	std::shared_ptr<Category> BuildLSCMenu()
	{
		auto lsc = std::make_shared<Category>("Los Santos Customs");

		auto preparation = std::make_shared<Group>("Preparation");
		auto modSlots = std::make_shared<Group>("Modification Slots");
		auto customOptions = std::make_shared<Group>("Custom Options");

		// Preparation buttons
		preparation->AddItem(std::make_shared<ImGuiItem>([]() {
			if (ImGui::Button("Prepare Current Vehicle"))
			{
				YimMenu::Features::LSC::prepare_vehicle();
			}
			ImGui::SameLine();
			if (YimMenu::Features::LSC::preparing_veh)
			{
				ImGui::Text("Preparing...");
			}
			else if (YimMenu::Features::LSC::current_veh != 0)
			{
				ImGui::Text("Vehicle Ready");
			}
		}));

		// Vehicle information
		preparation->AddItem(std::make_shared<ImGuiItem>([]() {
			if (YimMenu::Features::LSC::current_veh != 0)
			{
				ImGui::Text("Current Vehicle: %d", YimMenu::Features::LSC::current_veh);
				ImGui::Text("Model Hash: 0x%X", YimMenu::Features::LSC::model);
				ImGui::Text("Is Bennys: %s", YimMenu::Features::LSC::is_bennys ? "Yes" : "No");
			}
		}));

		// Modification slots
		modSlots->AddItem(std::make_shared<ImGuiItem>([]() {
			if (YimMenu::Features::LSC::current_veh == 0 || YimMenu::Features::LSC::preparing_veh)
			{
				ImGui::Text("Please prepare a vehicle first");
				return;
			}

			// Display available mod slots
			for (const auto& [slot, name] : YimMenu::Features::LSC::slot_display_names)
			{
				if (ImGui::TreeNode(name.c_str()))
				{
					// Get current mod for this slot
					int currentMod = VEHICLE::GET_VEHICLE_MOD(YimMenu::Features::LSC::current_veh, slot);
					
					// Display available mods for this slot
					if (YimMenu::Features::LSC::mod_display_names.count(slot))
					{
						for (const auto& [modIndex, modName] : YimMenu::Features::LSC::mod_display_names[slot])
						{
							bool isSelected = (currentMod == modIndex);
							if (ImGui::Selectable(modName.c_str(), isSelected))
							{
								// Apply the modification
								VEHICLE::SET_VEHICLE_MOD(YimMenu::Features::LSC::current_veh, slot, modIndex, false);
							}
						}
					}
					ImGui::TreePop();
				}
			}
		}));

		// Custom options (plate style, window tint, wheel type)
		customOptions->AddItem(std::make_shared<ImGuiItem>([]() {
			if (YimMenu::Features::LSC::current_veh == 0 || YimMenu::Features::LSC::preparing_veh)
			{
				ImGui::Text("Please prepare a vehicle first");
				return;
			}

			// Plate Style
			if (ImGui::TreeNode("Plate Style"))
			{
				int currentPlateStyle = VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(YimMenu::Features::LSC::current_veh);
				for (int i = 0; i < 6; i++)
				{
					bool isSelected = (currentPlateStyle == i);
					std::string plateName = "Plate Style " + std::to_string(i);
					if (ImGui::Selectable(plateName.c_str(), isSelected))
					{
						VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(YimMenu::Features::LSC::current_veh, i);
					}
				}
				ImGui::TreePop();
			}

			// Window Tint
			if (ImGui::TreeNode("Window Tint"))
			{
				int currentTint = VEHICLE::GET_VEHICLE_WINDOW_TINT(YimMenu::Features::LSC::current_veh);
				for (int i = 0; i < 7; i++)
				{
					bool isSelected = (currentTint == i);
					std::string tintName = "Tint Level " + std::to_string(i);
					if (ImGui::Selectable(tintName.c_str(), isSelected))
					{
						VEHICLE::SET_VEHICLE_WINDOW_TINT(YimMenu::Features::LSC::current_veh, i);
					}
				}
				ImGui::TreePop();
			}

			// Wheel Type
			if (ImGui::TreeNode("Wheel Type"))
			{
				int currentWheelType = VEHICLE::GET_VEHICLE_WHEEL_TYPE(YimMenu::Features::LSC::current_veh);
				for (int i = -1; i < 13; i++)
				{
					bool isSelected = (currentWheelType == i);
					std::string wheelName = (i == -1) ? "Stock" : "Wheel Type " + std::to_string(i);
					if (ImGui::Selectable(wheelName.c_str(), isSelected))
					{
						VEHICLE::SET_VEHICLE_WHEEL_TYPE(YimMenu::Features::LSC::current_veh, i);
					}
				}
				ImGui::TreePop();
			}
		}));

		lsc->AddItem(preparation);
		lsc->AddItem(modSlots);
		lsc->AddItem(customOptions);

		return lsc;
	}
}
