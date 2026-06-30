public Action Command_TNT(int client, int argc)
{
	float fOrigin[3];

	if (argc < 1) {
		GetClientEyePosition(client, fOrigin);
		SpawnTNT(fOrigin);
		LogAction(client, -1, "\"%L\" spawned TNT at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
		return Plugin_Handled;
	}

	char sArgs[64];
	char sTargetName[MAX_TARGET_LENGTH];
	int iTargets[MAXPLAYERS];
	int iTargetCount;
	bool bIsML;

	GetCmdArg(1, sArgs, sizeof(sArgs));

	if ((iTargetCount = ProcessTargetString(sArgs, client, iTargets, MAXPLAYERS, COMMAND_FILTER_CONNECTED, sTargetName, sizeof(sTargetName), bIsML)) <= 0) {
		ReplyToTargetError(client, iTargetCount);
		return Plugin_Handled;
	}

	for (int i = 0; i < iTargetCount; i++) {
		if (IsClientInGame(iTargets[i]) && IsPlayerAlive(iTargets[i]) && (ZR_IsClientZombie(iTargets[i]))) {
			GetClientEyePosition(iTargets[i], fOrigin);
			SpawnTNT(fOrigin);
			LogAction(client, -1, "\"%L\" gave TNT to \"%L\".", client, iTargets[i]);
		}
	}
	return Plugin_Handled;
}

public void SpawnTNT(float fOrigin[3]) {
	char sBuffer[256];
	float fOriginTemp[3];

	// Knife
	int iKnife = CreateEntityByName("weapon_knife");
	Format(sBuffer, sizeof(sBuffer), "tnt_knife_%d", g_iCounter);
	DispatchKeyValue(iKnife, "targetname", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "11051995%d", g_iCounter);
	DispatchKeyValue(iKnife, "hammerid", sBuffer);
	DispatchKeyValue(iKnife, "spawnflags", "1");
	DispatchKeyValue(iKnife, "angles", "0 0 0");
	DispatchKeyValueVector(iKnife, "origin", fOrigin);
	Format(sBuffer, sizeof(sBuffer), "tnt_ui_%d,Activate,,0,-1", g_iCounter);
	DispatchKeyValue(iKnife, "OnPlayerPickup", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_holder_sparks_%d,StartSpark,,0,1", g_iCounter);
	DispatchKeyValue(iKnife, "OnPlayerPickup", sBuffer);
	DispatchSpawn(iKnife);
	ActivateEntity(iKnife);
	HookSingleEntityOutput(iKnife, "OnPlayerPickup", TNTPickup, true);

	// Model
	int iModel = CreateEntityByName("prop_dynamic");
	Format(sBuffer, sizeof(sBuffer), "tnt_model_%d", g_iCounter);
	DispatchKeyValue(iModel, "targetname", sBuffer);
	DispatchKeyValue(iModel, "model", "models/props/furnitures/humans/barrel01b.mdl");
	DispatchKeyValue(iModel, "DisableBoneFollowers", "1");
	DispatchKeyValue(iModel, "angles", "0.432793 271.953 -12.4926");
	DispatchKeyValue(iModel, "solid", "0");
	DispatchKeyValue(iModel, "OnUser1", "!self,IgniteLifetime,0,0,-1");
	DispatchKeyValue(iModel, "OnUser1", "!self,FireUser1,,0.2,-1");
	fOriginTemp[0] = fOrigin[0] - 30.16;
	fOriginTemp[1] = fOrigin[1] - 2.0;
	fOriginTemp[2] = fOrigin[2] - 9.2;
	DispatchKeyValueVector(iModel, "origin", fOriginTemp);
	DispatchSpawn(iModel);
	ActivateEntity(iModel);
	SetVariantString("!activator");
	AcceptEntityInput(iModel, "SetParent", iKnife);
	SetVariantString("9999999");
	AcceptEntityInput(iModel, "SetHealth");
	AcceptEntityInput(iModel, "FireUser1");

	// pickup trigger_once
	int iTrigger = CreateEntityByName("trigger_once");
	Format(sBuffer, sizeof(sBuffer), "tnt_trigger_%d", g_iCounter);
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

	// Sparks
	int iSparksHolder = CreateEntityByName("env_spark");
	Format(sBuffer, sizeof(sBuffer), "tnt_holder_sparks_%d", g_iCounter);
	DispatchKeyValue(iSparksHolder, "targetname", sBuffer);
	DispatchKeyValue(iSparksHolder, "TrailLength", "1");
	DispatchKeyValue(iSparksHolder, "spawnflags", "0");
	DispatchKeyValue(iSparksHolder, "angles", "0 0 0");
	DispatchKeyValue(iSparksHolder, "MaxDelay", "0");
	DispatchKeyValue(iSparksHolder, "Magnitude", "1");
	fOriginTemp[0] = fOrigin[0] - 22.0;
	fOriginTemp[1] = fOrigin[1] - 0.0;
	fOriginTemp[2] = fOrigin[2] + 36.0;
	DispatchKeyValueVector(iSparksHolder, "origin", fOriginTemp);
	DispatchSpawn(iSparksHolder);
	ActivateEntity(iSparksHolder);
	SetVariantString("!activator");
	AcceptEntityInput(iSparksHolder, "SetParent", iKnife);

	// Sprite
	int iSprite = CreateEntityByName("env_sprite");
	Format(sBuffer, sizeof(sBuffer), "tnt_sprite_%d", g_iCounter);
	DispatchKeyValue(iSprite, "targetname", sBuffer);
	DispatchKeyValue(iSprite, "spawnflags", "0");
	DispatchKeyValue(iSprite, "spawnflags", "0");
	DispatchKeyValue(iSprite, "scale", "0.1");
	DispatchKeyValue(iSprite, "rendermode", "5");
	DispatchKeyValue(iSprite, "renderfx", "0");
	DispatchKeyValue(iSprite, "model", "sprites/640_train.vmt");
	fOriginTemp[0] = fOrigin[0] + 31.0;
	fOriginTemp[1] = fOrigin[1] + 1.73;
	fOriginTemp[2] = fOrigin[2] + 4.3;
	DispatchKeyValueVector(iSprite, "origin", fOriginTemp);
	DispatchSpawn(iSprite);
	ActivateEntity(iSprite);
	SetVariantString("!activator");
	AcceptEntityInput(iSprite, "SetParent", iKnife);

	// Push
	int iPush = CreateEntityByName("trigger_push");
	Format(sBuffer, sizeof(sBuffer), "tnt_push_%d", g_iCounter);
	DispatchKeyValue(iPush, "targetname", sBuffer);
	DispatchKeyValue(iPush, "spawnflags", "8");
	DispatchKeyValue(iPush, "startdisabled", "1");
	DispatchKeyValue(iPush, "speed", "18000");
	DispatchKeyValue(iPush, "pushdir", "-60 0 0");
	fOriginTemp[0] = fOrigin[0] + 79.46;
	fOriginTemp[1] = fOrigin[1] - 0.0;
	fOriginTemp[2] = fOrigin[2] + 15.76;
	DispatchKeyValueVector(iPush, "origin", fOriginTemp);
	DispatchSpawn(iPush);
	ActivateEntity(iPush);
	SetEntityModel(iPush, "models/vortigaunt_slave.mdl");
	float fMinbounds2[3] = {-36.0, -12.0, -25.0};
	float fMaxbounds2[3] = {36.0, 12.0, 25.0};
	SetEntPropVector(iPush, Prop_Send, "m_vecMins", fMinbounds2);
	SetEntPropVector(iPush, Prop_Send, "m_vecMaxs", fMaxbounds2);
	SetEntProp(iPush, Prop_Send, "m_nSolidType", 3);
	int enteffects2 = GetEntProp(iPush, Prop_Send, "m_fEffects");
	enteffects2 |= 32;
	SetEntProp(iPush, Prop_Send, "m_fEffects", enteffects2);
	SetVariantString("!activator");
	AcceptEntityInput(iPush, "SetParent", iKnife);

	// Relay Trigger
	int iRelayTrigger = CreateEntityByName("logic_relay");
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_trigger_%d", g_iCounter);
	DispatchKeyValue(iRelayTrigger, "targetname", sBuffer);
	DispatchKeyValue(iRelayTrigger, "spawnflags", "0");
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_launch_up_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyValue(iRelayTrigger, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_launch_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyValue(iRelayTrigger, "OnTrigger", sBuffer);
	DispatchSpawn(iRelayTrigger);
	ActivateEntity(iRelayTrigger);
	SetVariantString("!activator");
	AcceptEntityInput(iRelayTrigger, "SetParent", iKnife);

	// Launch Compare
	int iLaunchCompare = CreateEntityByName("logic_compare");
	Format(sBuffer, sizeof(sBuffer), "tnt_compare_launch_%d", g_iCounter);
	DispatchKeyValue(iLaunchCompare, "targetname", sBuffer);
	DispatchKeyValue(iLaunchCompare, "InitialValue", "1");
	DispatchKeyValue(iLaunchCompare, "CompareValue", "0");
	DispatchKeyValue(iLaunchCompare, "OnEqualTo", "!self,SetValue,1,0,-1");
	Format(sBuffer, sizeof(sBuffer), "tnt_sprite_%d,HideSprite,,0,-1", g_iCounter);
	DispatchKeyValue(iLaunchCompare, "OnEqualTo", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_launch_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyValue(iLaunchCompare, "OnEqualTo", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_launch_up_%d,Disable,,0,-1", g_iCounter);
	DispatchKeyValue(iLaunchCompare, "OnEqualTo", sBuffer);
	DispatchKeyValue(iLaunchCompare, "OnNotEqualTo", "!self,SetValue,0,0,-1");
	Format(sBuffer, sizeof(sBuffer), "tnt_sprite_%d,ShowSprite,,0,-1", g_iCounter);
	DispatchKeyValue(iLaunchCompare, "OnNotEqualTo", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_launch_%d,Disable,,0,-1", g_iCounter);
	DispatchKeyValue(iLaunchCompare, "OnNotEqualTo", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_launch_up_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyValue(iLaunchCompare, "OnNotEqualTo", sBuffer);
	DispatchKeyValueVector(iLaunchCompare, "origin", fOrigin);
	DispatchSpawn(iLaunchCompare);
	ActivateEntity(iLaunchCompare);
	SetVariantString("!activator");
	AcceptEntityInput(iLaunchCompare, "SetParent", iKnife);

	// Relay Launch
	int iRelayLaunch = CreateEntityByName("logic_relay");
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_launch_%d", g_iCounter);
	DispatchKeyValue(iRelayLaunch, "targetname", sBuffer);
	DispatchKeyValue(iRelayLaunch, "spawnflags", "0");
	DispatchKeyValue(iRelayLaunch, "StartDisabled", "0");
	DispatchKeyValueVector(iRelayLaunch, "origin", fOrigin);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_trigger_%d,Disable,,0,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunch, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_trigger_%d,Enable,,20,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunch, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_holder_sparks_%d,StopSpark,,0,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunch, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_holder_sparks_%d,StartSpark,,20,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunch, "OnTrigger", sBuffer);
	DispatchSpawn(iRelayLaunch);
	ActivateEntity(iRelayLaunch);
	SetVariantString("!activator");
	AcceptEntityInput(iRelayLaunch, "SetParent", iKnife);
	HookSingleEntityOutput(iRelayLaunch, "OnTrigger", TNTUse, false);

	// Relay Launch Up
	int iRelayLaunchUp = CreateEntityByName("logic_relay");
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_launch_up_%d", g_iCounter);
	DispatchKeyValue(iRelayLaunchUp, "targetname", sBuffer);
	DispatchKeyValue(iRelayLaunchUp, "spawnflags", "0");
	DispatchKeyValue(iRelayLaunchUp, "StartDisabled", "1");
	DispatchKeyValueVector(iRelayLaunchUp, "origin", fOrigin);
	Format(sBuffer, sizeof(sBuffer), "tnt_push_%d,Enable,,0,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunchUp, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_push_%d,Disable,,0.5,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunchUp, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_trigger_%d,Disable,,0,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunchUp, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_trigger_%d,Enable,,20,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunchUp, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_holder_sparks_%d,StopSpark,,0,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunchUp, "OnTrigger", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_holder_sparks_%d,StartSpark,,20,-1", g_iCounter);
	DispatchKeyValue(iRelayLaunchUp, "OnTrigger", sBuffer);
	DispatchSpawn(iRelayLaunchUp);
	ActivateEntity(iRelayLaunchUp);
	SetVariantString("!activator");
	AcceptEntityInput(iRelayLaunchUp, "SetParent", iKnife);
	HookSingleEntityOutput(iRelayLaunchUp, "OnTrigger", TNTUse, false);

	// Game UI
	int iUI = CreateEntityByName("game_ui");
	Format(sBuffer, sizeof(sBuffer), "tnt_ui_%d", g_iCounter);
	DispatchKeyValue(iUI, "targetname", sBuffer);
	DispatchKeyValue(iUI, "spawnflags", "0");
	DispatchKeyValue(iUI, "FieldOfView", "-1.0");
	Format(sBuffer, sizeof(sBuffer), "tnt_relay_trigger_%d,Trigger,,0,-1", g_iCounter);
	DispatchKeyValue(iUI, "PressedAttack2", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_compare_launch_%d,Compare,,0,-1", g_iCounter);
	DispatchKeyValue(iUI, "PressedAttack", sBuffer);
	DispatchSpawn(iUI);
	ActivateEntity(iUI);
	SetVariantString("!activator");
	AcceptEntityInput(iUI, "SetParent", iKnife);

	// enable pickup trigger
	SetVariantString("!activator");
	AcceptEntityInput(iTrigger, "Enable");

	g_iCounter ++;
}

public void TNTPickup(const char[] output, int caller, int activator, float delay) {
	char sBuffer[128];
	Format(sBuffer, sizeof(sBuffer), "say ** %N has picked up TNT **", activator);
	SetVariantString(sBuffer);
	//AcceptEntityInput(g_iConsole, "Command");
	PrintToChat(activator, " RIGHT CLICK = Launch TNT and LEFT CLICK = Switch between 'put down' and 'throw'.");
}

public void TNTUse(const char[] output, int caller, int activator, float delay) {
	char sBuffer[256];
	float fOrigin[3];
	float fAngles[3];

	GetClientEyeAngles(activator, fAngles);
	GetClientEyePosition(activator, fOrigin);
	fAngles[0] = 0.0;
	fAngles[2] = 0.0;
	fOrigin[0] = fOrigin[0] + Cosine(DegToRad(fAngles[1])) * 46.63;
	fOrigin[1] = fOrigin[1] + Sine(DegToRad(fAngles[1])) * 46.63;
	fOrigin[2] = fOrigin[2] - 29.86;

	// Prop
	int iProp = CreateEntityByName("prop_physics_multiplayer");
	Format(sBuffer, sizeof(sBuffer), "tnt_prop_%d", g_iCounter);
	DispatchKeyValue(iProp, "targetname", sBuffer);
	DispatchKeyValue(iProp, "model", "models/props/furnitures/humans/barrel01b.mdl");
	DispatchKeyValue(iProp, "spawnflags", "256");
	DispatchKeyValue(iProp, "physdamagescale", "0.1");
	DispatchKeyValue(iProp, "PerformanceMode", "0");
	DispatchKeyValue(iProp, "nodamageforces", "0");
	DispatchKeyValue(iProp, "solid", "6");
	DispatchKeyValue(iProp, "minhealthdmg", "999999");
	DispatchKeyValue(iProp, "health", "999999");
	DispatchKeyValueVector(iProp, "origin", fOrigin);
	Format(sBuffer, sizeof(sBuffer), "tnt_fire_%d,StartFire,,3,1", g_iCounter);
	DispatchKeyValue(iProp, "OnUser1", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_explosion_%d,Explode,,6.5,1", g_iCounter);
	DispatchKeyValue(iProp, "OnUser1", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "tnt_sound_%d,PlaySound,,6.5,1", g_iCounter);
	DispatchKeyValue(iProp, "OnUser1", sBuffer);
	Format(sBuffer, sizeof(sBuffer), "!self,Kill,,6.6,1", g_iCounter);
	DispatchKeyValue(iProp, "OnUser1", sBuffer);
	DispatchSpawn(iProp);
	ActivateEntity(iProp);
	AcceptEntityInput(iProp, "FireUser1");

	// Sparks
	int iSparks = CreateEntityByName("env_spark");
	Format(sBuffer, sizeof(sBuffer), "tnt_sparks_%d", g_iCounter);
	DispatchKeyValue(iSparks, "targetname", sBuffer);
	DispatchKeyValue(iSparks, "TrailLength", "1");
	DispatchKeyValue(iSparks, "spawnflags", "64");
	DispatchKeyValue(iSparks, "angles", "0 0 0");
	DispatchKeyValue(iSparks, "MaxDelay", "0");
	DispatchKeyValue(iSparks, "Magnitude", "1");
	DispatchKeyValueVector(iSparks, "origin", fOrigin);
	DispatchSpawn(iSparks);
	ActivateEntity(iSparks);
	SetVariantString("!activator");
	AcceptEntityInput(iSparks, "SetParent", iProp);

	// Fire
	int iFire = CreateEntityByName("env_fire");
	Format(sBuffer, sizeof(sBuffer), "tnt_fire_%d", g_iCounter);
	DispatchKeyValue(iFire, "targetname", sBuffer);
	DispatchKeyValue(iFire, "ignitionpoint", "32");
	DispatchKeyValue(iFire, "spawnflags", "17");
	DispatchKeyValue(iFire, "health", "30");
	DispatchKeyValue(iFire, "firesize", "64");
	DispatchKeyValue(iFire, "fireattack", "4");
	DispatchKeyValue(iFire, "damagescale", "1.0");
	DispatchKeyValueVector(iFire, "origin", fOrigin);
	DispatchSpawn(iFire);
	ActivateEntity(iFire);
	SetVariantString("!activator");
	AcceptEntityInput(iFire, "SetParent", iProp);

	// Explosion
	int iExplosion = CreateEntityByName("env_explosion");
	Format(sBuffer, sizeof(sBuffer), "tnt_explosion_%d", g_iCounter);
	DispatchKeyValue(iExplosion, "targetname", sBuffer);
	DispatchKeyValue(iExplosion, "fireballsprite", "sprites/zerogxplode.spr");
	DispatchKeyValue(iExplosion, "iMagnitude", "220");
	DispatchKeyValue(iExplosion, "iRadiusOverride", "1024");
	DispatchKeyValue(iExplosion, "rendermode", "5");
	DispatchKeyValue(iExplosion, "spawnflags", "16");
	DispatchKeyValueVector(iExplosion, "origin", fOrigin);
	DispatchSpawn(iExplosion);
	ActivateEntity(iExplosion);
	SetVariantString("!activator");
	AcceptEntityInput(iExplosion, "SetParent", iProp);

	// Sound
	int iSound = CreateEntityByName("ambient_generic");
	Format(sBuffer, sizeof(sBuffer), "tnt_sound_%d", g_iCounter);
	DispatchKeyValue(iSound, "targetname", sBuffer);
	DispatchKeyValue(iSound, "spawnflags", "48");
	DispatchKeyValue(iSound, "radius", "9800");
	DispatchKeyValue(iSound, "message", "ambient/explosions/explode_9.wav");
	DispatchKeyValue(iSound, "volume", "10");
	DispatchKeyValue(iSound, "health", "10");
	DispatchKeyValue(iSound, "pitch", "100");
	DispatchKeyValue(iSound, "pitchstart", "100");
	DispatchKeyValueVector(iSound, "origin", fOrigin);
	DispatchSpawn(iSound);
	ActivateEntity(iSound);
	SetVariantString("!activator");
	AcceptEntityInput(iSound, "SetParent", iProp);

	g_iCounter ++;
}