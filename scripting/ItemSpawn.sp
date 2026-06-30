#pragma newdecls required

#include <sdktools>
#include <sdkhooks>
#include <zombiereloaded>
#include <cstrike>

#pragma semicolon 1

int g_iCounter = 0;

bool g_bClientHasItem[MAXPLAYERS + 1] = {false, ...};

Handle SDKCall_GetSlot;
Handle SDKCall_BumpWeapon;
Handle SDKCall_OnPickedUp;

#include "itemspawn/balrog.inc"
#include "itemspawn/doghuman.inc"
#include "itemspawn/earth.inc"
#include "itemspawn/jumper.inc"
#include "itemspawn/tnt.inc"
#include "itemspawn/vortigaunt.inc"
#include "itemspawn/whiteknight.inc"

public Plugin myinfo = {
	name        = "ItemSpawn",
	author      = "Neon, koen",
	description = "",
	version     = "1.2.0",
}

public void OnPluginStart() {
	HookEvent("round_start", OnRoundStart, EventHookMode_Post);

	RegAdminCmd("sm_balrog", Command_Balrog, ADMFLAG_ROOT);
	RegAdminCmd("sm_humandog", Command_HumanDog, ADMFLAG_ROOT);
	RegAdminCmd("sm_earth", Command_Earth, ADMFLAG_ROOT);
	RegAdminCmd("sm_jumper", Command_Jumper, ADMFLAG_ROOT);
	RegAdminCmd("sm_tnt", Command_TNT, ADMFLAG_ROOT);
	RegAdminCmd("sm_vortigaunt", Command_Vortigaunt, ADMFLAG_ROOT);
	RegAdminCmd("sm_whiteknight", Command_WhiteKnight, ADMFLAG_ROOT);

	LoadTranslations("common.phrases");

	GameData hGameConf;
	if ((hGameConf = new GameData("ItemSpawn.games")) == null) {
		SetFailState("Failed to load \"ItemSpawn.games\" game config!");
		return;
	}

	// "CBaseCombatWeapon::GetSlot"
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(hGameConf, SDKConf_Virtual, "CBaseCombatWeapon::GetSlot")) {
		delete hGameConf;
		SetFailState("Failed to setup SDKCall \"SDKCall_GetSlot\"!");
		return;
	}

	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
	if ((SDKCall_GetSlot = EndPrepSDKCall()) == null) {
		delete hGameConf;
		SetFailState("Failed to end SDKCall \"SDKCall_GetSlot\"!");
		return;
	}

	// "CBaseCombatWeapon::OnPickedUp"
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(hGameConf, SDKConf_Virtual, "CBaseCombatWeapon::OnPickedUp")) {
		delete hGameConf;
		SetFailState("Failed to setup SDKCall \"SDKCall_OnPickedUp\"!");
		return;
	}

	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
	if ((SDKCall_OnPickedUp = EndPrepSDKCall()) == null) {
		delete hGameConf;
		SetFailState("Failed to end SDKCall \"SDKCall_OnPickedUp\"!");
		return;
	}

	// "CBasePlayer::BumpWeapon"
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(hGameConf, SDKConf_Virtual, "CBasePlayer::BumpWeapon")) {
		delete hGameConf;
		SetFailState("Failed to setup SDKCall \"SDKCall_BumpWeapon\"!");
		return;
	}

	PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
	if ((SDKCall_BumpWeapon = EndPrepSDKCall()) == null) {
		delete hGameConf;
		SetFailState("Failed to end SDKCall \"SDKCall_BumpWeapon\"!");
		return;
	}

	delete hGameConf;
}

public void OnMapStart() {
	// Physbox model
	PrecacheModel("models/props/cs_militia/crate_extrasmallmill.mdl");

	// Balrog
	PrecacheModel("models/player/slow/amberlyn/lotr/balrog/balrog_rafuron_hannibal.mdl");
	AddFileToDownloadsTable("models/player/slow/amberlyn/lotr/balrog/balrog_rafuron_hannibal.dx80.vtx");
	AddFileToDownloadsTable("models/player/slow/amberlyn/lotr/balrog/balrog_rafuron_hannibal.dx90.vtx");
	AddFileToDownloadsTable("models/player/slow/amberlyn/lotr/balrog/balrog_rafuron_hannibal.mdl");
	// AddFileToDownloadsTable("models/player/slow/amberlyn/lotr/balrog/balrog_rafuron_hannibal.sw.vtx");
	AddFileToDownloadsTable("models/player/slow/amberlyn/lotr/balrog/balrog_rafuron_hannibal.vvd");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_body.vmt");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_body.vtf");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_body_2.vmt");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_eyes.vmt");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_weapon.vmt");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_weapon.vtf");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_weapon_blade.vmt");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_weapon_bump.vtf");
	AddFileToDownloadsTable("materials/models/player/slow/amberlyn/lotr/balrog/slow_wings.vmt");
	AddFileToDownloadsTable("sound/nide/balrog_scream.mp3");

	// Dog
	PrecacheModel("models/player/pil/re1/dog/dog_pil.mdl");
	AddFileToDownloadsTable("models/player/pil/re1/dog/dog_pil.dx80.vtx");
	AddFileToDownloadsTable("models/player/pil/re1/dog/dog_pil.dx90.vtx");
	AddFileToDownloadsTable("models/player/pil/re1/dog/dog_pil.mdl");
	AddFileToDownloadsTable("models/player/pil/re1/dog/dog_pil.phy");
	// AddFileToDownloadsTable("models/player/pil/re1/dog/dog_pil.sw.vtx");
	AddFileToDownloadsTable("models/player/pil/re1/dog/dog_pil.vvd");
	AddFileToDownloadsTable("materials/models/player/pil/re1/dog/0000a.vmt");
	AddFileToDownloadsTable("materials/models/player/pil/re1/dog/0000a.vtf");

	// Dog Poo
	PrecacheModel("models/poo/curlygpoo.mdl");
	AddFileToDownloadsTable("models/poo/curlygpoo.dx80.vtx");
	AddFileToDownloadsTable("models/poo/curlygpoo.dx90.vtx");
	AddFileToDownloadsTable("models/poo/curlygpoo.mdl");
	AddFileToDownloadsTable("models/poo/curlygpoo.phy");
	// AddFileToDownloadsTable("models/poo/curlygpoo.sw.vtx");
	AddFileToDownloadsTable("models/poo/curlygpoo.vvd");
	AddFileToDownloadsTable("materials/models/poo/curlypoo.vmt");
	AddFileToDownloadsTable("materials/models/poo/curlypoo.vtf");

	// Earth Staff
	PrecacheModel("models/staff/staff.mdl");
	AddFileToDownloadsTable("models/staff/staff.dx80.vtx");
	AddFileToDownloadsTable("models/staff/staff.dx90.vtx");
	AddFileToDownloadsTable("models/staff/staff.mdl");
	AddFileToDownloadsTable("models/staff/staff.phy");
	// AddFileToDownloadsTable("models/staff/staff.sw.vtx");
	AddFileToDownloadsTable("models/staff/staff.vvd");
	AddFileToDownloadsTable("materials/models/Staff/staffofmagnus.vmt");
	AddFileToDownloadsTable("materials/models/Staff/staffofmagnus.vtf");

	// Earth Prop
	PrecacheModel("models/ffxii/earthmodel1.mdl");
	AddFileToDownloadsTable("models/ffxii/earthmodel1.dx80.vtx");
	AddFileToDownloadsTable("models/ffxii/earthmodel1.dx90.vtx");
	AddFileToDownloadsTable("models/ffxii/earthmodel1.mdl");
	AddFileToDownloadsTable("models/ffxii/earthmodel1.phy");
	// AddFileToDownloadsTable("models/ffxii/earthmodel1.sw.vtx");
	AddFileToDownloadsTable("models/ffxii/earthmodel1.vvd");
	AddFileToDownloadsTable("materials/models/ffxii/earthmodel1/rockwall01.vmt");

	// TNT
	PrecacheModel("models/props/furnitures/humans/barrel01b.mdl");
	AddFileToDownloadsTable("models/props/furnitures/humans/barrel01b.dx80.vtx");
	AddFileToDownloadsTable("models/props/furnitures/humans/barrel01b.dx90.vtx");
	AddFileToDownloadsTable("models/props/furnitures/humans/barrel01b.mdl");
	AddFileToDownloadsTable("models/props/furnitures/humans/barrel01b.phy");
	// AddFileToDownloadsTable("models/props/furnitures/humans/barrel01b.sw.vtx");
	AddFileToDownloadsTable("models/props/furnitures/humans/barrel01b.vvd");
	AddFileToDownloadsTable("materials/models/barrel01b/wood_barrel01b.vmt");
	AddFileToDownloadsTable("materials/models/barrel01b/wood_barrel01b.vtf");
	AddFileToDownloadsTable("materials/models/barrel01b/wood_barrel01b_broken.vmt");
	AddFileToDownloadsTable("materials/models/barrel01b/wood_barrel01b_no_metal.vtf");

	// Vortigaunt
	PrecacheModel("models/vortigaunt_slave.mdl");

	// WhiteKnight
	PrecacheModel("models/dog_jugger.mdl");
	AddFileToDownloadsTable("models/dog_jugger.dx80.vtx");
	AddFileToDownloadsTable("models/dog_jugger.dx90.vtx");
	AddFileToDownloadsTable("models/dog_jugger.mdl");
	// AddFileToDownloadsTable("models/dog_jugger.sw.vtx");
	AddFileToDownloadsTable("models/dog_jugger.vvd");
	AddFileToDownloadsTable("materials/models/dog_gondor/dog_sheet.vmt");
	AddFileToDownloadsTable("materials/models/dog_gondor/dog_sheet.vtf");
	AddFileToDownloadsTable("materials/models/dog_gondor/dog_sheet_phong.vtf");
	AddFileToDownloadsTable("materials/models/dog_gondor/eyeglass.vmt");
	AddFileToDownloadsTable("materials/models/dog_gondor/eyeglass.vtf");
	AddFileToDownloadsTable("materials/models/dog_gondor/weapon107_000_001.vmt");
	AddFileToDownloadsTable("materials/models/dog_gondor/weapon107_000_001.vtf");
	AddFileToDownloadsTable("materials/models/dog_gondor/weapon107_000_002.vtf");
}

stock int CreateEntityAtOrigin(const char[] classname, const float origin[3]) {
	int entity = CreateEntityByName(classname);
	TeleportEntity(entity, origin, NULL_VECTOR, NULL_VECTOR);
	return entity;
}

stock bool DispatchKeyFormat(int entity, const char[] key, const char[] value, any ...) {
	char buffer[1024];
	VFormat(buffer, sizeof(buffer), value, 4);
	DispatchKeyValue(entity, key, buffer);
	return true;
}

stock void SpawnAndActivate(int entity) {
	DispatchSpawn(entity);
	ActivateEntity(entity);
}

stock void ParentToEntity(int entity, int parent) {
	SetVariantString("!activator");
	AcceptEntityInput(entity, "SetParent", parent, parent);
}

stock void SetEntityBBox(int entity, const float mins[3], const float maxs[3]) {
	SetEntPropVector(entity, Prop_Send, "m_vecMins", mins);
	SetEntPropVector(entity, Prop_Send, "m_vecMaxs", maxs);
}

stock void SetEntityProps(int entity) {
	SetEntProp(entity, Prop_Send, "m_nSolidType", 3);
	SetEntProp(entity, Prop_Send, "m_fEffects", 32);
}

stock void AddItemFilter(int entity) {
	SDKHook(entity, SDKHook_StartTouch, OnTriggerTouch);
	SDKHook(entity, SDKHook_EndTouch, OnTriggerTouch);
	SDKHook(entity, SDKHook_Touch, OnTriggerTouch);
}

stock bool IsValidClient(int client) {
	return ((1 <= client <= MaxClients) && IsClientConnected(client) && IsClientInGame(client) && !IsFakeClient(client));
}

public Action OnTriggerTouch(int trigger, int client) {
	return (IsValidClient(client) && g_bClientHasItem[client]) ? Plugin_Handled : Plugin_Continue;
}

public Action EquipWeapons(Handle timer, any userid) {
	int client = GetClientOfUserId(userid);
	if (client != 0) {
		GivePlayerItem(client, "weapon_p90");
		GivePlayerItem(client, "weapon_elite");
		GivePlayerItem(client, "item_kevlar");
		GivePlayerItem(client, "weapon_hegrenade");
	}
	return Plugin_Stop;
}

public void OnClientDisconnect(int client) {
	g_bClientHasItem[client] = false;
}

public void OnRoundStart(Event hEvent, const char[] sEvent, bool bDontBroadcast)
{
	for (int client = 1; client <= MaxClients; client++) {
		g_bClientHasItem[client] = false;
	}

	g_iCounter = 0;

	// player_weaponstrip.
	int iPlayerStrip = CreateEntityByName("player_weaponstrip");
	DispatchKeyFormat(iPlayerStrip, "targetname",       "item_spawn_weaponstrip");
	SpawnAndActivate(iPlayerStrip);

	// player_speedmod.
	int iPlayerSpeed = CreateEntityByName("player_speedmod");
	DispatchKeyFormat(iPlayerSpeed, "targetname",       "item_spawn_speedmod");
	SpawnAndActivate(iPlayerSpeed);

	// filter_activator_class nodamage.
	int iNoDamageFilter = CreateEntityByName("filter_activator_class");
	DispatchKeyFormat(iNoDamageFilter, "targetname",    "item_filter_nodamage");
	DispatchKeyFormat(iNoDamageFilter, "Negated",       "0");
	DispatchKeyFormat(iNoDamageFilter, "filterclass",   "light");
	SpawnAndActivate(iNoDamageFilter);

	// filter_activator_team human.
	int iHumanFilter1 = CreateEntityByName("filter_activator_team");
	DispatchKeyFormat(iHumanFilter1, "targetname",      "item_filter_human");
	DispatchKeyFormat(iHumanFilter1, "Negated",         "0");
	DispatchKeyFormat(iHumanFilter1, "filterteam",      "3");
	SpawnAndActivate(iHumanFilter1);

	// filter_activator_team human ignored.
	int iHumanFilter2 = CreateEntityByName("filter_activator_team");
	DispatchKeyFormat(iHumanFilter2, "targetname",      "item_filter_human_ignored");
	DispatchKeyFormat(iHumanFilter2, "Negated",         "1");
	DispatchKeyFormat(iHumanFilter2, "filterteam",      "3");
	SpawnAndActivate(iHumanFilter2);

	// filter_damage_type human items.
	int iHumanFilter3 = CreateEntityByName("filter_damage_type");
	DispatchKeyFormat(iHumanFilter3, "targetname",      "item_filter_human_items");
	DispatchKeyFormat(iHumanFilter3, "Negated",         "0");
	DispatchKeyFormat(iHumanFilter3, "damagetype",      "512");
	SpawnAndActivate(iHumanFilter3);

	// filter_multi humans.
	int iHumanFilter4 = CreateEntityByName("filter_multi");
	DispatchKeyFormat(iHumanFilter4, "targetname",      "item_filter_humans");
	DispatchKeyFormat(iHumanFilter4, "Negated",         "0");
	DispatchKeyFormat(iHumanFilter4, "filtertype",      "1");
	DispatchKeyFormat(iHumanFilter4, "filter01",        "item_filter_human");
	DispatchKeyFormat(iHumanFilter4, "filter02",        "item_filter_human_items");
	SpawnAndActivate(iHumanFilter4);

	// filter_activator_team zombie.
	int iZombieFilter1 = CreateEntityByName("filter_activator_team");
	DispatchKeyFormat(iZombieFilter1, "targetname",     "item_filter_zombie");
	DispatchKeyFormat(iZombieFilter1, "Negated",        "0");
	DispatchKeyFormat(iZombieFilter1, "filterteam",     "2");
	SpawnAndActivate(iZombieFilter1);

	// filter_activator_team zombie ignored.
	int iZombieFilter2 = CreateEntityByName("filter_activator_team");
	DispatchKeyFormat(iZombieFilter2, "targetname",     "item_filter_zombie_ignored");
	DispatchKeyFormat(iZombieFilter2, "Negated",        "1");
	DispatchKeyFormat(iZombieFilter2, "filterteam",     "2");
	SpawnAndActivate(iZombieFilter2);

	// filter_damage_type zombie items.
	int iZombieFilter3 = CreateEntityByName("filter_damage_type");
	DispatchKeyFormat(iZombieFilter3, "targetname",     "item_filter_zombie_items");
	DispatchKeyFormat(iZombieFilter3, "Negated",        "0");
	DispatchKeyFormat(iZombieFilter3, "damagetype",     "128");
	SpawnAndActivate(iZombieFilter3);

	// filter_multi zombies.
	int iZombieFilter4 = CreateEntityByName("filter_multi");
	DispatchKeyFormat(iZombieFilter4, "targetname",     "item_filter_zombies");
	DispatchKeyFormat(iZombieFilter4, "Negated",        "0");
	DispatchKeyFormat(iZombieFilter4, "filtertype",     "1");
	DispatchKeyFormat(iZombieFilter4, "filter01",       "item_filter_zombie");
	DispatchKeyFormat(iZombieFilter4, "filter02",       "item_filter_zombie_items");
	SpawnAndActivate(iZombieFilter4);
}