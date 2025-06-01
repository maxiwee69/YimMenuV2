#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Scripts.hpp"
#include "types/script/globals/GlobalPlayerBD.hpp"

namespace YimMenu::Features
{
	class RevealPlayers : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			if (auto gpbd = GlobalPlayerBD::Get(); gpbd && Scripts::SafeToModifyFreemodeBroadcastGlobals())
				gpbd->Entries[Self::GetPlayer().GetId()].RevealPlayersActive = true;
		}

		virtual void OnDisable() override
		{
			if (auto gpbd = GlobalPlayerBD::Get(); gpbd && Scripts::SafeToModifyFreemodeBroadcastGlobals())
				gpbd->Entries[Self::GetPlayer().GetId()].RevealPlayersActive = false;
		}
	};

	static RevealPlayers _RevealPlayers{"revealplayers", "Reveal Players", "Reveals all players on the radar, including those hidden from radar"};
}
