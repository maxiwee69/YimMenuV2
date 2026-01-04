#include "UnlockEverything.hpp"
#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
    {
    std::shared_ptr<Category> BuildUnlockEverythingMenu()
        {
        auto menu = std::make_shared<Category>("UnlockEverything");
        auto group = std::make_shared<Group>("UnlockEverything");

        group->AddItem(std::make_unique<ImGuiItem>([] {
            static int int_index{};
            static int bool_index{};
            static int float_index{};
            static int packed_stat_int_index{};
            static int packed_stat_bool_index{};
            static int packed_stat_bool_range_index{};
            static int int_bit_index{};
            static int int_bits_range_index{};

            ImGui::Text("Bulk unlockers");
            if (ImGui::Button("Unlock clothes, awards, etc."))
                UnlockEverything::unlock_packed_bools_simple();
            ImGui::SameLine();
            if (ImGui::Button("Unlock paints"))
                UnlockEverything::unlock_paints();
            if (ImGui::Button("Apply misc heist flags"))
                UnlockEverything::set_misc();

            ImGui::Separator();

            if (ImGui::Button("Set ints"))
                UnlockEverything::set_int(int_index);
            ImGui::SameLine();
            ImGui::Text("%d", int_index);

            if (ImGui::Button("Set bools"))
                UnlockEverything::set_bool(bool_index);
            ImGui::SameLine();
            ImGui::Text("%d", bool_index);

            if (ImGui::Button("Set floats"))
                UnlockEverything::set_float(float_index);
            ImGui::SameLine();
            ImGui::Text("%d", float_index);

            if (ImGui::Button("Set packed ints"))
                UnlockEverything::set_packed_stat_int(packed_stat_int_index);
            ImGui::SameLine();
            ImGui::Text("%d", packed_stat_int_index);

            if (ImGui::Button("Set packed bools"))
                UnlockEverything::set_packed_stat_bool(packed_stat_bool_index);
            ImGui::SameLine();
            ImGui::Text("%d", packed_stat_bool_index);

            if (ImGui::Button("Set packed bool ranges"))
                UnlockEverything::set_packed_stat_bool_range(packed_stat_bool_range_index);
            ImGui::SameLine();
            ImGui::Text("%d", packed_stat_bool_range_index);

            if (ImGui::Button("Set int bits"))
                UnlockEverything::set_int_bit(int_bit_index);
            ImGui::SameLine();
            ImGui::Text("%d", int_bit_index);

            if (ImGui::Button("Set int bit ranges"))
                UnlockEverything::set_ints_bit_range(int_bits_range_index);
            ImGui::SameLine();
            ImGui::Text("%d", int_bits_range_index);

            if (ImGui::Button("Reset progress"))
                int_index = bool_index = float_index = packed_stat_int_index = packed_stat_bool_index = packed_stat_bool_range_index = int_bit_index = int_bits_range_index = 0;
            }));

        menu->AddItem(std::move(group));
        return menu;
        }
    }
