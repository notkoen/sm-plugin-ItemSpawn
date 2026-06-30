public Action Command_Earth(int client, int argc) {
	float fOrigin[3];

	if (argc < 1) {
		GetClientEyePosition(client, fOrigin);
		SpawnEarthStaff(fOrigin);
		LogAction(client, -1, "\"%L\" spawned Earth-Staff at <%f><%f><%f>.", client, fOrigin[0], fOrigin[1], fOrigin[2]);
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
		if (IsClientInGame(iTargets[i]) && IsPlayerAlive(iTargets[i]) && (ZR_IsClientHuman(iTargets[i]))) {
			GetClientEyePosition(iTargets[i], fOrigin);
			SpawnEarthStaff(fOrigin, iTargets[i]);
			LogAction(client, -1, "\"%L\" gave Earth-Staff to \"%L\".", client, iTargets[i]);
		}
	}
	return Plugin_Handled;
}

void SpawnEarthStaff(float fOrigin[3], int client = -1) {
	float fOriginTemp[3];

	// weapon_knife.
	int iPistol = CreateEntityAtOrigin("weapon_elite", fOrigin);
	DispatchKeyFormat(iPistol, "targetname",                "item_earth_staff_pistol_%d", g_iCounter);
	DispatchKeyFormat(iPistol, "hammerid",                  "11051995%d", g_iCounter);
	DispatchKeyFormat(iPistol, "spawnflags",                "1");
	DispatchKeyFormat(iPistol, "angles",                    "0 0 0");
	DispatchKeyFormat(iPistol, "ammo",                      "999");
	DispatchKeyFormat(iPistol, "OnPlayerPickup",            "item_earth_staff_controls_%d,Activate,,0,1", g_iCounter);
	SpawnAndActivate(iPistol);

	HookSingleEntityOutput(iPistol, "OnPlayerPickup", EarthStaffPickup, false);

	// model origin.
	fOriginTemp[0] = fOrigin[0] + 24.0;
	fOriginTemp[1] = fOrigin[1] - 16.0;
	fOriginTemp[2] = fOrigin[2] - 32.0;

	// prop_dynamic model.
	int iModel = CreateEntityAtOrigin("prop_dynamic", fOriginTemp);
	DispatchKeyFormat(iModel, "targetname",                 "item_earth_staff_model_%d", g_iCounter);
	DispatchKeyFormat(iModel, "model",                      "models/staff/staff.mdl");
	DispatchKeyFormat(iModel, "disablebonefollowers",       "1");
	DispatchKeyFormat(iModel, "defaultanim",                "idle");
	DispatchKeyFormat(iModel, "angles",                     "0 270 0");
	DispatchKeyFormat(iModel, "solid",                      "0");
	SpawnAndActivate(iModel);
	ParentToEntity(iModel, iPistol);

	// ambient_generic earth.
	int iSoundEarth = CreateEntityAtOrigin("ambient_generic", fOrigin);
	DispatchKeyFormat(iSoundEarth, "targetname",            "item_earth_staff_sound_%d", g_iCounter);
	DispatchKeyFormat(iSoundEarth, "spawnflags",            "48");
	DispatchKeyFormat(iSoundEarth, "radius",                "1250");
	DispatchKeyFormat(iSoundEarth, "sourceentityname",      "item_earth_staff_model_%d", g_iCounter);
	DispatchKeyFormat(iSoundEarth, "message",               "physics/concrete/boulder_impact_hard1.wav");
	DispatchKeyFormat(iSoundEarth, "volume",                "10");
	DispatchKeyFormat(iSoundEarth, "health",                "10");
	DispatchKeyFormat(iSoundEarth, "pitch",                 "100");
	DispatchKeyFormat(iSoundEarth, "pitchstart",            "100");
	SpawnAndActivate(iSoundEarth);
	ParentToEntity(iSoundEarth, iPistol);

	// logic_relay earth.
	int iRelayEarth = CreateEntityAtOrigin("logic_relay", fOrigin);
	DispatchKeyFormat(iRelayEarth, "targetname",        "item_earth_staff_relay_%d", g_iCounter);
	DispatchKeyFormat(iRelayEarth, "spawnflags",        "0");
	DispatchKeyFormat(iRelayEarth, "OnTrigger",         "!self,Disable,,0,-1");
	DispatchKeyFormat(iRelayEarth, "OnTrigger",         "!self,Enable,,5,-1");
	DispatchKeyFormat(iRelayEarth, "OnTrigger",         "item_earth_staff_sound_%d,PlaySound,,0.75,-1", g_iCounter);
	DispatchKeyFormat(iRelayEarth, "OnTrigger",         "item_earth_staff_model_%d,SetAnimation,Earth,0,-1", g_iCounter);
	SpawnAndActivate(iRelayEarth);
	ParentToEntity(iRelayEarth, iPistol);

	HookSingleEntityOutput(iRelayEarth, "OnTrigger", EarthStaffUse, false);

	// game_ui.
	int iControls = CreateEntityAtOrigin("game_ui", fOrigin);
	DispatchKeyFormat(iControls, "targetname",          "item_earth_staff_controls_%d", g_iCounter);
	DispatchKeyFormat(iControls, "spawnflags",          "0");
	DispatchKeyFormat(iControls, "fieldofview",         "-1.0");
	DispatchKeyFormat(iControls, "PressedAttack2",      "item_earth_staff_relay_%d,Trigger,,0,-1", g_iCounter);
	SpawnAndActivate(iControls);
	ParentToEntity(iControls, iPistol);

	g_iCounter++;

	if (client > 0) {
		int weaponSlot = SDKCall(SDKCall_GetSlot, iPistol);
		int weaponInSlot = GetPlayerWeaponSlot(client, weaponSlot);
		if (weaponInSlot != -1) {
			CS_DropWeapon(client, weaponInSlot, false);
		}

		if (SDKCall(SDKCall_BumpWeapon, client, iPistol)) {
			SDKCall(SDKCall_OnPickedUp, iPistol, client);
		}
	}
}

public void EarthStaffPickup(const char[] output, int caller, int activator, float delay) {
	ServerCommand("say ** %N has picked up the Earth-Staff **", activator);
	PrintToChat(activator, "Right Click with Knife = Use.");
}

public void EarthStaffUse(const char[] output, int caller, int activator, float delay) {
	CreateTimer(0.75, SpawnEarth, GetClientUserId(activator), TIMER_FLAG_NO_MAPCHANGE);
}

public Action SpawnEarth(Handle timer, any userid)
{
	int activator = GetClientOfUserId(userid);

	float fOrigin[3];
	float fAngles[3];
	GetClientEyeAngles(activator, fAngles);
	GetClientEyePosition(activator, fOrigin);
	fAngles[0] = 0.0;
	fAngles[2] = 0.0;
	fOrigin[0] = fOrigin[0] + Cosine(DegToRad(fAngles[1])) * 334.0;
	fOrigin[1] = fOrigin[1] + Sine(DegToRad(fAngles[1])) * 334.0;
	fOrigin[2] = fOrigin[2] - 23.00;

	// prop_dynamic model.
	int iModel = CreateEntityAtOrigin("prop_dynamic", fOrigin);
	DispatchKeyFormat(iModel, "targetname",             "item_earth_prop_%d", g_iCounter);
	DispatchKeyFormat(iModel, "model",                  "models/ffxii/earthmodel1.mdl");
	DispatchKeyFormat(iModel, "spawnflags",             "0");
	DispatchKeyFormat(iModel, "PerformanceMode",        "0");
	DispatchKeyFormat(iModel, "solid",                  "6");
	DispatchKeyFormat(iModel, "OnUser1",                "!self,Break,,6,-1");
	DispatchKeyFormat(iModel, "OnUser1",                "!self,Kill,,6,-1");
	SpawnAndActivate(iModel);
	AcceptEntityInput(iModel, "FireUser1");

	g_iCounter ++;
	return Plugin_Stop;
}