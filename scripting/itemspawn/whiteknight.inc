public Action Command_WhiteKnight(int client, int argc)
{
	switch (g_hCVar_SpawnMode.IntValue)
	{
		case 0:
		{
			ReplyToCommand(client, "[ItemSpawn] This feature is currently disabled.");
			return Plugin_Handled;
		}
		case 1:
		{
			if (!client)
			{
				ReplyToCommand(client, "[ItemSpawn] Cannot spawn items on yourself as server console.");
				return Plugin_Handled;
			}
			else if (argc > 0)
			{
				ReplyToCommand(client, "[ItemSpawn] You can only spawn items on yourself.");
				return Plugin_Handled;
			}
			else
			{
				float fOrigin[3];
				GetClientEyePosition(client, fOrigin);
				SpawnWhiteKnight(fOrigin);
				LogAction(client, -1, "\"%L\" spawned WhiteKnight at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
			}
		}
		case 2:
		{
			float fOrigin[3];

			if (argc < 1)
			{
				if (!client)
				{
					ReplyToCommand(client, "[ItemSpawn] Cannot spawn items on yourself as server console.");
					return Plugin_Handled;
				}
				else
				{
					GetClientEyePosition(client, fOrigin);
					SpawnWhiteKnight(fOrigin);
					LogAction(client, -1, "\"%L\" spawned WhiteKnight at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
				}
			}
			else
			{
				char sArgs[64];
				char sTargetName[MAX_TARGET_LENGTH];
				int iTargets[MAXPLAYERS];
				int iTargetCount;
				bool bIsML;

				GetCmdArg(1, sArgs, sizeof(sArgs));

				if ((iTargetCount = ProcessTargetString(sArgs, client, iTargets, MAXPLAYERS, COMMAND_FILTER_CONNECTED, sTargetName, sizeof(sTargetName), bIsML)) <= 0)
				{
					ReplyToTargetError(client, iTargetCount);
					return Plugin_Handled;
				}

				for (int i = 0; i < iTargetCount; i++)
				{
					if (IsClientInGame(iTargets[i]) && IsPlayerAlive(iTargets[i]) && (ZR_IsClientHuman(iTargets[i])))
					{
						GetClientEyePosition(iTargets[i], fOrigin);
						SpawnWhiteKnight(fOrigin);
						LogAction(client, -1, "\"%L\" gave WhiteKnight to \"%L\".", client, iTargets[i]);
					}
				}
			}
		}
	}
	return Plugin_Handled;
}

public void SpawnWhiteKnight(float fOrigin[3])
{
	float fOriginTemp[3];

	// weapon_knife.
	int iKnife = CreateEntityAtOrigin("weapon_knife", fOrigin);
	DispatchKeyFormat(iKnife, "targetname", "item_knight_knife_%d", g_iCounter);
	DispatchKeyFormat(iKnife, "hammerid", "11051995%d", g_iCounter);
	DispatchKeyFormat(iKnife, "spawnflags", "1");
	DispatchKeyFormat(iKnife, "angles", "0 0 0");
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "!self,AddOutput,renderfx 6,0,1");
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "item_knight_model_%d,SetAnimation,dog_run,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "item_knight_model_%d,SetDefaultAnimation,dog_run,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "item_knight_health_%d,FireUser1,,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "item_knight_health_%d,FireUser2,,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "item_knight_controls_%d,Activate,,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "!activator,AddOutput,health 50000,0,1");
	SpawnAndActivate(iKnife);

	HookSingleEntityOutput(iKnife, "OnPlayerPickup", WhiteKnightPickup, true);

	// model origin.
	fOriginTemp[0] = fOrigin[0] + 6.0;
	fOriginTemp[1] = fOrigin[1];
	fOriginTemp[2] = fOrigin[2] - 32.0;

	// prop_dynamic model.
	int iModel = CreateEntityAtOrigin("prop_dynamic", fOriginTemp);
	DispatchKeyFormat(iModel, "targetname", "item_knight_model_%d", g_iCounter);
	DispatchKeyFormat(iModel, "model", "models/dog_jugger.mdl");
	DispatchKeyFormat(iModel, "disablebonefollowers", "1");
	DispatchKeyFormat(iModel, "defaultanim", "ragdoll");
	DispatchKeyFormat(iModel, "angles", "0 0 0");
	DispatchKeyFormat(iModel, "solid", "0");
	SpawnAndActivate(iModel);
	ParentToEntity(iModel, iKnife);

	// trigger_once strip.
	int iTriggerStrip = CreateEntityAtOrigin("trigger_once", fOrigin);
	DispatchKeyFormat(iTriggerStrip, "targetname", "item_knight_strip_%d", g_iCounter);
	DispatchKeyFormat(iTriggerStrip, "filtername", "item_filter_human");
	DispatchKeyFormat(iTriggerStrip, "spawnflags", "1");
	DispatchKeyFormat(iTriggerStrip, "startdisabled", "1");
	DispatchKeyFormat(iTriggerStrip, "OnStartTouch", "item_spawn_weaponstrip,StripWeaponsAndSuit,,0,1");
	SpawnAndActivate(iTriggerStrip);
	ParentToEntity(iTriggerStrip, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerStrip, view_as<float>({-8.0, -8.0, -8.0}), view_as<float>({8.0, 8.0, 8.0}));
	SetEntityProps(iTriggerStrip);

	// attack origin.
	fOriginTemp[0] = fOrigin[0] + 112.0;
	fOriginTemp[1] = fOrigin[1];
	fOriginTemp[2] = fOrigin[2] + 4.0;

	// trigger_push attack.
	int iTriggerAttack1 = CreateEntityAtOrigin("trigger_push", fOriginTemp);
	DispatchKeyFormat(iTriggerAttack1, "targetname", "item_knight_attack_1_%d", g_iCounter);
	DispatchKeyFormat(iTriggerAttack1, "filtername", "item_filter_zombie");
	DispatchKeyFormat(iTriggerAttack1, "spawnflags", "1");
	DispatchKeyFormat(iTriggerAttack1, "startdisabled", "1");
	DispatchKeyFormat(iTriggerAttack1, "pushdir", "-45 0 0");
	DispatchKeyFormat(iTriggerAttack1, "speed", "600");
	SpawnAndActivate(iTriggerAttack1);
	ParentToEntity(iTriggerAttack1, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerAttack1, view_as<float>({-70.0, -52.0, -36.0}), view_as<float>({70.0, 52.0, 36.0}));
	SetEntityProps(iTriggerAttack1);

	// trigger_push attack.
	int iTriggerAttack2 = CreateEntityAtOrigin("trigger_push", fOriginTemp);
	DispatchKeyFormat(iTriggerAttack2, "targetname", "item_knight_attack_2_%d", g_iCounter);
	DispatchKeyFormat(iTriggerAttack2, "filtername", "item_filter_zombie");
	DispatchKeyFormat(iTriggerAttack2, "spawnflags", "1");
	DispatchKeyFormat(iTriggerAttack2, "startdisabled", "1");
	DispatchKeyFormat(iTriggerAttack2, "pushdir", "-45 0 0");
	DispatchKeyFormat(iTriggerAttack2, "speed", "1500");
	SpawnAndActivate(iTriggerAttack2);
	ParentToEntity(iTriggerAttack2, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerAttack2, view_as<float>({-70.0, -52.0, -36.0}), view_as<float>({70.0, 52.0, 36.0}));
	SetEntityProps(iTriggerAttack2);

	// attack origin.
	fOriginTemp[0] = fOrigin[0] + 130.0;
	fOriginTemp[1] = fOrigin[1];
	fOriginTemp[2] = fOrigin[2] + 4.0;

	// trigger_hurt attack.
	int iTriggerAttack3 = CreateEntityAtOrigin("trigger_hurt", fOriginTemp);
	DispatchKeyFormat(iTriggerAttack3, "targetname", "item_knight_attack_3_%d", g_iCounter);
	DispatchKeyFormat(iTriggerAttack3, "filtername", "item_filter_human_ignored");
	DispatchKeyFormat(iTriggerAttack3, "spawnflags", "9");
	DispatchKeyFormat(iTriggerAttack3, "startdisabled", "1");
	DispatchKeyFormat(iTriggerAttack3, "damagetype", "512");
	DispatchKeyFormat(iTriggerAttack3, "damagemodel", "0");
	DispatchKeyFormat(iTriggerAttack3, "damagecap", "20");
	DispatchKeyFormat(iTriggerAttack3, "damage", "800");
	SpawnAndActivate(iTriggerAttack3);
	ParentToEntity(iTriggerAttack3, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerAttack3, view_as<float>({-52.0, -52.0, -36.0}), view_as<float>({52.0, 52.0, 36.0}));
	SetEntityProps(iTriggerAttack3);

	// nuke origin.
	fOriginTemp[0] = fOrigin[0] + 6.0;
	fOriginTemp[1] = fOrigin[1];
	fOriginTemp[2] = fOrigin[2] + 32.0;

	// trigger_hurt nuke.
	int iTriggerNuke = CreateEntityAtOrigin("trigger_hurt", fOriginTemp);
	DispatchKeyFormat(iTriggerNuke, "targetname", "item_knight_nuke_%d", g_iCounter);
	DispatchKeyFormat(iTriggerNuke, "filtername", "item_filter_human_ignored");
	DispatchKeyFormat(iTriggerNuke, "spawnflags", "9");
	DispatchKeyFormat(iTriggerNuke, "startdisabled", "1");
	DispatchKeyFormat(iTriggerNuke, "damagetype", "512");
	DispatchKeyFormat(iTriggerNuke, "damagemodel", "0");
	DispatchKeyFormat(iTriggerNuke, "damagecap", "20");
	DispatchKeyFormat(iTriggerNuke, "damage", "20000");
	SpawnAndActivate(iTriggerNuke);
	ParentToEntity(iTriggerNuke, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerNuke, view_as<float>({-1024.0, -1024.0, -800.0}), view_as<float>({1024.0, 1024.0, 800.0}));
	SetEntityProps(iTriggerNuke);

	// ambient_generic attack.
	int iSoundAttack1 = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSoundAttack1, "targetname", "item_knight_attack_sound_1_%d", g_iCounter);
	DispatchKeyFormat(iSoundAttack1, "spawnflags", "48");
	DispatchKeyFormat(iSoundAttack1, "radius", "1250");
	DispatchKeyFormat(iSoundAttack1, "sourceentityname", "item_knight_knife_%d", g_iCounter);
	DispatchKeyFormat(iSoundAttack1, "message", "npc/roller/blade_out.wav");
	DispatchKeyFormat(iSoundAttack1, "volume", "10");
	DispatchKeyFormat(iSoundAttack1, "health", "10");
	DispatchKeyFormat(iSoundAttack1, "pitch", "100");
	DispatchKeyFormat(iSoundAttack1, "pitchstart", "100");
	SpawnAndActivate(iSoundAttack1);
	ParentToEntity(iSoundAttack1, iKnife);

	// ambient_generic attack.
	int iSoundAttack2 = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSoundAttack2, "targetname", "item_knight_attack_sound_2_%d", g_iCounter);
	DispatchKeyFormat(iSoundAttack2, "spawnflags", "48");
	DispatchKeyFormat(iSoundAttack2, "radius", "1250");
	DispatchKeyFormat(iSoundAttack2, "sourceentityname", "item_knight_knife_%d", g_iCounter);
	DispatchKeyFormat(iSoundAttack2, "message", "npc/strider/strider_skewer1.wav");
	DispatchKeyFormat(iSoundAttack2, "volume", "10");
	DispatchKeyFormat(iSoundAttack2, "health", "10");
	DispatchKeyFormat(iSoundAttack2, "pitch", "100");
	DispatchKeyFormat(iSoundAttack2, "pitchstart", "100");
	SpawnAndActivate(iSoundAttack2);
	ParentToEntity(iSoundAttack2, iKnife);

	// ambient_generic nuke.
	int iSoundNuke1 = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSoundNuke1, "targetname", "item_knight_nuke_sound_1_%d", g_iCounter);
	DispatchKeyFormat(iSoundNuke1, "spawnflags", "49");
	DispatchKeyFormat(iSoundNuke1, "radius", "1250");
	DispatchKeyFormat(iSoundNuke1, "sourceentityname", "item_knight_knife_%d", g_iCounter);
	DispatchKeyFormat(iSoundNuke1, "message", "ambient/levels/citadel/portal_beam_shoot4.wav");
	DispatchKeyFormat(iSoundNuke1, "volume", "10");
	DispatchKeyFormat(iSoundNuke1, "health", "10");
	DispatchKeyFormat(iSoundNuke1, "pitch", "100");
	DispatchKeyFormat(iSoundNuke1, "pitchstart", "100");
	SpawnAndActivate(iSoundNuke1);
	ParentToEntity(iSoundNuke1, iKnife);

	// ambient_generic nuke.
	int iSoundNuke2 = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSoundNuke2, "targetname", "item_knight_nuke_sound_2_%d", g_iCounter);
	DispatchKeyFormat(iSoundNuke2, "spawnflags", "49");
	DispatchKeyFormat(iSoundNuke2, "radius", "1250");
	DispatchKeyFormat(iSoundNuke2, "sourceentityname", "item_knight_knife_%d", g_iCounter);
	DispatchKeyFormat(iSoundNuke2, "message", "ambient/explosions/explode_6.wav");
	DispatchKeyFormat(iSoundNuke2, "volume", "10");
	DispatchKeyFormat(iSoundNuke2, "health", "10");
	DispatchKeyFormat(iSoundNuke2, "pitch", "100");
	DispatchKeyFormat(iSoundNuke2, "pitchstart", "100");
	DispatchKeyFormat(iSoundNuke2, "fadein", "0");
	DispatchKeyFormat(iSoundNuke2, "fadeout", "0");
	SpawnAndActivate(iSoundNuke2);
	ParentToEntity(iSoundNuke2, iKnife);

	// logic_branch attack.
	int iBranchAttack = CreateEntityAtOrigin("logic_branch", fOrigin);
	DispatchKeyFormat(iBranchAttack, "targetname", "item_knight_attack_branch_%d", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "spawnflags", "0");
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_attack_case_1_%d,PickRandom,,0,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_attack_sound_1_%d,PlaySound,,0.3,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_3_%d,AddOutput,damage 800,0.3,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_attack_1_%d,Enable,,0.31,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_attack_3_%d,Enable,,0.31,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_attack_1_%d,Disable,,0.5,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_attack_3_%d,Disable,,0.5,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_attack_compare_%d,SetValue,0,1,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_attack_compare_%d,SetCompareValue,0,1,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_shield_compare_%d,SetCompareValue,0,1,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnFalse", "item_knight_nuke_compare_%d,SetCompareValue,0,1,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_spawn_speedmod,ModifySpeed,0,0,-1");
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_case_2_%d,PickRandom,,0,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_sound_2_%d,PlaySound,,1,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_3_%d,AddOutput,damage 1500,1,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_2_%d,Enable,,1.01,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_3_%d,Enable,,1.01,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_2_%d,Disable,,1.2,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_3_%d,Disable,,1.2,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_attack_compare_%d,SetValue,0,2,-1", g_iCounter);
	DispatchKeyFormat(iBranchAttack, "OnTrue", "item_knight_walk_%d,Trigger,,2,-1", g_iCounter);
	SpawnAndActivate(iBranchAttack);
	ParentToEntity(iBranchAttack, iKnife);

	// logic_case attack.
	int iCaseAttack1 = CreateEntityAtOrigin("logic_case", fOrigin);
	DispatchKeyFormat(iCaseAttack1, "targetname", "item_knight_attack_case_1_%d", g_iCounter);
	DispatchKeyFormat(iCaseAttack1, "spawnflags", "0");
	DispatchKeyFormat(iCaseAttack1, "Case01", "1");
	DispatchKeyFormat(iCaseAttack1, "Case02", "2");
	DispatchKeyFormat(iCaseAttack1, "Case03", "3");
	DispatchKeyFormat(iCaseAttack1, "OnCase01", "item_knight_model_%d,SetAnimation,dog_sword1,0,-1", g_iCounter);
	DispatchKeyFormat(iCaseAttack1, "OnCase02", "item_knight_model_%d,SetAnimation,dog_sword2,0,-1", g_iCounter);
	DispatchKeyFormat(iCaseAttack1, "OnCase03", "item_knight_model_%d,SetAnimation,dog_sword3,0,-1", g_iCounter);
	SpawnAndActivate(iCaseAttack1);
	ParentToEntity(iCaseAttack1, iKnife);

	// logic_case attack.
	int iCaseAttack2 = CreateEntityAtOrigin("logic_case", fOrigin);
	DispatchKeyFormat(iCaseAttack2, "targetname", "item_knight_attack_case_2_%d", g_iCounter);
	DispatchKeyFormat(iCaseAttack2, "spawnflags", "0");
	DispatchKeyFormat(iCaseAttack2, "Case01", "1");
	DispatchKeyFormat(iCaseAttack2, "Case02", "2");
	DispatchKeyFormat(iCaseAttack2, "OnCase01", "item_knight_model_%d,SetAnimation,dog_sword1heavy,0,-1", g_iCounter);
	DispatchKeyFormat(iCaseAttack2, "OnCase02", "item_knight_model_%d,SetAnimation,dog_sword2heavy,0,-1", g_iCounter);
	SpawnAndActivate(iCaseAttack2);
	ParentToEntity(iCaseAttack2, iKnife);

	// logic_compare attack.
	int iCompareAttack = CreateEntityAtOrigin("logic_compare", fOrigin);
	DispatchKeyFormat(iCompareAttack, "targetname", "item_knight_attack_compare_%d", g_iCounter);
	DispatchKeyFormat(iCompareAttack, "spawnflags", "0");
	DispatchKeyFormat(iCompareAttack, "OnEqualTo", "!self,SetValue,1,0,-1");
	DispatchKeyFormat(iCompareAttack, "OnEqualTo", "item_knight_attack_compare_%d,SetCompareValue,-1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareAttack, "OnEqualTo", "item_knight_attack_branch_%d,Test,,0,-1", g_iCounter);
	SpawnAndActivate(iCompareAttack);
	ParentToEntity(iCompareAttack, iKnife);

	// logic_compare shield.
	int iCompareShield = CreateEntityAtOrigin("logic_compare", fOrigin);
	DispatchKeyFormat(iCompareShield, "targetname", "item_knight_shield_compare_%d", g_iCounter);
	DispatchKeyFormat(iCompareShield, "spawnflags", "0");
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "!self,SetValue,1,0,-1");
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "!self,SetValue,0,6,-1");
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "item_spawn_speedmod,ModifySpeed,0,0,-1");
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "item_knight_attack_compare_%d,SetCompareValue,-1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "item_knight_shield_compare_%d,SetCompareValue,-1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "item_knight_model_%d,SetAnimation,dog_defense,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "item_knight_model_%d,SetDefaultAnimation,dog_defense,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "item_knight_health_%d,SetDamageFilter,item_filter_nodamage,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "item_knight_health_%d,SetDamageFilter,item_filter_zombies,2,-1", g_iCounter);
	DispatchKeyFormat(iCompareShield, "OnEqualTo", "item_knight_walk_%d,Trigger,,2,-1", g_iCounter);
	SpawnAndActivate(iCompareShield);
	ParentToEntity(iCompareShield, iKnife);

	// logic_compare nuke.
	int iCompareNuke = CreateEntityAtOrigin("logic_compare", fOrigin);
	DispatchKeyFormat(iCompareNuke, "targetname", "item_knight_nuke_compare_%d", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "spawnflags", "0");
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "!self,SetValue,1,0,-1");
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "!self,SetValue,0,150,-1");
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_spawn_speedmod,ModifySpeed,0,0,-1");
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_attack_compare_%d,SetCompareValue,-1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_shield_compare_%d,SetCompareValue,-1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_nuke_compare_%d,SetCompareValue,-1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_model_%d,SetAnimation,dog_ultimated,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_walk_%d,Disable,,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_nuke_sound_1_%d,PlaySound,,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_nuke_sound_2_%d,PlaySound,,5.1,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_nuke_%d,Enable,,5.1,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_nuke_%d,Disable,,5.3,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_walk_%d,Enable,,5.5,-1", g_iCounter);
	DispatchKeyFormat(iCompareNuke, "OnEqualTo", "item_knight_walk_%d,Trigger,,5.51,-1", g_iCounter);
	SpawnAndActivate(iCompareNuke);
	ParentToEntity(iCompareNuke, iKnife);

	// logic_relay combo 1.
	int iRelayCombo1 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayCombo1, "targetname", "item_knight_combo_1_%d", g_iCounter);
	DispatchKeyFormat(iRelayCombo1, "spawnflags", "0");
	DispatchKeyFormat(iRelayCombo1, "OnTrigger", "item_knight_combo_2_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayCombo1, "OnTrigger", "item_knight_combo_2_%d,Disable,,0.3,-1", g_iCounter);
	SpawnAndActivate(iRelayCombo1);
	ParentToEntity(iRelayCombo1, iKnife);

	// logic_relay combo 2.
	int iRelayCombo2 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayCombo2, "targetname", "item_knight_combo_2_%d", g_iCounter);
	DispatchKeyFormat(iRelayCombo2, "spawnflags", "0");
	DispatchKeyFormat(iRelayCombo2, "startdisabled", "1");
	DispatchKeyFormat(iRelayCombo2, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayCombo2, "OnTrigger", "item_knight_combo_3_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayCombo2, "OnTrigger", "item_knight_combo_3_%d,Disable,,0.3,-1", g_iCounter);
	SpawnAndActivate(iRelayCombo2);
	ParentToEntity(iRelayCombo2, iKnife);

	// logic_relay combo 3.
	int iRelayCombo3 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayCombo3, "targetname", "item_knight_combo_3_%d", g_iCounter);
	DispatchKeyFormat(iRelayCombo3, "spawnflags", "0");
	DispatchKeyFormat(iRelayCombo3, "startdisabled", "1");
	DispatchKeyFormat(iRelayCombo3, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayCombo3, "OnTrigger", "item_knight_combo_4_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayCombo3, "OnTrigger", "item_knight_combo_4_%d,Disable,,0.3,-1", g_iCounter);
	SpawnAndActivate(iRelayCombo3);
	ParentToEntity(iRelayCombo3, iKnife);

	// logic_relay combo 4.
	int iRelayCombo4 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayCombo4, "targetname", "item_knight_combo_4_%d", g_iCounter);
	DispatchKeyFormat(iRelayCombo4, "spawnflags", "0");
	DispatchKeyFormat(iRelayCombo4, "startdisabled", "1");
	DispatchKeyFormat(iRelayCombo4, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayCombo4, "OnTrigger", "item_knight_combo_5_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayCombo4, "OnTrigger", "item_knight_combo_5_%d,Disable,,0.3,-1", g_iCounter);
	SpawnAndActivate(iRelayCombo4);
	ParentToEntity(iRelayCombo4, iKnife);

	// logic_relay combo 5.
	int iRelayCombo5 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayCombo5, "targetname", "item_knight_combo_5_%d", g_iCounter);
	DispatchKeyFormat(iRelayCombo5, "spawnflags", "0");
	DispatchKeyFormat(iRelayCombo5, "startdisabled", "1");
	DispatchKeyFormat(iRelayCombo5, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayCombo5, "OnTrigger", "item_knight_combo_6_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayCombo5, "OnTrigger", "item_knight_combo_6_%d,Disable,,0.3,-1", g_iCounter);
	SpawnAndActivate(iRelayCombo5);
	ParentToEntity(iRelayCombo5, iKnife);

	// logic_relay combo 6.
	int iRelayCombo6 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayCombo6, "targetname", "item_knight_combo_6_%d", g_iCounter);
	DispatchKeyFormat(iRelayCombo6, "spawnflags", "0");
	DispatchKeyFormat(iRelayCombo6, "startdisabled", "1");
	DispatchKeyFormat(iRelayCombo6, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayCombo6, "OnTrigger", "item_knight_combo_7_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayCombo6, "OnTrigger", "item_knight_combo_7_%d,Disable,,0.3,-1", g_iCounter);
	SpawnAndActivate(iRelayCombo6);
	ParentToEntity(iRelayCombo6, iKnife);

	// logic_relay combo 7.
	int iRelayCombo7 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayCombo7, "targetname", "item_knight_combo_7_%d", g_iCounter);
	DispatchKeyFormat(iRelayCombo7, "spawnflags", "0");
	DispatchKeyFormat(iRelayCombo7, "startdisabled", "1");
	DispatchKeyFormat(iRelayCombo7, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayCombo7, "OnTrigger", "item_knight_combo_8_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayCombo7, "OnTrigger", "item_knight_combo_8_%d,Disable,,0.3,-1", g_iCounter);
	SpawnAndActivate(iRelayCombo7);
	ParentToEntity(iRelayCombo7, iKnife);

	// logic_relay combo 8.
	int iRelayCombo8 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayCombo8, "targetname", "item_knight_combo_8_%d", g_iCounter);
	DispatchKeyFormat(iRelayCombo8, "spawnflags", "0");
	DispatchKeyFormat(iRelayCombo8, "startdisabled", "1");
	DispatchKeyFormat(iRelayCombo8, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayCombo8, "OnTrigger", "item_knight_nuke_compare_%d,Compare,,0,-1", g_iCounter);
	SpawnAndActivate(iRelayCombo8);
	ParentToEntity(iRelayCombo8, iKnife);

	// logic_relay walk.
	int iRelayWalk = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayWalk, "targetname", "item_knight_walk_%d", g_iCounter);
	DispatchKeyFormat(iRelayWalk, "spawnflags", "0");
	DispatchKeyFormat(iRelayWalk, "OnTrigger", "item_knight_model_%d,SetAnimation,dog_run,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayWalk, "OnTrigger", "item_knight_model_%d,SetDefaultAnimation,dog_run,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayWalk, "OnTrigger", "item_knight_attack_compare_%d,SetCompareValue,0,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayWalk, "OnTrigger", "item_knight_shield_compare_%d,SetCompareValue,0,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayWalk, "OnTrigger", "item_knight_nuke_compare_%d,SetCompareValue,0,0,-1", g_iCounter);
	DispatchKeyFormat(iRelayWalk, "OnTrigger", "item_spawn_speedmod,ModifySpeed,1,0,-1");
	SpawnAndActivate(iRelayWalk);
	ParentToEntity(iRelayWalk, iKnife);

	// logic_relay death.
	int iRelayDeath = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayDeath, "targetname", "item_knight_death_%d", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "spawnflags", "0");
	DispatchKeyFormat(iRelayDeath, "OnTrigger", "!self,FireUser1,,0.1,-1");
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_knife_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_strip_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_3_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_nuke_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_sound_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_sound_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_nuke_sound_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_nuke_sound_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_branch_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_case_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_case_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_attack_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_shield_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_nuke_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_combo_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_combo_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_combo_3_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_combo_4_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_combo_5_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_combo_6_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_combo_7_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_combo_8_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_walk_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "item_knight_controls_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1", "!activator,AddOutput,rendermode 0,0,1");
	DispatchKeyFormat(iRelayDeath, "OnUser1", "!activator,AddOutput,renderfx 0,0,1");
	DispatchKeyFormat(iRelayDeath, "OnUser1", "!activator,SetDamageFilter,,0,1");
	DispatchKeyFormat(iRelayDeath, "OnUser1", "!activator,SetHealth,0,0.02,-1");
	DispatchKeyFormat(iRelayDeath, "OnUser1", "!self,Kill,,0,1");
	SpawnAndActivate(iRelayDeath);
	ParentToEntity(iRelayDeath, iKnife);

	// health origin.
	fOriginTemp[0] = fOrigin[0] + 6.0;
	fOriginTemp[1] = fOrigin[1];
	fOriginTemp[2] = fOrigin[2] - 80.0;

	// func_physbox_multiplayer health.
	int iHealth = CreateEntityAtOrigin("func_physbox_multiplayer", fOriginTemp);
	DispatchKeyFormat(iHealth, "targetname", "item_knight_health_%d", g_iCounter);
	DispatchKeyFormat(iHealth, "model", "models/props/cs_militia/crate_extrasmallmill.mdl");
	DispatchKeyFormat(iHealth, "spawnflags", "9237504");
	DispatchKeyFormat(iHealth, "rendermode", "0");
	DispatchKeyFormat(iHealth, "renderfx", "0");
	DispatchKeyFormat(iHealth, "rendercolor", "255 255 255");
	DispatchKeyFormat(iHealth, "renderamt", "255 255 255");
	DispatchKeyFormat(iHealth, "propdata", "0");
	DispatchKeyFormat(iHealth, "pressuredelay", "0");
	DispatchKeyFormat(iHealth, "preferredcarryangles", "0 0 0");
	DispatchKeyFormat(iHealth, "performancemode", "1");
	DispatchKeyFormat(iHealth, "notsolid", "0");
	DispatchKeyFormat(iHealth, "nodamageforces", "0");
	DispatchKeyFormat(iHealth, "material", "2");
	DispatchKeyFormat(iHealth, "massScale", "0");
	DispatchKeyFormat(iHealth, "health", "750");
	DispatchKeyFormat(iHealth, "gibdir", "0 0 0");
	DispatchKeyFormat(iHealth, "forcetoenablemotion", "0");
	DispatchKeyFormat(iHealth, "explosion", "0");
	DispatchKeyFormat(iHealth, "exploderadius", "0");
	DispatchKeyFormat(iHealth, "explodemagnitude", "0");
	DispatchKeyFormat(iHealth, "explodedamage", "0");
	DispatchKeyFormat(iHealth, "disableshadows", "1");
	DispatchKeyFormat(iHealth, "disablereceiveshadows", "1");
	DispatchKeyFormat(iHealth, "damagetype", "0");
	DispatchKeyFormat(iHealth, "damagetoenablemotion", "0");
	DispatchKeyFormat(iHealth, "damagefilter", "item_filter_zombies");
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_knife_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_strip_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_3_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_nuke_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_sound_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_sound_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_nuke_sound_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_nuke_sound_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_branch_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_case_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_case_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_attack_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_shield_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_nuke_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_combo_1_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_combo_2_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_combo_3_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_combo_4_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_combo_5_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_combo_6_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_combo_7_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_combo_8_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_walk_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_controls_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_model_%d,ClearParent,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_model_%d,SetAnimation,dog_die,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak", "item_knight_model_%d,SetDefaultAnimation,dog_die_loop,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnPlayerUse", "item_knight_attack_branch_%d,Toggle,,0,-1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser1", "!activator,SetDamageFilter,item_filter_zombie_ignored,0,-1");
	DispatchKeyFormat(iHealth, "OnUser1", "!activator,AddOutput,rendermode 10,0,-1");
	DispatchKeyFormat(iHealth, "OnUser1", "!activator,AddOutput,renderfx 6,0,-1");
	DispatchKeyFormat(iHealth, "OnUser1", "!self,FireUser1,,1,-1");
	DispatchKeyFormat(iHealth, "OnUser2", "item_knight_death_%d,CancelPending,,0,-1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser2", "item_knight_death_%d,Trigger,,0.05,-1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser2", "!self,FireUser2,,0.1,-1");
	SpawnAndActivate(iHealth);
	ParentToEntity(iHealth, iKnife);

	// make the physbox work.
	SetEntityBBox(iHealth, view_as<float>({-36.0, -52.0, 48.0}), view_as<float>({36.0, 52.0, 160.0}));
	SetEntityProps(iHealth);

	HookSingleEntityOutput(iHealth, "OnBreak", WhiteKnightKill, true);

	// game_ui.
	int iControls = CreateEntityAtOrigin("game_ui", fOrigin);
	DispatchKeyFormat(iControls, "targetname", "item_knight_controls_%d", g_iCounter);
	DispatchKeyFormat(iControls, "spawnflags", "0");
	DispatchKeyFormat(iControls, "fieldofview", "-1.0");
	DispatchKeyFormat(iControls, "PressedAttack", "item_knight_attack_compare_%d,Compare,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedAttack2", "item_knight_shield_compare_%d,Compare,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedBack", "item_knight_combo_5_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedBack", "item_knight_combo_7_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedForward", "item_knight_combo_6_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedForward", "item_knight_combo_8_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedMoveLeft", "item_knight_combo_1_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedMoveLeft", "item_knight_combo_2_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedMoveRight", "item_knight_combo_3_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedMoveRight", "item_knight_combo_4_%d,Trigger,,0,-1", g_iCounter);
	SpawnAndActivate(iControls);
	ParentToEntity(iControls, iKnife);

	AcceptEntityInput(iTriggerStrip, "Enable");
	g_iCounter++;
}

public void WhiteKnightPickup(const char[] output, int caller, int activator, float delay)
{
	ServerCommand("say ** %N has picked up the White Knight **", activator);
	PrintToChat(activator, " LEFT CLICK = ATTACK and RIGHT CLICK = SHIELD.");
	CreateTimer(2.0, EquipWeapons, GetClientUserId(activator), TIMER_FLAG_NO_MAPCHANGE);
}

public void WhiteKnightKill(const char[] output, int caller, int activator, float delay)
{
	if (IsValidClient(activator))
		ServerCommand("say ** %N has killed the White Knight **", activator);
}