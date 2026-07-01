public Action Command_Balrog(int client, int argc) {
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
				SpawnBalrog(fOrigin);
				LogAction(client, -1, "\"%L\" spawned Balrog at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
					SpawnBalrog(fOrigin);
					LogAction(client, -1, "\"%L\" spawned Balrog at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
					if (IsClientInGame(iTargets[i]) && IsPlayerAlive(iTargets[i]) && (ZR_IsClientZombie(iTargets[i])))
					{
						GetClientEyePosition(iTargets[i], fOrigin);
						SpawnBalrog(fOrigin);
						LogAction(client, -1, "\"%L\" gave Balrog to \"%L\".", client, iTargets[i]);
					}
				}
			}
		}
	}
	return Plugin_Handled;
}

public void SpawnBalrog(float fOrigin[3]) {
	float fOriginTemp[3];

	// weapon_knife.
	int iKnife = CreateEntityAtOrigin("weapon_knife", fOrigin);
	DispatchKeyFormat(iKnife, "targetname",             "item_balrog_knife_%d", g_iCounter);
	DispatchKeyFormat(iKnife, "hammerid",               "11051995%d", g_iCounter);
	DispatchKeyFormat(iKnife, "spawnflags",             "1");
	DispatchKeyFormat(iKnife, "angles",                 "0 0 0");
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "!self,AddOutput,renderfx 6,0,1");
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "item_balrog_model_%d,SetAnimation,balrog_attack1,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "item_balrog_model_%d,SetDefaultAnimation,crouch_walk_lower,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "item_balrog_health_%d,FireUser1,,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "item_balrog_health_%d,FireUser2,,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "item_balrog_health_%d,FireUser3,,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "item_balrog_controls_%d,Activate,,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "!activator,AddOutput,health 50000,0,1");
	DispatchKeyFormat(iKnife, "OnPlayerPickup",         "item_spawn_speedmod,ModifySpeed,0.8,0,1");
	SpawnAndActivate(iKnife);

	HookSingleEntityOutput(iKnife, "OnPlayerPickup", BalrogPickup, true);

	// model origin.
	fOriginTemp[0] = fOrigin[0] + 4.5;
	fOriginTemp[1] = fOrigin[1] + 1.26;
	fOriginTemp[2] = fOrigin[2] - 28.9;

	// prop_dynamic model.
	int iModel = CreateEntityAtOrigin("prop_dynamic", fOriginTemp);
	DispatchKeyFormat(iModel, "targetname",             "item_balrog_model_%d", g_iCounter);
	DispatchKeyFormat(iModel, "model",                  "models/player/slow/amberlyn/lotr/balrog/balrog_rafuron_hannibal.mdl");
	DispatchKeyFormat(iModel, "disablebonefollowers",   "1");
	DispatchKeyFormat(iModel, "defaultanim",            "crouch_idle_lower");
	DispatchKeyFormat(iModel, "angles",                 "0 2 0");
	DispatchKeyFormat(iModel, "solid",                  "0");
	SpawnAndActivate(iModel);
	ParentToEntity(iModel, iKnife);

	// trigger_once strip.
	int iTriggerStrip = CreateEntityAtOrigin("trigger_once", fOrigin);
	DispatchKeyFormat(iTriggerStrip, "targetname",      "item_balrog_strip_%d", g_iCounter);
	DispatchKeyFormat(iTriggerStrip, "filtername",      "item_filter_zombie");
	DispatchKeyFormat(iTriggerStrip, "spawnflags",      "1");
	DispatchKeyFormat(iTriggerStrip, "startdisabled",   "1");
	DispatchKeyFormat(iTriggerStrip, "OnStartTouch",    "item_spawn_weaponstrip,StripWeaponsAndSuit,,0,1");
	SpawnAndActivate(iTriggerStrip);
	ParentToEntity(iTriggerStrip, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerStrip, view_as<float>({-8.0, -8.0, -8.0}), view_as<float>({8.0, 8.0, 8.0}));
	SetEntityProps(iTriggerStrip);

	// walk origin.
	fOriginTemp[0] = fOrigin[0] + 28.5;
	fOriginTemp[1] = fOrigin[1] - 2.74;
	fOriginTemp[2] = fOrigin[2] - 4.9;

	// trigger_hurt walk.
	int iTriggerWalk = CreateEntityAtOrigin("trigger_hurt", fOriginTemp);
	DispatchKeyFormat(iTriggerWalk, "targetname",       "item_balrog_walk_%d", g_iCounter);
	DispatchKeyFormat(iTriggerWalk, "filtername",       "item_filter_zombie_ignored");
	DispatchKeyFormat(iTriggerWalk, "spawnflags",       "9");
	DispatchKeyFormat(iTriggerWalk, "startdisabled",    "1");
	DispatchKeyFormat(iTriggerWalk, "damagetype",       "128");
	DispatchKeyFormat(iTriggerWalk, "damagemodel",      "0");
	DispatchKeyFormat(iTriggerWalk, "damagecap",        "20");
	DispatchKeyFormat(iTriggerWalk, "damage",           "190");
	SpawnAndActivate(iTriggerWalk);
	ParentToEntity(iTriggerWalk, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerWalk, view_as<float>({-32.0, -72.0, -84.0}), view_as<float>({32.0, 72.0, 84.0}));
	SetEntityProps(iTriggerWalk);

	// roar origin.
	fOriginTemp[0] = fOrigin[0] - 15.5;
	fOriginTemp[1] = fOrigin[1] + 5.0;
	fOriginTemp[2] = fOrigin[2] + 31.0;

	// trigger_hurt roar.
	int iTriggerRoar = CreateEntityAtOrigin("trigger_hurt", fOriginTemp);
	DispatchKeyFormat(iTriggerRoar, "targetname",       "item_balrog_roar_%d", g_iCounter);
	DispatchKeyFormat(iTriggerRoar, "filtername",       "item_filter_zombie");
	DispatchKeyFormat(iTriggerRoar, "spawnflags",       "1");
	DispatchKeyFormat(iTriggerRoar, "startdisabled",    "1");
	DispatchKeyFormat(iTriggerRoar, "damagetype",       "0");
	DispatchKeyFormat(iTriggerRoar, "damagemodel",      "0");
	DispatchKeyFormat(iTriggerRoar, "damagecap",        "20");
	DispatchKeyFormat(iTriggerRoar, "damage",           "-20000");
	DispatchKeyFormat(iTriggerRoar, "OnStartTouch",     "!activator,AddOutput,max_health 10000,0,-1");
	DispatchKeyFormat(iTriggerRoar, "OnStartTouch",     "!activator,AddOutput,max_health 100,0.02,-1");
	DispatchKeyFormat(iTriggerRoar, "OnStartTouch",     "!activator,AddOutput,rendercolor 255 0 0,0,-1");
	DispatchKeyFormat(iTriggerRoar, "OnStartTouch",     "!activator,AddOutput,rendercolor 255 255 255,10,-1");
	DispatchKeyFormat(iTriggerRoar, "OnStartTouch",     "item_spawn_speedmod,ModifySpeed,1.5,0,-1");
	DispatchKeyFormat(iTriggerRoar, "OnStartTouch",     "item_spawn_speedmod,ModifySpeed,1.0,10,-1");
	SpawnAndActivate(iTriggerRoar);
	ParentToEntity(iTriggerRoar, iKnife);
	AddItemFilter(iTriggerRoar);

	// make the trigger work.
	SetEntityBBox(iTriggerRoar, view_as<float>({-736.0, -736.0, -560.0}), view_as<float>({736.0, 736.0, 560.0}));
	SetEntityProps(iTriggerRoar);

	// attack origin.
	fOriginTemp[0] = fOrigin[0] + 360.5;
	fOriginTemp[1] = fOrigin[1] - 58.74;
	fOriginTemp[2] = fOrigin[2] + 35.6;

	// trigger_hurt attack.
	int iTriggerAttack = CreateEntityAtOrigin("trigger_hurt", fOriginTemp);
	DispatchKeyFormat(iTriggerAttack, "targetname",     "item_balrog_attack_%d", g_iCounter);
	DispatchKeyFormat(iTriggerAttack, "filtername",     "item_filter_zombie_ignored");
	DispatchKeyFormat(iTriggerAttack, "spawnflags",     "9");
	DispatchKeyFormat(iTriggerAttack, "startdisabled",  "1");
	DispatchKeyFormat(iTriggerAttack, "damagetype",     "128");
	DispatchKeyFormat(iTriggerAttack, "damagemodel",    "0");
	DispatchKeyFormat(iTriggerAttack, "damagecap",      "20");
	DispatchKeyFormat(iTriggerAttack, "damage",         "500");
	SpawnAndActivate(iTriggerAttack);
	ParentToEntity(iTriggerAttack, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerAttack, view_as<float>({-152.0, -256.0, -71.5}), view_as<float>({152.0, 256.0, 71.5}));
	SetEntityProps(iTriggerAttack);

	// ambient_generic walk.
	int iSoundWalk = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSoundWalk, "targetname",         "item_balrog_walk_sound_%d", g_iCounter);
	DispatchKeyFormat(iSoundWalk, "spawnflags",         "48");
	DispatchKeyFormat(iSoundWalk, "radius",             "8250");
	DispatchKeyFormat(iSoundWalk, "sourceentityname",   "item_balrog_knife_%d", g_iCounter);
	DispatchKeyFormat(iSoundWalk, "message",            "npc/strider/strider_step2.wav");
	DispatchKeyFormat(iSoundWalk, "volume",             "10");
	DispatchKeyFormat(iSoundWalk, "health",             "10");
	DispatchKeyFormat(iSoundWalk, "pitch",              "100");
	DispatchKeyFormat(iSoundWalk, "pitchstart",         "100");
	SpawnAndActivate(iSoundWalk);
	ParentToEntity(iSoundWalk, iKnife);

	// ambient_generic roar.
	int iSoundRoar = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSoundRoar, "targetname",         "item_balrog_roar_sound_%d", g_iCounter);
	DispatchKeyFormat(iSoundRoar, "spawnflags",         "49");
	DispatchKeyFormat(iSoundRoar, "radius",             "8250");
	DispatchKeyFormat(iSoundRoar, "sourceentityname",   "item_balrog_knife_%d", g_iCounter);
	DispatchKeyFormat(iSoundRoar, "message",            "unloze/balrog_scream.mp3");
	DispatchKeyFormat(iSoundRoar, "volume",             "10");
	DispatchKeyFormat(iSoundRoar, "health",             "10");
	DispatchKeyFormat(iSoundRoar, "pitch",              "100");
	DispatchKeyFormat(iSoundRoar, "pitchstart",         "100");
	SpawnAndActivate(iSoundRoar);
	ParentToEntity(iSoundRoar, iKnife);

	// ambient_generic attack.
	int iSoundAttack = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSoundAttack, "targetname",       "item_balrog_attack_sound_%d", g_iCounter);
	DispatchKeyFormat(iSoundAttack, "spawnflags",       "48");
	DispatchKeyFormat(iSoundAttack, "radius",           "8250");
	DispatchKeyFormat(iSoundAttack, "sourceentityname", "item_balrog_knife_%d", g_iCounter);
	DispatchKeyFormat(iSoundAttack, "message",          "npc/strider/strider_skewer1.wav");
	DispatchKeyFormat(iSoundAttack, "volume",           "10");
	DispatchKeyFormat(iSoundAttack, "health",           "10");
	DispatchKeyFormat(iSoundAttack, "pitch",            "100");
	DispatchKeyFormat(iSoundAttack, "pitchstart",       "100");
	SpawnAndActivate(iSoundAttack);
	ParentToEntity(iSoundAttack, iKnife);

	// logic_compare roar.
	int iCompareRoar = CreateEntityAtOrigin("logic_compare", fOrigin);
	DispatchKeyFormat(iCompareRoar, "targetname",       "item_balrog_roar_compare_%d", g_iCounter);
	DispatchKeyFormat(iCompareRoar, "spawnflags",       "0");
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "!self,SetValue,1,0,-1");
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "!self,SetValue,0,25,-1");
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "item_balrog_attack_compare_%d,SetCompareValue,-1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "item_balrog_attack_compare_%d,SetCompareValue,0,4,-1", g_iCounter);
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "item_spawn_speedmod,ModifySpeed,0,0,-1");
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "item_spawn_speedmod,ModifySpeed,0.8,4,-1");
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "item_balrog_model_%d,SetAnimation,balrog_groar,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "item_balrog_roar_sound_%d,PlaySound,,1,-1", g_iCounter);
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "item_balrog_roar_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareRoar, "OnEqualTo",        "item_balrog_roar_%d,Disable,,4,-1", g_iCounter);
	SpawnAndActivate(iCompareRoar);
	ParentToEntity(iCompareRoar, iKnife);

	// logic_compare attack.
	int iCompareAttack = CreateEntityAtOrigin("logic_compare", fOrigin);
	DispatchKeyFormat(iCompareAttack, "targetname",     "item_balrog_attack_compare_%d", g_iCounter);
	DispatchKeyFormat(iCompareAttack, "spawnflags",     "0");
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "!self,SetValue,1,0,-1");
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "!self,SetValue,0,4,-1");
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "item_balrog_roar_compare_%d,SetCompareValue,-1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "item_balrog_roar_compare_%d,SetCompareValue,0,4,-1", g_iCounter);
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "item_spawn_speedmod,ModifySpeed,0,0,-1");
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "item_spawn_speedmod,ModifySpeed,0.8,4,-1");
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "item_balrog_model_%d,SetAnimation,balrog_attack1,0,-1", g_iCounter);
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "item_balrog_attack_sound_%d,PlaySound,,1,-1", g_iCounter);
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "item_balrog_attack_%d,Enable,,1.75,-1", g_iCounter);
	DispatchKeyFormat(iCompareAttack, "OnEqualTo",      "item_balrog_attack_%d,Disable,,2.2,-1", g_iCounter);
	SpawnAndActivate(iCompareAttack);
	ParentToEntity(iCompareAttack, iKnife);

	// logic_relay death.
	int iRelayDeath = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayDeath, "targetname",        "item_balrog_death_%d", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "spawnflags",        "0");
	DispatchKeyFormat(iRelayDeath, "OnTrigger",         "!self,FireUser1,,0.1,-1");
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_knife_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_strip_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_walk_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_roar_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_attack_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_walk_sound_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_roar_sound_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_attack_sound_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_roar_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_attack_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "item_balrog_controls_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "!activator,AddOutput,rendermode 0,0,1");
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "!activator,AddOutput,renderfx 0,0,1");
//  DispatchKeyFormat(iRelayDeath, "OnUser1",           "!activator,AddOutput,gravity 1,0,1");
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "!activator,SetDamageFilter,,0,1");
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "!activator,SetHealth,0,0.02,-1");
	DispatchKeyFormat(iRelayDeath, "OnUser1",           "!self,Kill,,0,1");
	SpawnAndActivate(iRelayDeath);
	ParentToEntity(iRelayDeath, iKnife);

	// health origin.
	fOriginTemp[0] = fOrigin[0] + 44.0;
	fOriginTemp[1] = fOrigin[1] - 18.74;
	fOriginTemp[2] = fOrigin[2] + 126.89;

	// func_physbox_multiplayer health.
	int iHealth = CreateEntityAtOrigin("func_physbox_multiplayer", fOriginTemp);
	DispatchKeyFormat(iHealth, "targetname",            "item_balrog_health_%d", g_iCounter);
	DispatchKeyFormat(iHealth, "model",                 "models/props/cs_militia/crate_extrasmallmill.mdl");
	DispatchKeyFormat(iHealth, "spawnflags",            "8713216");
	DispatchKeyFormat(iHealth, "rendermode",            "0");
	DispatchKeyFormat(iHealth, "renderfx",              "0");
	DispatchKeyFormat(iHealth, "rendercolor",           "255 255 255");
	DispatchKeyFormat(iHealth, "renderamt",             "255 255 255");
	DispatchKeyFormat(iHealth, "propdata",              "0");
	DispatchKeyFormat(iHealth, "pressuredelay",         "0");
	DispatchKeyFormat(iHealth, "preferredcarryangles",  "0 0 0");
	DispatchKeyFormat(iHealth, "performancemode",       "1");
	DispatchKeyFormat(iHealth, "notsolid",              "0");
	DispatchKeyFormat(iHealth, "nodamageforces",        "0");
	DispatchKeyFormat(iHealth, "material",              "0");
	DispatchKeyFormat(iHealth, "massScale",             "0");
	DispatchKeyFormat(iHealth, "health",                "32500");
	DispatchKeyFormat(iHealth, "gibdir",                "0 0 0");
	DispatchKeyFormat(iHealth, "forcetoenablemotion",   "0");
	DispatchKeyFormat(iHealth, "explosion",             "0");
	DispatchKeyFormat(iHealth, "exploderadius",         "0");
	DispatchKeyFormat(iHealth, "explodemagnitude",      "0");
	DispatchKeyFormat(iHealth, "explodedamage",         "0");
	DispatchKeyFormat(iHealth, "disableshadows",        "1");
	DispatchKeyFormat(iHealth, "disablereceiveshadows", "1");
	DispatchKeyFormat(iHealth, "damagetype",            "0");
	DispatchKeyFormat(iHealth, "damagetoenablemotion",  "0");
	DispatchKeyFormat(iHealth, "damagefilter",          "item_filter_humans");
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_knife_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_strip_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_walk_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_roar_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_attack_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_walk_sound_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_roar_sound_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_attack_sound_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_roar_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_attack_compare_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_controls_%d,Kill,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_model_%d,ClearParent,,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_model_%d,SetAnimation,balrog_death,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnBreak",               "item_balrog_model_%d,SetDefaultAnimation,balrog_death_idle,0,1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser1",               "!activator,SetDamageFilter,item_filter_human_ignored,0,-1");
	DispatchKeyFormat(iHealth, "OnUser1",               "!activator,AddOutput,rendermode 10,0,-1");
	DispatchKeyFormat(iHealth, "OnUser1",               "!activator,AddOutput,renderfx 6,0,-1");
//  DispatchKeyFormat(iHealth, "OnUser1",               "!activator,AddOutput,gravity 2,0,-1");
	DispatchKeyFormat(iHealth, "OnUser1",               "!self,FireUser1,,1,-1");
	DispatchKeyFormat(iHealth, "OnUser2",               "item_balrog_walk_sound_%d,PlaySound,,0,-1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser2",               "item_balrog_walk_%d,Disable,,0,-1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser2",               "item_balrog_walk_%d,Enable,,0.1,-1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser2",               "!self,FireUser2,,0.95,-1");
	DispatchKeyFormat(iHealth, "OnUser3",               "item_balrog_death_%d,CancelPending,,0,-1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser3",               "item_balrog_death_%d,Trigger,,0.05,-1", g_iCounter);
	DispatchKeyFormat(iHealth, "OnUser3",               "!self,FireUser3,,0.1,-1");
	SpawnAndActivate(iHealth);
	ParentToEntity(iHealth, iKnife);

	// make the physbox work.
	SetEntityBBox(iHealth, view_as<float>({-124.0, -120.0, -188.2}), view_as<float>({124.0, 120.0, 188.2}));
	SetEntityProps(iHealth);

	HookSingleEntityOutput(iHealth, "OnBreak", BalrogKill, true);

	// game_ui.
	int iControls = CreateEntityAtOrigin("game_ui", fOrigin);
	DispatchKeyFormat(iControls, "targetname",          "item_balrog_controls_%d", g_iCounter);
	DispatchKeyFormat(iControls, "spawnflags",          "0");
	DispatchKeyFormat(iControls, "fieldofview",         "-1.0");
	DispatchKeyFormat(iControls, "PressedAttack",       "item_balrog_attack_compare_%d,Compare,,0,-1", g_iCounter);
	DispatchKeyFormat(iControls, "PressedAttack2",      "item_balrog_roar_compare_%d,Compare,,0,-1", g_iCounter);
	SpawnAndActivate(iControls);
	ParentToEntity(iControls, iKnife);

	AcceptEntityInput(iTriggerStrip, "Enable");
	g_iCounter++;
}

public void BalrogPickup(const char[] output, int caller, int activator, float delay) {
	if (IsValidClient(activator)) {
		ServerCommand("say ** %N has picked up Balrog **", activator);
		PrintToChat(activator, "RIGHT CLICK = MOTIVATE ZOMBIES and LEFT CLICK = ATTACK.");
		g_bClientHasItem[activator] = true;
	}
}

//----------------------------------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------------------------------
public void BalrogKill(const char[] output, int caller, int activator, float delay) {
	if (IsValidClient(activator)) {
		ServerCommand("say ** %N has killed the Balrog **", activator);
	}
}