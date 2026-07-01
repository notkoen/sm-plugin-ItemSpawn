public Action Command_Vortigaunt(int client, int argc)
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
				SpawnVortigaunt(fOrigin);
				LogAction(client, -1, "\"%L\" spawned Vortigaunt at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
					SpawnVortigaunt(fOrigin);
					LogAction(client, -1, "\"%L\" spawned Vortigaunt at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
						SpawnVortigaunt(fOrigin);
						LogAction(client, -1, "\"%L\" gave Vortigaunt to \"%L\".", client, iTargets[i]);
					}
				}
			}
		}
	}
	return Plugin_Handled;
}

public void SpawnVortigaunt(float fOrigin[3])
{
	char sBuffer[256];
	float fOriginTemp[3];

	// Knife
	int iKnife = CreateEntityByName("weapon_knife");
	Format(sBuffer, sizeof(sBuffer), "vort_knife_%d", g_iCounter);
	DispatchKeyValue(iKnife, "targetname", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "11051995%d", g_iCounter);
	DispatchKeyValue(iKnife, "hammerid", sBuffer);
	DispatchKeyValue(iKnife, "spawnflags", "1");
	DispatchKeyValue(iKnife, "angles", "0 0 0");
	DispatchKeyValueVector(iKnife, "origin", fOrigin);
	DispatchKeyValue(iKnife, "OnPlayerPickUp", "!activator,AddOutput,renderfx 6,0,-1");
	Format(sBuffer, sizeof(sBuffer), "vort_knife_%d,AddOutput,renderfx 6,0.03,-1", g_iCounter);
	DispatchKeyValue(iKnife, "OnPlayerPickup", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "vort_ui_%d,Activate,,0,-1", g_iCounter);
	DispatchKeyValue(iKnife, "OnPlayerPickup", sBuffer);
	DispatchSpawn(iKnife);
	ActivateEntity(iKnife);
	HookSingleEntityOutput(iKnife, "OnPlayerPickup", VortigauntPickup, true);

	// Model
	int iModel = CreateEntityByName("prop_dynamic");
	Format(sBuffer, sizeof(sBuffer), "vort_model_%d", g_iCounter);
	DispatchKeyValue(iModel, "targetname", sBuffer);
	DispatchKeyValue(iModel, "model", "models/vortigaunt_slave.mdl");
	DispatchKeyValue(iModel, "DisableBoneFollowers", "1");
	DispatchKeyValue(iModel, "DefaultAnim", "Run_all");
	fOriginTemp[0] = fOrigin[0] + 7.0;
	fOriginTemp[1] = fOrigin[1] - 0.0;
	fOriginTemp[2] = fOrigin[2] - 34.24;
	DispatchKeyValueVector(iModel, "origin", fOriginTemp);
	DispatchSpawn(iModel);
	ActivateEntity(iModel);
	SetVariantString("!activator");
	AcceptEntityInput(iModel, "SetParent", iKnife);

	// pickup trigger_once
	int iTrigger = CreateEntityByName("trigger_once");
	Format(sBuffer, sizeof(sBuffer), "vort_trigger_%d", g_iCounter);
	DispatchKeyValue(iTrigger, "targetname", sBuffer);
	DispatchKeyValue(iTrigger, "spawnflags", "1");
	DispatchKeyValue(iTrigger, "startdisabled", "1");
	DispatchKeyValue(iTrigger, "filtername", "item_spawn_zombie_filter");
	DispatchKeyValue(iTrigger, "OnStartTouch", "item_spawn_weaponstrip,StripWeaponsAndSuit,,0,1");
	DispatchKeyValueVector(iTrigger, "origin", fOrigin);
	DispatchSpawn(iTrigger);
	ActivateEntity(iTrigger);
	SetEntityModel(iTrigger, "models/vortigaunt_slave.mdl");
	float fMinbounds[3] = {-10.0, -10.0, -36.0};
	float fMaxbounds[3] = {10.0, 10.0, 36.0};
	SetEntPropVector(iTrigger, Prop_Send, "m_vecMins", fMinbounds);
	SetEntPropVector(iTrigger, Prop_Send, "m_vecMaxs", fMaxbounds);
	SetEntProp(iTrigger, Prop_Send, "m_nSolidType", 2);
	int enteffects = GetEntProp(iTrigger, Prop_Send, "m_fEffects");
	enteffects |= 32;
	SetEntProp(iTrigger, Prop_Send, "m_fEffects", enteffects);
	SetVariantString("!activator");
	AcceptEntityInput(iTrigger, "SetParent", iKnife);

	// Sound
	int iSound = CreateEntityByName("ambient_generic");
	Format(sBuffer, sizeof(sBuffer), "vort_sound_%d", g_iCounter);
	DispatchKeyValue(iSound, "targetname", sBuffer);
	DispatchKeyValue(iSound, "spawnflags", "48");
	DispatchKeyValue(iSound, "radius", "2250");
	Format(sBuffer, sizeof(sBuffer), "vort_knife_%d", g_iCounter);
	DispatchKeyValue(iSound, "SourceEntityName", sBuffer);
	DispatchKeyValue(iSound, "message", "ambient/energy/zap9.wav");
	DispatchKeyValue(iSound, "volume", "10");
	DispatchKeyValue(iSound, "health", "10");
	DispatchKeyValue(iSound, "pitch", "100");
	DispatchKeyValue(iSound, "pitchstart", "100");
	DispatchSpawn(iSound);
	ActivateEntity(iSound);
	SetVariantString("!activator");
	AcceptEntityInput(iSound, "SetParent", iKnife);

	// Push
	int iPush = CreateEntityByName("trigger_push");
	Format(sBuffer, sizeof(sBuffer), "vort_push_%d", g_iCounter);
	DispatchKeyValue(iPush, "targetname", sBuffer);
	DispatchKeyValue(iPush, "spawnflags", "1");
	DispatchKeyValue(iPush, "startdisabled", "1");
	DispatchKeyValue(iPush, "speed", "1800");
	DispatchKeyValue(iPush, "pushdir", "0 180 0");
	DispatchKeyValue(iPush, "filtername", "item_spawn_human_filter");
	fOriginTemp[0] = fOrigin[0] + 899.0;
	fOriginTemp[1] = fOrigin[1] - 0.0;
	fOriginTemp[2] = fOrigin[2] - 8.0;
	DispatchKeyValueVector(iPush, "origin", fOriginTemp);
	DispatchSpawn(iPush);
	ActivateEntity(iPush);
	SetEntityModel(iPush, "models/vortigaunt_slave.mdl");
	float fMinbounds2[3] = {-852.0, -16.0, -9.0};
	float fMaxbounds2[3] = {852.0, 16.0, 9.0};
	SetEntPropVector(iPush, Prop_Send, "m_vecMins", fMinbounds2);
	SetEntPropVector(iPush, Prop_Send, "m_vecMaxs", fMaxbounds2);
	SetEntProp(iPush, Prop_Send, "m_nSolidType", 3);
	int enteffects2 = GetEntProp(iPush, Prop_Send, "m_fEffects");
	enteffects2 |= 32;
	SetEntProp(iPush, Prop_Send, "m_fEffects", enteffects2);
	SetVariantString("!activator");
	AcceptEntityInput(iPush, "SetParent", iKnife);

	// Relay
	int iRelay = CreateEntityByName("logic_relay");
	Format(sBuffer, sizeof(sBuffer), "vort_relay_%d", g_iCounter);
	DispatchKeyValueVector(iRelay, "origin", fOrigin);
	DispatchKeyValue(iRelay, "targetname", sBuffer);
	DispatchKeyValue(iRelay, "spawnflags", "0");
	DispatchKeyValue(iRelay, "startdisabled", "0");
	DispatchKeyValue(iRelay, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyValue(iRelay, "OnTrigger", "!self,Enable,,4,-1");
	Format(sBuffer, sizeof(sBuffer), "vort_model_%d,SetAnimation,zapattack1,0,-1", g_iCounter);
	DispatchKeyValue(iRelay, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "vort_push_%d,Enable,,1.5,-1", g_iCounter);
	DispatchKeyValue(iRelay, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "vort_push_%d,Disable,,2,-1", g_iCounter);
	DispatchKeyValue(iRelay, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "vort_sound_%d,PlaySound,,1.5,-1", g_iCounter);
	DispatchKeyValue(iRelay, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "vort_beam_%d,TurnOn,,1.5,-1", g_iCounter);
	DispatchKeyValue(iRelay, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "vort_beam_%d,TurnOff,,2.5,-1", g_iCounter);
	DispatchKeyValue(iRelay, "OnTrigger", sBuffer);
	DispatchKeyValue(iRelay, "OnTrigger", "item_spawn_speedmod,ModifySpeed,0,0,-1");
	DispatchKeyValue(iRelay, "OnTrigger", "item_spawn_speedmod,ModifySpeed,1.15,3.25,-1");
	DispatchSpawn(iRelay);
	ActivateEntity(iRelay);
	SetVariantString("!activator");
	AcceptEntityInput(iRelay, "SetParent", iKnife);

	// Game UI
	int iUI = CreateEntityByName("game_ui");
	Format(sBuffer, sizeof(sBuffer), "vort_ui_%d", g_iCounter);
	DispatchKeyValue(iUI, "targetname", sBuffer);
	DispatchKeyValue(iUI, "spawnflags", "0");
	DispatchKeyValue(iUI, "FieldOfView", "-1.0");
	Format(sBuffer, sizeof(sBuffer), "vort_relay_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyValue(iUI, "PressedAttack2", sBuffer);
	DispatchSpawn(iUI);
	ActivateEntity(iUI);
	SetVariantString("!activator");
	AcceptEntityInput(iUI, "SetParent", iKnife);

	// beam start
	int iBeamStart = CreateEntityByName("prop_dynamic_override");
	Format(sBuffer, sizeof(sBuffer), "vort_beam_start_%d", g_iCounter);
	DispatchKeyValue(iBeamStart, "targetname", sBuffer);
	DispatchKeyValue(iBeamStart, "model", "models/props/cs_italy/bananna.mdl");
	DispatchKeyValue(iBeamStart, "disableshadows", "1");
	DispatchKeyValue(iBeamStart, "disablereceiveshadows", "1");
	DispatchKeyValue(iBeamStart, "DisableBoneFollowers", "1");
	DispatchKeyValue(iBeamStart, "rendermode", "10");
	fOriginTemp[0] = fOrigin[0] + 29.0;
	fOriginTemp[1] = fOrigin[1] - 0.0;
	fOriginTemp[2] = fOrigin[2] - 8.0;
	DispatchKeyValueVector(iBeamStart, "origin", fOriginTemp);
	DispatchSpawn(iBeamStart);
	ActivateEntity(iBeamStart);
	SetVariantString("!activator");
	AcceptEntityInput(iBeamStart, "SetParent", iKnife);

	// beam end
	int iBeamEnd = CreateEntityByName("prop_dynamic_override");
	Format(sBuffer, sizeof(sBuffer), "vort_beam_end_%d", g_iCounter);
	DispatchKeyValue(iBeamEnd, "targetname", sBuffer);
	DispatchKeyValue(iBeamEnd, "model", "models/props/cs_italy/bananna.mdl");
	DispatchKeyValue(iBeamEnd, "disableshadows", "1");
	DispatchKeyValue(iBeamEnd, "disablereceiveshadows", "1");
	DispatchKeyValue(iBeamEnd, "DisableBoneFollowers", "1");
	DispatchKeyValue(iBeamEnd, "rendermode", "10");
	fOriginTemp[0] = fOrigin[0] + 1743.0;
	fOriginTemp[1] = fOrigin[1] - 0.0;
	fOriginTemp[2] = fOrigin[2] - 8.0;
	DispatchKeyValueVector(iBeamEnd, "origin", fOriginTemp);
	DispatchSpawn(iBeamEnd);
	ActivateEntity(iBeamEnd);
	SetVariantString("!activator");
	AcceptEntityInput(iBeamEnd, "SetParent", iKnife);

	// Beam
	int iBeam = CreateEntityByName("env_beam");
	Format(sBuffer, sizeof(sBuffer), "vort_beam_%d", g_iCounter);
	DispatchKeyValue(iBeam, "targetname", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "vort_beam_start_%d", g_iCounter);
	DispatchKeyValue(iBeam, "LightningStart", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "vort_beam_end_%d", g_iCounter);
	DispatchKeyValue(iBeam, "LightningEnd", sBuffer);
	DispatchKeyValue(iBeam, "BoltWidth", "3");
	DispatchKeyValue(iBeam, "NoiseAmplitude", "1");
	DispatchKeyValue(iBeam, "decalname", "Bigshot");
	DispatchKeyValue(iBeam, "framerate", "0");
	DispatchKeyValue(iBeam, "framestart", "0");
	DispatchKeyValue(iBeam, "life", "1");
	DispatchKeyValue(iBeam, "spawnflags", "48");
	DispatchKeyValue(iBeam, "TouchType", "1");
	DispatchKeyValue(iBeam, "rendercolor", "0 200 0");
	DispatchKeyValue(iBeam, "texture", "sprites/laserbeam.spr");
	DispatchSpawn(iBeam);
	SetVariantString("!activator");
	AcceptEntityInput(iBeam, "SetParent", iKnife);

	// enable pickup trigger
	SetVariantString("!activator");
	AcceptEntityInput(iTrigger, "Enable");

	g_iCounter ++;
}

public void VortigauntPickup(const char[] output, int caller, int activator, float delay)
{
	char sBuffer[128];
	Format(sBuffer, sizeof(sBuffer), "say ** %N has picked up Vortigaunt **", activator);
	SetVariantString(sBuffer);
	//AcceptEntityInput(g_iConsole, "Command");
	PrintToChat(activator, "Right Click to Pull Humans in front of you.");
}