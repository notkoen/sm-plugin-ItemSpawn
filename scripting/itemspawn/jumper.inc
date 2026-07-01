public Action Command_Jumper(int client, int argc)
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
				SpawnJumper(fOrigin);
				LogAction(client, -1, "\"%L\" spawned Jumper at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
					SpawnJumper(fOrigin);
					LogAction(client, -1, "\"%L\" spawned Jumper at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
					if (IsClientInGame(iTargets[i]) && IsPlayerAlive(iTargets[i]))
					{
						GetClientEyePosition(iTargets[i], fOrigin);
						SpawnJumper(fOrigin);
						LogAction(client, -1, "\"%L\" gave Jumper to \"%L\".", client, iTargets[i]);
					}
				}
			}
		}
	}
	return Plugin_Handled;
}

public void SpawnJumper(float fOrigin[3])
{
	float fOriginTemp[3];

	// weapon_knife.
	int iKnife = CreateEntityAtOrigin("weapon_knife", fOrigin);
	DispatchKeyFormat(iKnife, "targetname", "item_jumper_knife_%d", g_iCounter);
	DispatchKeyFormat(iKnife, "hammerid", "11051995%d", g_iCounter);
	DispatchKeyFormat(iKnife, "spawnflags", "1");
	DispatchKeyFormat(iKnife, "angles", "0 0 0");
	DispatchKeyFormat(iKnife, "OnPlayerPickup", "item_jumper_controls_%d,Activate,,0,1", g_iCounter);

	SpawnAndActivate(iKnife);

	HookSingleEntityOutput(iKnife, "OnPlayerPickup", JumperPickup, true);

	// light origin.
	fOriginTemp[0] = fOrigin[0] + 0.0;
	fOriginTemp[1] = fOrigin[1] + 0.0;
	fOriginTemp[2] = fOrigin[2] + 6.0;

	// light.
	int iLight = CreateEntityAtOrigin("light_dynamic", fOriginTemp);
	DispatchKeyFormat(iLight, "targetname", "item_jumper_light_%d", g_iCounter);
	DispatchKeyFormat(iLight, "style", "0");
	DispatchKeyFormat(iLight, "spotlight_radius", "180");
	DispatchKeyFormat(iLight, "spawnflags", "1");
	DispatchKeyFormat(iLight, "pitch", "-90");
	DispatchKeyFormat(iLight, "distance", "130");
	DispatchKeyFormat(iLight, "brightness", "10");
	DispatchKeyFormat(iLight, "angles", "0 0 0");
	DispatchKeyFormat(iLight, "_zero_percent_distance", "50");
	DispatchKeyFormat(iLight, "_quadratic_attn", "115");
	DispatchKeyFormat(iLight, "_linear_attn", "70");
	DispatchKeyFormat(iLight, "_lightHDR", "-1 -1 -1 1");
	DispatchKeyFormat(iLight, "_light", "255 128 0 1");
	DispatchKeyFormat(iLight, "_inner_cone", "0");
	DispatchKeyFormat(iLight, "_fifty_percent_distance", "42");
	DispatchKeyFormat(iLight, "_constant_attn", "50");
	DispatchKeyFormat(iLight, "_cone", "0");
	SpawnAndActivate(iLight);
	ParentToEntity(iLight, iKnife);

	// trigger_once strip.
	int iTriggerStrip = CreateEntityAtOrigin("trigger_once", fOrigin);
	DispatchKeyFormat(iTriggerStrip, "targetname", "item_jumper_strip_%d", g_iCounter);
	DispatchKeyFormat(iTriggerStrip, "spawnflags", "1");
	DispatchKeyFormat(iTriggerStrip, "startdisabled", "1");
	DispatchKeyFormat(iTriggerStrip, "OnStartTouch", "item_spawn_weaponstrip,StripWeaponsAndSuit,,0,1");
	SpawnAndActivate(iTriggerStrip);
	ParentToEntity(iTriggerStrip, iKnife);

	// make the trigger work.
	SetEntityBBox(iTriggerStrip, view_as<float>({-8.0, -8.0, -8.0}), view_as<float>({8.0, 8.0, 8.0}));
	SetEntityProps(iTriggerStrip);

	// logic_relay jump.
	int iRelayJump = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayJump, "targetname", "item_jumper_relay_%d", g_iCounter);
	DispatchKeyFormat(iRelayJump, "spawnflags", "0");
	DispatchKeyFormat(iRelayJump, "OnTrigger", "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayJump, "OnTrigger", "!self,Enable,,5,-1");
	SpawnAndActivate(iRelayJump);
	ParentToEntity(iRelayJump, iKnife);

	HookSingleEntityOutput(iRelayJump, "OnTrigger", JumperUse, false);

	// game_ui.
	int iControls = CreateEntityAtOrigin("game_ui", fOrigin);
	DispatchKeyFormat(iControls, "targetname", "item_jumper_controls_%d", g_iCounter);
	DispatchKeyFormat(iControls, "spawnflags", "0");
	DispatchKeyFormat(iControls, "fieldofview", "-1.0");
	DispatchKeyFormat(iControls, "PressedAttack2", "item_jumper_relay_%d,Trigger,,0,-1", g_iCounter);
	SpawnAndActivate(iControls);
	ParentToEntity(iControls, iKnife);

	AcceptEntityInput(iTriggerStrip, "Enable");
	g_iCounter++;
}

public void JumperPickup(const char[] output, int caller, int activator, float delay)
{
	ServerCommand("say ** %N has picked up Jumper **", activator);
	PrintToChat(activator, "RIGHT CLICK = Jump Boost");
	if (ZR_IsClientHuman(activator))
		CreateTimer(2.0, EquipWeapons, GetClientUserId(activator), TIMER_FLAG_NO_MAPCHANGE);
}

public void JumperUse(const char[] output, int caller, int activator, float delay)
{
	float fPushVector[3];
	fPushVector[0] = 0.0;
	fPushVector[1] = 0.0;
	fPushVector[2] = 500.0;

	float fCurrentVector[3];
	GetEntPropVector(activator, Prop_Data, "m_vecVelocity", fCurrentVector);

	fPushVector[0] += fCurrentVector[0];
	fPushVector[1] += fCurrentVector[1];
	fPushVector[2] += fCurrentVector[2];

	TeleportEntity(activator, NULL_VECTOR, NULL_VECTOR, fPushVector);
}