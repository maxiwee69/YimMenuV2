#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Tunables.hpp"

namespace YimMenu::Features
{
	class AllowWeaponsInInteriors : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr auto m_TunableHash = "KICK_OUT_OF_NIGHTCLUB_WITH_WEAPON"_J;
		static constexpr int m_DefaultValue = 1;

		virtual void OnTick() override
		{
			if (auto tunable = Tunables::GetTunable(m_TunableHash))
			{
				*tunable->As<int*>() = 0; // Set to 0 to prevent kicking out with weapons
			}
		}

		virtual void OnDisable() override
		{
			if (auto tunable = Tunables::GetTunable(m_TunableHash))
			{
				*tunable->As<int*>() = m_DefaultValue; // Restore default behavior
			}
		}
	};

	static AllowWeaponsInInteriors _AllowWeaponsInInteriors{"allowweaponsininteriors", "Allow Weapons in Interiors", "Allows you to use weapons in interiors like nightclubs without being kicked out"};
}
