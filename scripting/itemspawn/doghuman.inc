public Action Command_HumanDog(int client, int argc)
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
				SpawnHumanDog(fOrigin);
				LogAction(client, -1, "\"%L\" spawned Human Dog at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
					SpawnHumanDog(fOrigin);
					LogAction(client, -1, "\"%L\" spawned Human Dog at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
						SpawnHumanDog(fOrigin);
						LogAction(client, -1, "\"%L\" gave Human Dog to \"%L\".", client, iTargets[i]);
					}
				}
			}
		}
	}
	return Plugin_Handled;
}

public void SpawnHumanDog(float fOrigin[3])
{
	float fOriginTemp[3];

	// weapon_knife.
	int iKnife = CreateEntityAtOrigin("weapon_knife", fOrigin);
	DispatchKeyFormat(iKnife, "targetname", "human_dog_knife_%d", g_iCounter);
	DispatchKeyFormat(iKnife, "hammerid", "11051995%d", g_iCounter);
	DispatchKeyFormat(iKnife, "spawnflags", "1");
	DispatchKeyFormat(iKnife, "angles", "0 0 0");
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "!self,AddOutput,renderfx 6,0,1");
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "item_human_dog_controls_%d,Activate,,0,1", g_iCounter);
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "!activator,AddOutput,health 50000,0,1");
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "item_spawn_speedmod,ModifySpeed,0.8,0,1");
	SpawnAndActivate(iKnife);

	HookSingleEntityOutput(iKnife, "OnPlayerPickup", HumanDogPickup, true);

	// model origin.
	fOriginTemp[0] = fOrigin[0] + 10.4;
	fOriginTemp[1] = fOrigin[1] - 2.0;
	fOriginTemp[2] = fOrigin[2] - 40.32;

	// prop_dynamic model.
	int iModel = CreateEntityAtOrigin("prop_dynamic", fOriginTemp);
	DispatchKeyFormat(iModel, "targetname", "item_human_dog_model_%d", g_iCounter);
	DispatchKeyFormat(iModel, "model", "models/player/pil/re1/dog/dog_pil.mdl");
	DispatchKeyFormat(iModel, "disablebonefollowers", "1");
	DispatchKeyFormat(iModel, "defaultanim", "Run_lower");
	DispatchKeyFormat(iModel, "angles", "0 0 0");
	DispatchKeyFormat(iModel, "solid", "0");
	DispatchKeyFormat(iModel, "skin", "1");
	SpawnAndActivate(iModel);
	ParentToEntity(iModel, iKnife);

	// trigger_once strip.
	int iTriggerStrip = CreateEntityAtOrigin("trigger_once", fOrigin);
	DispatchKeyFormat(iTriggerStrip, "targetname", "item_human_dog_strip_%d", g_iCounter);
	DispatchKeyFormat(iTriggerStrip, "filtername", "item_filter_human");
	DispatchKeyFormat(iTriggerStrip, "spawnflags", "1");
	DispatchKeyFormat(iTriggerStrip, "startdisabled", "1");
	DispatchKeyFormat(iTriggerStrip, "OnStartTouch", "item_spawn_weaponstrip,StripWeaponsAndSuit,,0,1");
	SpawnAndActivate(iTriggerStrip);
	ParentToEntity(iTriggerStrip, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerStrip, view_as<float>({-8.0, -8.0, -8.0}), view_as<float>({8.0, 8.0, 8.0}));
	SetEntityProps(iTriggerStrip);

	// Relay Trigger
	int iRelayTrigger = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayTrigger, "targetname", "human_dog_relay_trigger_%d", g_iCounter);
	DispatchKeyFormat(iRelayTrigger, "spawnflags", "0");
	DispatchKeyFormat(iRelayTrigger, "OnTrigger", "human_dog_relay_trigger2_%d,Trigger,,0,4", g_iCounter);
	SpawnAndActivate(iRelayTrigger);
	ParentToEntity(iRelayTrigger, iKnife);

	// Relay Trigger2
	int iRelayTrigger2 = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayTrigger2, "targetname", "human_dog_relay_trigger2_%d", g_iCounter);
	DispatchKeyFormat(iRelayTrigger2, "spawnflags", "0");
	SpawnAndActivate(iRelayTrigger2);
	ParentToEntity(iRelayTrigger2, iKnife);

	HookSingleEntityOutput(iRelayTrigger2, "OnTrigger", HumanDogUse, false);

	// game_ui.
	int iControls = CreateEntityAtOrigin("game_ui", fOrigin);
	DispatchKeyFormat(iControls, "targetname", "item_human_dog_controls_%d", g_iCounter);
	DispatchKeyFormat(iControls, "spawnflags", "0");
	DispatchKeyFormat(iControls, "fieldofview", "-1.0");
	DispatchKeyFormat(iControls, "PressedAttack2", "human_dog_relay_trigger_%d,Trigger,,0,-1", g_iCounter);
	SpawnAndActivate(iControls);
	ParentToEntity(iControls, iKnife);

	// enable pickup trigger
	AcceptEntityInput(iTriggerStrip, "Enable");

	g_iCounter++;
}

public void HumanDogPickup(const char[] output, int caller, int activator, float delay)
{
	ServerCommand("say ** %N has picked up Human Dog **", activator);
	PrintToChat(activator, " RIGHT CLICK with your KNIFE to leave a present for the Zombies (max 4)");
	CreateTimer(2.0, EquipWeapons, GetClientUserId(activator), TIMER_FLAG_NO_MAPCHANGE);
}

public void HumanDogUse(const char[] output, int caller, int activator, float delay)
{
	float fOrigin[3];
	float fAngles[3];
	GetClientEyeAngles(activator, fAngles);
	GetClientEyePosition(activator, fOrigin);
	fAngles[0] = 0.0;
	fAngles[2] = 0.0;
	fOrigin[0] = fOrigin[0] + Cosine(DegToRad(fAngles[1])) * -50.0;
	fOrigin[1] = fOrigin[1] + Sine(DegToRad(fAngles[1])) * -50.0;
	fOrigin[2] = fOrigin[2] - 62.00;

	// prop_dynamic model.
	int iModel = CreateEntityAtOrigin("prop_dynamic", fOrigin);
	DispatchKeyFormat(iModel, "targetname", "human_dog_poo_%d", g_iCounter);
	DispatchKeyFormat(iModel, "model", "models/poo/curlygpoo.mdl");
	DispatchKeyFormat(iModel, "spawnflags", "0");
	DispatchKeyFormat(iModel, "PerformanceMode", "0");
	DispatchKeyFormat(iModel, "solid", "0");
	SpawnAndActivate(iModel);

	// trigger the explosion.
	int iTriggerExplosion = CreateEntityAtOrigin("trigger_once", fOrigin);
	DispatchKeyFormat(iTriggerExplosion, "targetname", "human_dog_poo_trigger_%d", g_iCounter);
	DispatchKeyFormat(iTriggerExplosion, "filtername", "item_filter_zombie");
	DispatchKeyFormat(iTriggerExplosion, "spawnflags", "1");
	DispatchKeyFormat(iTriggerExplosion, "startdisabled", "0");
	DispatchKeyFormat(iTriggerExplosion, "OnStartTouch", "human_dog_poo_%d,Kill,,1,1", g_iCounter);
	DispatchKeyFormat(iTriggerExplosion, "OnStartTouch", "human_dog_poo_hurt_%d,Enable,,0,1", g_iCounter);
	DispatchKeyFormat(iTriggerExplosion, "OnStartTouch", "human_dog_poo_hurt_%d,Disable,,0.25,1", g_iCounter);
	DispatchKeyFormat(iTriggerExplosion, "OnStartTouch", "human_dog_particles_%d,Start,,0,-1", g_iCounter);
	DispatchKeyFormat(iTriggerExplosion, "OnStartTouch", "human_dog_particles_%d,Kill,,1,-1", g_iCounter);
	DispatchKeyFormat(iTriggerExplosion, "OnStartTouch", "human_dog_sound_%d,PlaySound,,0,-1", g_iCounter);
	SpawnAndActivate(iTriggerExplosion);
	ParentToEntity(iTriggerExplosion, iModel);

	// make the trigger work.
	SetEntityBBox(iTriggerExplosion, view_as<float>({-12.0, -12.0, -7.5}), view_as<float>({12.0, 12.0, 7.5}));
	SetEntityProps(iTriggerExplosion);

	// trigger poo damage.
	int iTriggerExplosionDamage = CreateEntityAtOrigin("trigger_hurt", fOrigin);
	DispatchKeyFormat(iTriggerExplosionDamage, "targetname", "human_dog_poo_hurt_%d", g_iCounter);
	DispatchKeyFormat(iTriggerExplosionDamage, "filtername", "item_filter_zombie");
	DispatchKeyFormat(iTriggerExplosionDamage, "spawnflags", "1");
	DispatchKeyFormat(iTriggerExplosionDamage, "startdisabled", "1");
	DispatchKeyFormat(iTriggerExplosionDamage, "damagetype", "0");
	DispatchKeyFormat(iTriggerExplosionDamage, "damagemodel", "0");
	DispatchKeyFormat(iTriggerExplosionDamage, "damagecap", "20");
	DispatchKeyFormat(iTriggerExplosionDamage, "damage", "8000");
	SpawnAndActivate(iTriggerExplosionDamage);
	ParentToEntity(iTriggerExplosionDamage, iModel);

	// make the trigger work.
	SetEntityBBox(iTriggerExplosionDamage, view_as<float>({-96.0, -112.0, -142.0}), view_as<float>({96.0, 112.0, 142.0}));
	SetEntityProps(iTriggerExplosionDamage);

	// particles.
	int iParticles = CreateEntityAtOrigin("info_particle_system", fOrigin);
	DispatchKeyFormat(iParticles, "targetname", "human_dog_particles_%d", g_iCounter);
	DispatchKeyFormat(iParticles, "effect_name", "fire_small_01");
	DispatchKeyFormat(iParticles, "start_active", "0");
	SpawnAndActivate(iParticles);
	ParentToEntity(iParticles, iModel);

	// sound.
	int iSound = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSound, "targetname", "human_dog_sound_%d", g_iCounter);
	DispatchKeyFormat(iSound, "spawnflags", "49");
	DispatchKeyFormat(iSound, "radius", "1250");
	DispatchKeyFormat(iSound, "message", "ambient/explosions/explode_9.wav");
	DispatchKeyFormat(iSound, "volume", "10");
	DispatchKeyFormat(iSound, "health", "10");
	DispatchKeyFormat(iSound, "pitch", "100");
	DispatchKeyFormat(iSound, "pitchstart", "100");
	SpawnAndActivate(iSound);
	ParentToEntity(iSound, iModel);

	g_iCounter ++;
}