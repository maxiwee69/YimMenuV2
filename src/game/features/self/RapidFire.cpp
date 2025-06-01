#include "core/commands/LoopedCommand.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/util/Math.hpp"
#include "core/util/Joaat.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"
#include "game/frontend/GUI.hpp"
#include "types/pad/ControllerInputs.hpp"
#include "core/commands/Commands.hpp"
#include "game/gta/Pools.hpp"
#include <cfloat>

namespace YimMenu::Features
{
	class RapidFire : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		const std::size_t m_TickRate = 5u;
		std::size_t m_CurrentTick = 0;

		bool CanShoot()
		{
			bool result = (m_CurrentTick == 0);
			m_CurrentTick = (m_CurrentTick + 1) % m_TickRate;
			return result;
		}

		// Helper method to get the same target that SilentAim would use
		Ped GetSilentAimTarget()
		{
			// Access the SilentAim target selection commands
			auto targetPoliceCmd = Commands::GetCommand(Joaat("silentaimtargetpolice"));
			auto targetPlayersCmd = Commands::GetCommand(Joaat("silentaimtargetplayers"));  
			auto targetEveryoneCmd = Commands::GetCommand(Joaat("silentaimtargeteveryone"));

			bool targetPolice = false;
			bool targetPlayers = false;
			bool targetEveryone = false;

			if (targetPoliceCmd)
			{
				auto policeCommand = dynamic_cast<BoolCommand*>(targetPoliceCmd);
				if (policeCommand) targetPolice = policeCommand->GetState();
			}

			if (targetPlayersCmd)
			{
				auto playersCommand = dynamic_cast<BoolCommand*>(targetPlayersCmd);
				if (playersCommand) targetPlayers = playersCommand->GetState();
			}

			if (targetEveryoneCmd)
			{
				auto everyoneCommand = dynamic_cast<BoolCommand*>(targetEveryoneCmd);
				if (everyoneCommand) targetEveryone = everyoneCommand->GetState();
			}

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
				::Entity entityHitHandle = 0;

				int rayHandle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(
					selfHandPosition.x, selfHandPosition.y, selfHandPosition.z,
					targetHeadPosition.x, targetHeadPosition.y, targetHeadPosition.z,
					10, 
					selfPed.GetHandle(),
					7 
				);

				SHAPETEST::GET_SHAPE_TEST_RESULT(rayHandle, &didHit, &endCoords, &surfaceNormal, &entityHitHandle);

				if (!didHit)
					return true;
				else if (entityHitHandle != 0)
				{
					Ped hitPed = Ped(entityHitHandle);
					return hitPed.IsValid() && hitPed == targetPed;
				}
				return false;
			};

			if (targetEveryone)
			{
				for (Ped ped : Pools::GetPeds())
				{
					if (ped && ped.IsValid() && ped != selfPed && !ped.IsDead())
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
			else
			{
				if (targetPolice)
				{
					for (Ped ped : Pools::GetPeds()) 
					{
						if (ped && ped.IsValid() && ped != selfPed && !ped.IsDead())
						{
							Hash model = ENTITY::GET_ENTITY_MODEL(ped.GetHandle());
							int pedType = PED::GET_PED_TYPE(ped.GetHandle());
							bool isPoliceType = (pedType == 6 || pedType == 20 || pedType == 21 || pedType == 29);
							bool isNooseModel = (model == Joaat("s_m_y_swat_01") || model == Joaat("s_m_y_noose_01") || model == Joaat("u_m_y_noosepatrol_01") || model == Joaat("u_m_y_noosecom_01"));
							bool isHostileRelationship = (PED::GET_RELATIONSHIP_BETWEEN_PEDS(ped.GetHandle(), selfPed.GetHandle()) == 5);

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

			// Check for players
			if (targetPlayers)
			{
				for (auto& playerPair : Players::GetPlayers())
				{
					Player& player = playerPair.second;
					Ped playerPed = player.GetPed();

					if (playerPed && playerPed.IsValid() && playerPed != selfPed && !playerPed.IsDead())
					{
						Vector3 pedPosition = playerPed.GetPosition();
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

		virtual void OnTick() override
		{
			if (!HUD::IS_PAUSE_MENU_ACTIVE() && !GUI::IsOpen() && Self::GetPed() && !Self::GetPed().IsDead())
			{
				if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_ATTACK) && CanShoot())
				{
					const auto weapon_entity = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(Self::GetPed().GetHandle(), 0);
					if (!weapon_entity)
						return;

					Vector3 dim_min;
					Vector3 dim_max;
					MISC::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(weapon_entity), &dim_min, &dim_max);

					const auto dimensions = (dim_max - dim_min) * 0.5f;
					const auto weapon_position =
					    ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(weapon_entity, dimensions.x, 0, dimensions.z * 0.3f);

					Vector3 end;
					
					// Check if SilentAim is active
					auto silentAimCommand = Commands::GetCommand(Joaat("silentaim"));
					bool silentAimActive = false;
					if (silentAimCommand)
					{
						auto silentAimBoolCommand = dynamic_cast<BoolCommand*>(silentAimCommand);
						if (silentAimBoolCommand && silentAimBoolCommand->GetState())
						{
							silentAimActive = true;
						}
					}

					if (silentAimActive)
					{
						// When SilentAim is active, find the same target it would use
						// and fire additional bullets at it
						Ped target = GetSilentAimTarget();
						if (target && target.IsValid())
						{
							// Fire at the target's head position (same as SilentAim)
							Vector3 target_head_pos = target.GetBonePosition(31086); // SKEL_Head
							end = target_head_pos;
						}
						else
						{
							// Fallback to camera direction if no target found
							auto camera_direction = Math::RotationToDirection(CAM::GET_GAMEPLAY_CAM_ROT(0));
							auto camera_position  = CAM::GET_GAMEPLAY_CAM_COORD() + camera_direction;
							end = camera_position + camera_direction * 2000.0;
						}
					}
					else
					{
						// Normal rapid fire when SilentAim is not active
						auto camera_direction = Math::RotationToDirection(CAM::GET_GAMEPLAY_CAM_ROT(0));
						auto camera_position  = CAM::GET_GAMEPLAY_CAM_COORD() + camera_direction;

						end = camera_position + camera_direction * 2000.0;

						const auto raycast_handle =
						    SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(camera_position.x,
						        camera_position.y,
						        camera_position.z,
						        end.x,
						        end.y,
						        end.z,
						        -1,
						        0,
						        7);
						BOOL did_raycast_hit{FALSE};
						Vector3 raycast_hit_position{};
						Vector3 raycast_surface_normal_hit_position{};
						int raycast_hit_entity{};
						SHAPETEST::GET_SHAPE_TEST_RESULT(raycast_handle, &did_raycast_hit, &raycast_hit_position, &raycast_surface_normal_hit_position, &raycast_hit_entity);
						if (did_raycast_hit)
						{
							end = raycast_hit_position;
						}
					}

					Hash weapon_hash = Self::GetPed().GetCurrentWeapon();

					MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(
					    weapon_position.x,
					    weapon_position.y,
					    weapon_position.z,
					    end.x,
					    end.y,
					    end.z,
					    WEAPON::GET_WEAPON_DAMAGE(weapon_hash, 0),
					    true,
					    weapon_hash,
					    Self::GetPed().GetHandle(),
					    true,
					    false,
					    -1.0);
				}
			}
		}
	};

	static RapidFire _RapidFire{"rapidfire", "Rapid Fire", "Increases the fire rate of your weapons substantially"};
}