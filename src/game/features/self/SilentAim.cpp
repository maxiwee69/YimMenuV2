#include "core/commands/LoopedCommand.hpp"
#include "core/commands/BoolCommand.hpp" 
#include "core/util/Math.hpp"
#include "core/util/Joaat.hpp" // Added for model hash checks
#include "game/backend/Self.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Pools.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/pad/ControllerInputs.hpp"
#include <cfloat> // Required for FLT_MAX

namespace YimMenu::Features
{
	static BoolCommand s_targetPolice("silentaimtargetpolice", "Target Police", "Include police/NOOSE/SWAT in silent aim targets", true);
	static BoolCommand s_targetPlayers("silentaimtargetplayers", "Target Players", "Include other players in silent aim targets", true);
	static BoolCommand s_targetEveryone("silentaimtargeteveryone", "Target Everyone", "Target any non-friendly ped", false);

	class SilentAim : public LoopedCommand
	{
	public:
		SilentAim() :
		    LoopedCommand("silentaim", "Silent Aim", "Aims at Peds and Players silently")
		{
		}

		bool m_isShooting = false;

		virtual void OnTick() override
		{
			Ped selfPed = Self::GetPed();
			if (!selfPed || !selfPed.IsValid())
			{
				if (m_isShooting) 
				{
					m_isShooting = false;
				}
				return;
			}

			// Activate only when attacking
			bool isAttackPressed = PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK);
			
			Ped target{Ped(0)};

			if (isAttackPressed)
			{
				target = GetTarget(); 
			}

			if (isAttackPressed && target && target.IsValid())
			{
				Vector3 target_head_pos = target.GetBonePosition(31086); // SKEL_Head
				PED::SET_PED_SHOOTS_AT_COORD(selfPed.GetHandle(), target_head_pos.x, target_head_pos.y, target_head_pos.z, true);
				m_isShooting = true;
			}
			else
			{
				if (m_isShooting)
				{
					// Ensure selfPed is still valid before telling it to stop
					Ped currentSelfPedForStop = Self::GetPed();
					if (currentSelfPedForStop && currentSelfPedForStop.IsValid()) 
					{
						PED::SET_PED_SHOOTS_AT_COORD(currentSelfPedForStop.GetHandle(), 0.f, 0.f, 0.f, false);
					}
					m_isShooting = false;
				}
			}
		}

		virtual void OnDisable() override
		{
			if (m_isShooting)
			{
				Ped selfPed = Self::GetPed();
				if (selfPed && selfPed.IsValid())
				{
					PED::SET_PED_SHOOTS_AT_COORD(selfPed.GetHandle(), 0.f, 0.f, 0.f, false);
				}
				m_isShooting = false;
			}
		}

	private:
		Ped GetTarget()
		{
			Ped bestTarget{Ped(0)};
			float closestDistance = FLT_MAX;

			Ped selfPed = Self::GetPed(); 

			if (!selfPed || !selfPed.IsValid())
				return Ped(0); 
			
			Vector3 selfPosition = selfPed.GetPosition();
			Vector3 selfHandPosition = selfPed.GetBonePosition(28422); 

			auto perform_los_check = [&](Ped targetPed) -> bool {
				if (!targetPed || !targetPed.IsValid()) return false;
				Vector3 targetHeadPosition = targetPed.GetBonePosition(31086); // SKEL_Head
				
				BOOL didHit = FALSE;
				Vector3 endCoords{};
				Vector3 surfaceNormal{};
				::Entity entityHitHandle = 0; // Use raw Entity type (int) and initialize

				int rayHandle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(
					selfHandPosition.x, selfHandPosition.y, selfHandPosition.z,
					targetHeadPosition.x, targetHeadPosition.y, targetHeadPosition.z,
					10, 
					selfPed.GetHandle(),
					7 
				);

				SHAPETEST::GET_SHAPE_TEST_RESULT(rayHandle, &didHit, &endCoords, &surfaceNormal, &entityHitHandle);

				// If it didn't hit anything, LOS is clear to the target's head.
				// If it did hit something, check if that something is the target ped itself.
				if (!didHit)
					return true;
				else if (entityHitHandle != 0)
				{
					// Construct a Ped from the handle to compare
					Ped hitPed = Ped(entityHitHandle);
					return hitPed.IsValid() && hitPed == targetPed;
				}
				return false; // Hit something, but it wasn't a valid ped or not the target ped
			};

			if (s_targetEveryone.GetState())
			{
				for (Ped ped : Pools::GetPeds())
				{
					if (ped && ped.IsValid() && ped != selfPed && !ped.IsDead())
					{
						int relationship = PED::GET_RELATIONSHIP_BETWEEN_PEDS(ped.GetHandle(), selfPed.GetHandle());
						if (true)
						{
							Vector3 pedPosition = ped.GetPosition();
							float distance = MISC::GET_DISTANCE_BETWEEN_COORDS(selfPosition.x, selfPosition.y, selfPosition.z, pedPosition.x, pedPosition.y, pedPosition.z, true);
							if (distance < closestDistance)
							{
								if (perform_los_check(ped))
								{
									bestTarget = ped;
									closestDistance = distance;
								}
							}
						}
					}
				}
			}
			else
			{
				if (s_targetPolice.GetState())
				{
					for (Ped ped : Pools::GetPeds()) 
					{
						if (ped && ped.IsValid() && ped != selfPed && !ped.IsDead())
						{
							Hash model = ENTITY::GET_ENTITY_MODEL(ped.GetHandle());
							int pedType = PED::GET_PED_TYPE(ped.GetHandle());
							bool isPoliceType = (pedType == 6 /*COP*/ || pedType == 20 /*SWAT*/ || pedType == 21 /*ARMY*/ || pedType == 29 /*SPECIAL_PED*/);
							bool isNooseModel = (model == "s_m_y_swat_01"_J || model == "s_m_y_noose_01"_J || model == "u_m_y_noosepatrol_01"_J || model == "u_m_y_noosecom_01"_J);
							bool isHostileRelationship = (PED::GET_RELATIONSHIP_BETWEEN_PEDS(ped.GetHandle(), selfPed.GetHandle()) == 5 /*RELATIONSHIP_HATE*/);

							if (isPoliceType || isNooseModel || isHostileRelationship)
							{
								Vector3 pedPosition = ped.GetPosition(); 
								float distance = MISC::GET_DISTANCE_BETWEEN_COORDS(selfPosition.x, selfPosition.y, selfPosition.z, pedPosition.x, pedPosition.y, pedPosition.z, true);

								if (distance < closestDistance)
								{
									if (perform_los_check(ped))
									{
										bestTarget = ped;
										closestDistance = distance;
									}
								}
							}
						}
					}
				}
			}

			// Check for players - this runs regardless of s_targetEveryone, but respects s_targetPlayers
			if (s_targetPlayers.GetState())
			{
				for (auto& playerPair : Players::GetPlayers())
				{
					Player& player = playerPair.second;
					Ped playerPed = player.GetPed();

					if (playerPed && playerPed.IsValid() && playerPed != selfPed && !playerPed.IsDead())
					{
						Vector3 pedPosition = playerPed.GetPosition(); // Using general position for distance check
						float distance = MISC::GET_DISTANCE_BETWEEN_COORDS(selfPosition.x, selfPosition.y, selfPosition.z, pedPosition.x, pedPosition.y, pedPosition.z, true);

						if (distance < closestDistance)
						{
							if (perform_los_check(playerPed))
							{
								bestTarget = playerPed;
								closestDistance = distance;
							}
						}
					}
				}
			}
			return bestTarget;
		}
	};

	static SilentAim _SilentAim{};
}
