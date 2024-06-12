/*

░██╗░░░░░░░██╗███╗░░░███╗██████╗░
░██║░░██╗░░██║████╗░████║██╔══██╗
░╚██╗████╗██╔╝██╔████╔██║██████╔╝
░░████╔═████║░██║╚██╔╝██║██╔═══╝░
░░╚██╔╝░╚██╔╝░██║░╚═╝░██║██║░░░░░
░░░╚═╝░░░╚═╝░░╚═╝░░░░░╚═╝╚═╝░░░░░*/

#define Game
#define HUD_ELEMS
#define TOC (0x724C38)
#include "Mem.h"
#include "PS3Lib.h"
#include "Structs.h"
#include <string>
#include <ctime>

/*Button Definitions*/
#define DPAD_UP(clientNum) (BtnPressed(clientNum, "+actionslot 1"))
#define DPAD_DOWN(clientNum) (BtnPressed(clientNum, "+actionslot 2"))
#define DPAD_LEFT(clientNum) (BtnPressed(clientNum, "+actionslot 3"))
#define DPAD_RIGHT(clientNum) (BtnPressed(clientNum, "+actionslot 4"))
#define BUTTON_A(clientNum) (BtnPressed(clientNum, "+gostand"))
#define BUTTON_X(clientNum) (BtnPressed(clientNum, "+usereload"))
#define BUTTON_RSTICK(clientNum) (BtnPressed(clientNum, "+melee"))
#define RESET_BTN(clientNum) (*(short*)(*(int*)0x1BF911C + (clientNum * 0x97F80) + 0x20E9F) = 0)
/*End of button definitions*/

bool colorsChanged = false;
bool TextChanged = false;
bool ShadersChanged = false;
bool forceHostEnabled = false;
int forceHostTimeout = 0;
int myIndex;

struct opd_s {
	unsigned int sub;
	unsigned int toc;
};

struct Client {
	bool hasVision;
	bool localizedStrings;
	bool hasHuds;
} client_t[18];

void Sleep(usecond_t time)
{
	sys_timer_usleep(time * 1000);
}

namespace Game {

	opd_s key = {0x238070,0x724C38};
	void (*Keyboard)(int,char* Title, char* PresetText, int MaxLength,int) = (void(*)(int,char*,char*,int,int))&key;

	opd_s cBuff_AddText_t = { 0x001D9EC0, 0x00724C38 };
    void(*cBuff_AddText)(int localClientIndex, const char* Command) = (void(*)(int, const char*))&cBuff_AddText_t;

	opd_s SV_GameSendServerCommand_t = { 0x0021A0A0, 0x00724C38 };
	void(*SV_GameSendServerCommand)(int clientIndex, int type, const char* Command) = (void(*)(int, int, const char*))&SV_GameSendServerCommand_t;

	int (*stdc_printf)(const char*, ...) = (int(*)(const char*, ...))0x0071C2B0;
    int (*stdc_snprintf)(char* destination, size_t len, const char* format, ...) = (int(*)(char*, size_t, const char*, ...))0x0071C2C0;

	opd_s Dvar_GetBoolAddress = { 0x00277138, TOC };
	bool(*Dvar_GetBool)(const char *dvarName) = (bool(*)(const char*))&Dvar_GetBoolAddress;

	opd_s Dvar_GetStringAddress = { 0x00276F60, TOC };
	const char*(*Dvar_GetString)(const char* Dvar) = (const char*(*)(const char*))&Dvar_GetStringAddress;

	opd_s SV = { 0x0021A0A0, TOC };
	void(*svgssc)(int clientNum, int type, char* cmd) = (void(*)(int, int, char*))&SV;
	void(*Kick_Client)(int clientNum, const char* Reason, bool NotifyClient) = (void(*)(int, const char*, bool))0x0070A890;

	
	void Cbuf_AddText(char* Command)
	{
		opd_s Cbuf1 = {0x001D9EC0 , 0x00724C38 };
		void(*Cbuf)(int type,char* cmd) = (void(*)(int,char*))&Cbuf1;
		Cbuf(0, Command);
	}

	void iPrintlnBold(int client, const char* txt)
	{
			char cmd[0x50];
			stdc_snprintf(cmd, sizeof(cmd), "c \"%s\"", txt);
			SV_GameSendServerCommand(client, 0, cmd);
	}

	bool IC[18];
	void IceClan(int client)
	{
		if(IC[client] == false)
		{
			SV_GameSendServerCommand(client, 1, "v clanname WMP|\"");
			SV_GameSendServerCommand(client, 1, "g \"Clients ClanTag ^7[^6Modified^7]\"");
			IC[client] = true;
		}
	}

	char* CallKeyBoard(char* Title, char* PresetText = "", int MaxLength = 16)
{
    Keyboard(0, Title, PresetText, MaxLength, 0x70B4D8);
    Sleep(50);

    // Set the timeout duration (e.g., 5000 ms or 5 seconds)
    int timeout_ms = 3000;
    int elapsed_ms = 0;

    // Loop to check the condition with a timeout
    while (*(int*)(0x203B4C8) != 0)
    {
        // Sleep for a short period to avoid busy waiting
        Sleep(10);
        elapsed_ms += 10;

        // Check if the elapsed time has exceeded the timeout duration
        if (elapsed_ms >= timeout_ms)
        {
            // Timeout reached, exit the loop
            break;
        }
    }

    return ReadStringC(0x2510E22);
}

void setName(char *name) {
	strcpy((char*)0x014E8B90,name);
}
void setNameC(int cl, char *name) {
	strcpy((char*)(0x014E8B90+ (cl*0x3700)),name);
}

	void ChangeClientName(int cl)
	{
		char buff[100];
		char* name = (char*)CallKeyBoard("Enter Custom Name","",16);
		stdc_snprintf(buff, sizeof(buff), "%s", name);

		setNameC(cl, buff);
	}

	void ChangeName(int cl)
	{
		char buff[100];
		char* name = (char*)CallKeyBoard("Enter Custom Name","",16);
		stdc_snprintf(buff, sizeof(buff), "%s", name);

		setName(buff);
	}

	enum svscmd_type {
	SV_CMD_CAN_IGNORE = 0x0,
	SV_CMD_RELIABLE = 0x1,
    };

	void iPrintln(int clientNum, const char* text) {
	char buffer[0x200];
	sprintf(buffer, "%c \"%s\"", 'f', text);
	SV_GameSendServerCommand(clientNum, SV_CMD_CAN_IGNORE, buffer);
    }

	unsigned int G_Client(int client, unsigned int Mod = 0)
{
	return (0x14E2200 + (0x3700 * client)) + Mod;
}

    unsigned int G_Entity(int client, unsigned int Mod = 0)
{
    return *(int*)0x1319800 + (0x280 * client) + Mod;
}

    int gEntity(int client, int mod = 0x00)
{
	return (0x1319800 + mod + (client * 0x280));
}

	int gClient(int client, int mod=0x00)
{
	return (0x14E2200 + mod + (client * 0x3700));
}

	int getPlayerName(int client)
{
	return (0x14E5408 + (client * 0x3700));
}

	typedef int(__cdecl *Drop_Weapon_t)(gentity_s *pEnt, int weaponIndex, char weaponModel, unsigned int tag);
    extern Drop_Weapon_t Drop_Weapon;
	opd_s Drop_Weapon_s = { 0x1861C8, TOC };
    Drop_Weapon_t Drop_Weapon = Drop_Weapon_t(&Drop_Weapon_s);

	opd_s G_GivePlayerWeapon_t = { 0x001C0890, 0x00724C38 };
	typedef void(*G_GPW)(int pPS, int iWeaponIndex, int Camo, int Akimbo);
	G_GPW G_GivePlayerWeapon = (G_GPW)(opd_s*)&G_GivePlayerWeapon_t;

	opd_s G_GetWeaponIndexForName_t = { 0x001BF4A0, 0x00724C38 };
	typedef int(*G_GWIFN)(const char *weaponName);
	G_GWIFN GetWeaponIndex = (G_GWIFN)(opd_s*)&G_GetWeaponIndexForName_t;

	opd_s giveAmmo_t = { 0x00186638, 0x00724C38 };
	typedef void(*gA)(int clientNumber, unsigned int weaponIndex, char weaponModel, int count, int fillClip);
	gA giveAmmo = (gA)(opd_s*)&giveAmmo_t;

	void Sleep(usecond_t time)
{
	sys_timer_usleep(time * 1000);
}

    int ReadInt321(int Address)
{
		return *(int*)Address;
}

    char* ReadString1(int Address)
{
		return (char*)Address;
}

    char* GetMap()
{
	char* str = ReadString1(0xD495F4); char* MapStr = "/";
    if (Dvar_GetBool("cl_ingame") == 1)
    {
        if (str==("afghan"))
            MapStr = "Afghan";
        if (str==("highrise"))
            MapStr = "Highrise";
        if (str==("rundown"))
            MapStr = "Rundown";
        if (str==("quarry"))
            MapStr = "Quarry";
        if (str==("nightshift"))
            MapStr = "Skidrow";
        if (str==("terminal"))
            MapStr = "Terminal";
        if (str==("brecourt"))
            MapStr = "Wasteland";
        if (str==("derail"))
            MapStr = "Derail";
        if (str==("estate"))
            MapStr = "Estate";
        if (str==("favela"))
            MapStr = "Favela";
        if (str==("invasion"))
            MapStr = "Invasion";
        if (str==("rust"))
            MapStr = "Rust";
        if (str==("scrapyard") || str==(("boneyard")))
            MapStr = "Scrapyard";
        if (str==("sub"))
            MapStr = "Subbase";
        if (str==("underpass"))
            MapStr = "Underpass";
        if (str==("checkpoint"))
            MapStr = "Karachi";
        if (str==("bailout"))
            MapStr = "Bailout";
        if (str==("compact"))
            MapStr = "Salvage";
        if (str==("storm") || str==(("storm2")))
            MapStr = "Storm";
        if (str==("crash"))
            MapStr = "Crash";
        if (str==("overgrown"))
            MapStr = "Overgrown";
        if (str==("strike"))
            MapStr = "Strike";
        if (str==("vacant"))
            MapStr = "Vacant";
        if (str==("trailerpark"))
            MapStr = "Trailer Park";
        if (str==("fuel"))
            MapStr = "Fuel";
        if (str==("abandon"))
            MapStr = "Carnival";
    }
    return (char*)MapStr;
}

    int getCurrentWeapon(int clientIndex)
{
    int Index = ReadInt321(G_Client(clientIndex) + 0x2B4);
    char* MapName = GetMap();
    if ((MapName == "Afghan" || MapName == "Highrise" || MapName == "Karachi" || MapName == "Quarry" || MapName == "Rundown" || MapName == "Terminal" || MapName == "Skidrow" || MapName == "Wasteland"))
    {
        if (Index > 2)
            Index--;
    }
    return Index;
}

	void SwitchToWeap(int cl, const char *Weapon, int WeaponIndex = 0)
	{
		if (WeaponIndex == 0)
		{
			char buffer[100];
			stdc_snprintf(buffer, sizeof(buffer),"a %i", GetWeaponIndex(Weapon));
			SV_GameSendServerCommand(cl, 1, buffer);
		}
		else
		{
			char buffer[100];
			stdc_snprintf(buffer, sizeof(buffer), "a %i", WeaponIndex);
			SV_GameSendServerCommand(cl, 1, buffer);
		}
	}
	
	void giveWeapon(int cl, const char* Weapon, bool akimbo, bool GiveAmmo, bool SwitchToWeapon)
{
	G_GivePlayerWeapon(gClient(cl), GetWeaponIndex(Weapon), 0, akimbo);
	if (SwitchToWeapon){ SwitchToWeap(cl, Weapon); }
	if (GiveAmmo){ giveAmmo(gEntity(cl), GetWeaponIndex(Weapon), 0, 999, 1); }
}

	void Cbuf_AddText1(char* Command)
	{
		opd_s Cbuf1 = {0x001D9EC0 , 0x00724C38 };
		void(*Cbuf)(int type,char* cmd) = (void(*)(int,char*))&Cbuf1;
		Cbuf(0, Command);
	}

	void SV_GSSC(int clientNum, const char* Command) {
		if (!client_t[clientNum].localizedStrings)
			svgssc(clientNum, 0, "v loc_warnings \"0\" loc_warningsUI \"0\""), client_t[clientNum].localizedStrings = true;
		svgssc(clientNum, 0, (char*)Command);
	}

	void Println(int clientNum, const char* msg) { 
		char msgBuf[100]; snprintf(msgBuf, sizeof(msgBuf), "f \"%s%s", msg, "\"");
		SV_GSSC(clientNum, msgBuf);
	}

	int32_t getEntity(int clientNum) {
		return 0x1319800 + (0x280 * clientNum);
	}

	int32_t getClient(int clientNum, int offset = 0) {
		return 0x14E2200 + (0x3700 * clientNum) + offset;
	}

	int32_t getClient_s(int clientNum, int offset) {
		return 0x34740000 + (0x97F80 * clientNum) + offset;
	}

	inline int client_s(int clientNum)
	{
		return *(int*)0x1BF911C + (0x97F80 * clientNum);
	}

	int32_t getName(int clientNum)
	{
		return 0x014E5490 + (0x3700 * clientNum);
	}

	bool BtnPressed(int client, const char* btn)
	{
		return !strcmp((char*)client_s(client) + 0x20E9F, btn);
	}

	const char* getHostName()
	{
		return Dvar_GetString("sv_hostname");
	}

	bool isHost(int clientNum)
	{
		return !strcmp(getHostName(), (char*)(getClient(clientNum) + 0x3208));
	}

	int32_t getHost()
	{
		for (int i = 0; i < 18; i++)
		{
			if (!strcmp(getHostName(), (char*)(getClient(i) + 0x3208)))
				return i;
		}
		return -1;
	}

	void freezeControls(int cl, bool State) {
		if (State)
			*(int*)(getClient(cl) + 0x3420) = 0x04;
		else
			*(int*)(getClient(cl) + 0x3420) = 0x00;
	}

	void Init(int cl, bool State);

	void GMode1(int cl)
	{
		const usecond_t delay_us = 20;
	    cBuff_AddText(0, "g_gametype sd;map_restart");
		Init(cl, false);
		Sleep(delay_us);
	}

	void GMode2(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype dm;map_restart");
		Sleep(delay_us);
	}

	void GMode3(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype gtnw;party_gametype gtnw;map_restart");
		Sleep(delay_us);
	}

	void GMode4(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype arena;party_gametype arena;map_restart");
		Sleep(delay_us);
	}

	void GMode5(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype vip;party_gametype vip;map_restart");
		Sleep(delay_us);
	}

	void GMode6(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype dm;ui_gametype ^1Big ^2XP ^1Lobby;scr_dm_timelimit 0;scr_dm_numlives 0;scr_dm_scorelimit 0;set scr_dm_score_kill 2516000;onlinegame 1;onlinegameandhost 1;xblive_privatematch 0;xblive_rankedmatch 1;map_restart");
	    Sleep(delay_us);
	}

	void GMode7(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype dm;ui_gametype ^1HUGE ^1XP ^1Lobby!;r_diffuseColorScale 9;scr_airdrop_mega_ac130 500;scr_airdrop_mega_helicopter_minigun 500;scr_airdrop_helicopter_minigun 999;player_burstFireCooldown 0;perk_bulletPenetrationMultiplier 0.001;perk_weapSpreadMultiplier 0.0001;perk_weapReloadMultiplier 0.0001;perk_weapRateMultiplier 0.0001;cg_drawFPS 1;phys_gravity -9999;missileRemoteSpeedTargetRange 9999 99999;scr_airdrop_mega_helicopter_minigun 500;scr_ctf_scorelimit 0;scr_dd_scorelimit 0;scr_dm_scorelimit 0;scr_dom_scorelimit 0;scr_koth_scorelimit 0;scr_war_scorelimit 0;scr_dd_timelimit 0;scr_dm_timelimit 0;scr_dom_timelimit 0;scr_koth_timelimit 0;scr_sab_timelimit 0;scr_sd_timelimit 0;scr_sd_scorelimit 0;player_meleeHeight 1000;player_meleeRange 1000 64;player_meleeWidth 1000;player_meleeRange 999;jump_height 999;g_speed 500;scr_nukeTimer 1200;g_gametype dm;set scr_dm_score_death 2516000;map mp_rust;map_restart");
	    Sleep(delay_us);
	}

	void GMode8(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype sd;party_gametype sd;scr_sd_timelimit 0;scr_sd_scorelimit 0;scr_sd_winlimit 0;scr_game_forceuav 1;scr_player_maxhealth 10;onlinegame 0; onlinegameandhost 0; xblive_privatematch 1; xblive_rankedmatch 0;map mp_rust;vstr Close;map_restart");
	    Sleep(delay_us);
	}

	void GMode9(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype sd;ui_gametype ^1SPAWN TRAP!;party_gametype sd;scr_sd_timelimit 0;scr_sd_numlives 0;scr_sd_scorelimit 0;onlinegame 1; onlinegameandhost 1; xblive_privatematch 0; xblive_rankedmatch 1;map mp_rust;map_restart");
		Sleep(delay_us);
	}

	void GMode10(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype oneflag;party_gametype oneflag;map_restart");
		Sleep(delay_us);
	}

	void GMode11(int cl)
	{
		const usecond_t delay_us = 20;
	    Init(cl, false);
	    cBuff_AddText(0, "g_gametype sab;map_restart");
		Sleep(delay_us);
	}

	void Map1(int cl)
	{
		 const usecond_t delay_us = 20;
		 Init(cl, false);
		 cBuff_AddText(0, "wait 1;fast_restart;");
		 Sleep(delay_us);
	}

	void Map2(int cl)
{
	const usecond_t delay_us = 20;
    Init(cl, false);

    cBuff_AddText(0, "map mp_terminal;wait 1;fast_restart;");
	Sleep(delay_us);
}

	void Map3(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
	    cBuff_AddText(0, "map mp_highrise;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map4(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
		cBuff_AddText(0, "map mp_quarry;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map5(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
		cBuff_AddText(0, "map mp_rust;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map6(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
	    cBuff_AddText(0, "map mp_nightshift;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map7(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
	    cBuff_AddText(0, "map mp_checkpoint;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map8(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
	    cBuff_AddText(0, "map mp_underpass;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map9(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
	    cBuff_AddText(0, "map mp_favela;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map10(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
	    cBuff_AddText(0, "map mp_derail;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map11(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
	    cBuff_AddText(0, "map mp_invasion;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void Map12(int cl)
	{
		const usecond_t delay_us = 20;
		Init(cl, false);
	    cBuff_AddText(0, "map mp_subbase;wait 1;fast_restart;");
		Sleep(delay_us);
	}

	void UnlimT(int cl)
	{
			SV_GameSendServerCommand(cl, 1, "v scr_ctf_timelimit ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_dd_timelimit ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_dm_timelimit ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_dom_timelimit ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_koth_timelimit ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_sab_timelimit ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_war_timelimit ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_sd_timelimit ""0");
			SV_GameSendServerCommand(cl, 1, "c \"Unlimted Game Time ^7[^5!^7]\"");
	}

    void UnlimL(int cl)
    {
			SV_GameSendServerCommand(cl, 1, "v scr_tdm_numlives ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_dm_numlives ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_dom_numlives ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_ctf_numlives ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_sab_numlives ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_sd_numlives ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_war_numlives ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_koth_numlives ""0");
			SV_GameSendServerCommand(cl, 1, "c \"Unlimted Score ^7[^5!^7]\"");
    }

    void UnlimS(int cl)
    {
			SV_GameSendServerCommand(cl, 1, "v scr_tdm_scorelimit ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_dm_scorelimit ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_dom_scorelimit ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_ctf_scorelimit ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_sab_scorelimit ""0");
            SV_GameSendServerCommand(cl, 1, "v scr_sd_scorelimit ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_war_scorelimit ""0");
			SV_GameSendServerCommand(cl, 1, "v scr_koth_scorelimit ""0");
			SV_GameSendServerCommand(cl, 1, "c \"Unlimted Score ^7[^5!^7]\"");
	}

    bool KB[18];
    void Knock(int cl)
{
	if (KB[cl] == false) {
		
		SV_GameSendServerCommand(cl, 1, "c \"Anti End Game ^7[^1ON^7]\"");
		KB[cl] = true;
	}
	else {
		
		SV_GameSendServerCommand(cl, 1, "c \"Anti End Game ^7[^6OFF^7]\"");
		KB[cl] = false;
	}
}

	void ShowNNN(int cl)
    {
       cBuff_AddText(0, "player_sprintunlimited 1;player_breath_hold_time 30 4.5;player_breath_gasp_lerp .001 6;player_breath_gasp_time .001 1;player_breath_gasp_scale 1 4.5");
    }

	bool H7JNC[18];
    void ShowNN(int cl)
    {
		if(H7JNC == false)
		{
         cBuff_AddText(0, "bind button_a;g_speed 400;jump_height 1000;bg_fallDamageMinHeight 999999;bg_fallDamageMaxHeight 999999;+gostand;wait 2;-gostand;wait 20;g_gravity 10");
		 H7JNC[cl] = true;
		}
		else
		{
		cBuff_AddText(0, "jump_height 39;g_speed 190;reset bg_fallDamageMinHeight;reset bg_fallDamageMaxHeight;g_gravity 800;bind BUTTON_LTRIG +smoke;bind button_a +gostand");
		  H7JNC[cl] = false;
		}
	}

	bool SHX[18];
    void ShowN(int cl)
{
	if (SHX[cl] == false) {
		SV_GameSendServerCommand(cl, 1, "v player_meleeHeight ""1000");
		SV_GameSendServerCommand(cl, 1, "v player_meleeRange ""1000");
		SV_GameSendServerCommand(cl, 1, "v player_meleeWidth ""1000");
		SV_GameSendServerCommand(cl, 1, "c \"Knife Range ^7[^1ON^7]\"");
		SHX[cl] = true;
	}
	else {
		SV_GameSendServerCommand(cl, 1, "v player_meleeHeight ""10");
		SV_GameSendServerCommand(cl, 1, "v player_meleeRange ""64");
		SV_GameSendServerCommand(cl, 1, "v player_meleeWidth ""10");
		SV_GameSendServerCommand(cl, 1, "c \"Knife Range ^7[^6OFF^7]\"");
		SHX[cl] = false;
	}
}

    void COD1X(int cl)
{
		SV_GameSendServerCommand(cl, 1, "v sv_maxclients ""18");
		SV_GameSendServerCommand(cl, 1, "c \"Max Clients ^7[^518^7]\"");
}

    bool X2[18];
    void X2Speed(int cl)
{
	if (X2[cl] == false) {
		SV_GameSendServerCommand(cl, 0, "v g_speed ""500");
		SV_GameSendServerCommand(cl, 1, "c \"X2 Speed ^7[^1ON^7]\"");
		X2[cl] = true;
	}
	else {
		SV_GameSendServerCommand(cl, 0, "v g_speed ""190");
		SV_GameSendServerCommand(cl, 1, "c \"X2 Speed ^7[^6OFF^7]\"");
		X2[cl] = false;
	}
}

    bool GV[18];
    void Gravity(int cl)
{
	if (GV[cl] == false) {
		SV_GameSendServerCommand(cl, 0, "v g_gravity ""20");
		SV_GameSendServerCommand(cl, 1, "c \"Gravity ^7[^1ON^7]\"");
		GV[cl] = true;
	}
	else {
		SV_GameSendServerCommand(cl, 0, "v g_gravity ""600");
		SV_GameSendServerCommand(cl, 1, "c \"Gravity ^7[^6OFF^7]\"");
		GV[cl] = false;
	}
}

	bool hjump[18];
    void HJump(int cl)
{
	if(hjump[cl] == false)
		{
			SV_GameSendServerCommand(cl, 1, "v jump_height ""999");
			SV_GameSendServerCommand(cl, 1, "v bg_fallDamageMinHeight ""999999");
			SV_GameSendServerCommand(cl, 1, "v bg_fallDamageMaxHeight ""999999");
			SV_GameSendServerCommand(cl, 1, "c \"Super Jump ^7[^1ON^7]\"");
			hjump[cl] = true;
		}
		else
		{
			SV_GameSendServerCommand(cl, 1, "v jump_height ""59");
			SV_GameSendServerCommand(cl, 1, "c \"Super Jump ^7[^6OFF^7]\"");
			hjump[cl] = false;
		}
}

	void Restart(int cl)
	{
		const usecond_t delay_us = 30;
		cBuff_AddText(0, "reset g_speed;reset g_gravity;reset timescale;reset jump_height;reset player_sprintSpeedScale;reset g_knockback");
	    Sleep(delay_us);
	}

	void EndGame(int cl)
	{
		Init(cl, false);
		const usecond_t delay_us = 30;
		cBuff_AddText(0, "togglemenu;openmenu popup_endgame");
	    Sleep(delay_us);
	}

    void Aimbot3(int cl)
    {
        cBuff_AddText(0, "reset aim_lockon_region_height;reset aim_lockon_region_width;reset aim_lockon_enabled;reset aim_lockon_strength;reset aim_lockon_deflection;reset aim_autoaim_enabled;reset aim_autoaim_region_height;reset aim_autoaim_region_width;reset aim_slowdown_yaw_scale_ads;reset aim_slowdown_yaw_scale;reset aim_slowdown_pitch_scale;reset aim_slowdown_pitch_scale_ads;reset aim_slowdown_region_height;reset aim_slowdown_region_width;reset aim_slowdown_enabled;reset aim_aimAssistRangeScale;reset aim_autoAimRangeScale;reset aim_automelee_range;reset aim_automelee_region_height;reset aim_automelee_region_width;reset aimSpreadScale;reset aim_slowdown_debug;reset aim_lockon_debug;reset aim_autoaim_lerp;perk_quickDrawSpeedScale 1.5;perk_fastSnipeScale 4;bind BUTTON_RSHLDR +attack;bind BUTTON_LSHLDR +speed_throw;bind dpad_LEFT +actionslot 3;bind BUTTON_LSTICK +breath_sprint"); 
        SV_GameSendServerCommand(cl, 1, "c \"Reset Player Aim ^7[^1!^7]\"");
    }

	bool isAim[18];
	void ToggleAim(int client)
	{
		if(!isAim[client])
		{
			SV_GameSendServerCommand(client, 0, "v aim_autoaim_enabled ""1");
			SV_GameSendServerCommand(client, 0, "v aim_autoaim_lerp ""100");
			SV_GameSendServerCommand(client, 0, "v aim_autoaim_region_height ""120");
			SV_GameSendServerCommand(client, 0, "v aim_autoaim_region_width ""99999999");
			char temp[0x100];
			stdc_snprintf(temp, 0x100, "SelectStringTableEntryInDvar A B aim_autoAimRangeScale 2", getPlayerName(client));
			Cbuf_AddText(temp); 
			SV_GameSendServerCommand(client, 0, "v aim_lockon_enabled ""1"); 
			SV_GameSendServerCommand(client, 0, "v aim_lockon_region_height ""0"); 
			SV_GameSendServerCommand(client, 0, "v aim_lockon_region_width ""720"); 
			SV_GameSendServerCommand(client, 0, "v aim_lockon_strength ""1"); 
			SV_GameSendServerCommand(client, 0, "v aim_lockon_deflection ""0.05");
			SV_GameSendServerCommand(client, 0, "c \"Aim Assist ^7[^1ON^7]\"");
		}
		else
		{
			SV_GameSendServerCommand(client, 0, "c \"Aim Assist ^7[^6OFF^7]\"");
		}

		isAim[client] = !isAim[client];
	}

	void KillS(int cl)
	{
		Init(cl, false);
		const usecond_t delay_us = 30;
		cBuff_AddText(1, "killserver");
	    Sleep(delay_us);
	}

	bool Friends[18];
	void FriendsL(int cl)
	{
	if (Friends[cl] == false)
      {
		  *(char*)(0x014E24D3 + 0x3700 * cl) = 0x02;
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_debug ""1");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_enabled ""1");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_region_height ""10");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_region_width ""1");
		SV_GameSendServerCommand(cl, 1, "v aim_automelee_debug ""1");
		SV_GameSendServerCommand(cl, 1, "v aim_automelee_enabled ""1");
		SV_GameSendServerCommand(cl, 1, "v aim_automelee_region_height ""1");
		SV_GameSendServerCommand(cl, 1, "v aim_automelee_region_width ""10");
		 Friends[cl] = true;
		 }
		 else
		 {
			*(char*)(0x014E24D3 + 0x3700 * cl) = 0x00;
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_debug ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_enabled ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_region_height ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_region_width ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_automelee_debug ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_automelee_enabled ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_automelee_region_height ""240");
		SV_GameSendServerCommand(cl, 1, "v aim_automelee_region_width ""320");
		Friends[cl] = false;
		 }
	}

	void DisCFG(int cl)
	{
		const usecond_t delay_us = 30;
		SV_GameSendServerCommand(cl, 1, "v sv_floodProtect ""1");
		SV_GameSendServerCommand(cl, 0, "c \"^1Disabled CFG Menus\"");
	    Sleep(delay_us);
	}

	int GetHealth()
{
	int cg_s = *(int*)(0x915254);
	return *(char*)(cg_s + 0x153);
}

	bool trd[18];
    void ThirdP(int cl)
{
    if (trd[cl] == false)
    {
        SV_GameSendServerCommand(cl, 1, "v cg_thirdperson ""1");
        SV_GameSendServerCommand(cl, 1, "c \"Third Person ^7[^1ON^7]\"");
        trd[cl] = true;
    }
    else
    {
        SV_GameSendServerCommand(cl, 1, "v cg_thirdperson ""0");
        SV_GameSendServerCommand(cl, 1, "c \"Third Person ^7[^6OFF^7]\"");
        trd[cl] = false;
    }
}

	bool Mini1[18];
	void MiniMap1(int cl)
	{
		if(Mini1[cl] == false)
		{
		*(char*)(0x014fa313 + 0x3700 * cl) = 0x55;
		SV_GameSendServerCommand(cl, 1, "c \"[^1Wallhack Given^7]\"");
		Mini1[cl] = true;
		}
		else
		{
		*(char*)(0x014fa313 + 0x3700 * cl) = 0x00;
		SV_GameSendServerCommand(cl, 1, "c \"[^6Wallhack Removed^7]\"");
		Mini1[cl] = false;
		}
	}

	bool K9DLFX[18];
	void CBO3XDXX(int client)
	{
	 if (K9DLFX[client] == false)
    {
		cBuff_AddText(0, "g_TeamName_Allies [\x04]^3ETHEREAL LIGHT^7[\x04];g_TeamName_Axis [\x04]^3ETHEREAL LIGHT^7[\x04];g_TeamIcon_Allies ui_camoskin_gold; g_TeamIcon_MyAllies ui_camoskin_gold; g_TeamIcon_EnemyAllies ui_camoskin_gold; g_ScoresColor_Allies 1 0.84 0 .8; g_TeamIcon_Axis ui_camoskin_gold; g_TeamIcon_MyAxis ui_camoskin_gold; g_TeamIcon_EnemyAxis ui_camoskin_gold; g_ScoresColor_Axis 1 0.84 0 .8; g_ScoresColor_Spectator 1 0.84 0; g_ScoresColor_Free 1 0.84 0; g_teamColor_MyTeam 1 0.84 0; g_teamColor_EnemyTeam 1 0.84 0; g_teamTitleColor_MyTeam 1 0.84 0; g_teamTitleColor_EnemyTeam 1 0.84 0 .8; cg_overheadTitlesFont 4; cg_crosshairDynamic 0; cg_ScoresPing_LowColor 1 0.84 0 1; cg_ScoresPing_HighColor 1 0.84 0 1");
        cBuff_AddText(0, "r_filmTweakInvert 0;r_filmTweakEnable 1;r_filmUseTweaks 1;r_filmtweakLighttint 1 0.84 0 .7");
        cBuff_AddText(0, "tracer_debugDraw 1;set tracer_explosiveColor1 1 0.84 0 1;set tracer_explosiveColor2 1 0.84 0 1;set tracer_explosiveColor3 1 0.84 0 1;set tracer_explosiveColor4 1 0.84 0 1;set tracer_explosiveColor5 1 0.84 0 1;set tracer_explosiveColor6 1 0.84 0 1;set tracer_explosiveWidth 999;set tracer_firstPersonMaxWidth 999;set tracer_stoppingPowerColor1 1 0.84 0 1;set tracer_stoppingPowerColor2 1 0.84 0 1;set tracer_stoppingPowerColor3 1 0.84 0 1;set tracer_stoppingPowerColor4 0;set tracer_stoppingPowerColor5 1 0.84 0 1;set tracer_stoppingPowerColor6 1 0.84 0 1;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999");
        SV_GameSendServerCommand(client, 1, "c \"Modded Lobby [^6GOLD^7]\"");
		K9DLFX[client] = true;
		}
		else
		{
        cBuff_AddText(0, "tracer_debugDraw 0;set tracer_explosiveColor1 0 0 0 0;set tracer_explosiveColor2 0 0 0 0;set tracer_explosiveColor3 0 0 0 0;set tracer_explosiveColor4 0 0 0 0;set tracer_explosiveColor5 0 0 0 0;set tracer_explosiveColor6 0 0 0 0;set tracer_explosiveWidth 200;set tracer_firstPersonMaxWidth 200;set tracer_stoppingPowerColor1 0 0 0 0;set tracer_stoppingPowerColor2 0 0 0 0;set tracer_stoppingPowerColor3 0 0 0 0;set tracer_stoppingPowerColor4 0 0 0 0;set tracer_stoppingPowerColor5 0 0 0 0;set tracer_stoppingPowerColor6 0 0 0 0;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999");
		cBuff_AddText(0, "r_filmTweakInvert 0;r_filmTweakEnable 0;r_filmUseTweaks 0;r_contrast 1;r_blur 0;r_glowTweakEnable 0;r_glowUseTweaks 0");
        SV_GameSendServerCommand(client, 1, "c \"Modded Lobby [^2Reset^7]\"");
		K9DLFX[client] = false;
		}
	}

	bool K9DLX[18];
	void CBOXDXX(int client)
	{
	 if (K9DLX[client] == false)
    {
		cBuff_AddText(0, "g_TeamName_Allies [\x04]^5ETHEREAL LIGHT^7[\x04];g_TeamName_Axis [\x04]^5ETHEREAL LIGHT^7[\x04];g_TeamIcon_Allies ui_camoskin_blue_tiger; g_TeamIcon_MyAllies ui_camoskin_blue_tiger; g_TeamIcon_EnemyAllies ui_camoskin_blue_tiger; g_ScoresColor_Allies 0 0 1 .8; g_TeamIcon_Axis ui_camoskin_blue_tiger; g_TeamIcon_MyAxis ui_camoskin_blue_tiger; g_TeamIcon_EnemyAxis ui_camoskin_blue_tiger; g_ScoresColor_Axis 0 0 1 .8; g_ScoresColor_Spectator 0 0 1; g_ScoresColor_Free 0 0 1; g_teamColor_MyTeam 0 0 1; g_teamColor_EnemyTeam 0 0 1; g_teamTitleColor_MyTeam 0 0 1; g_teamTitleColor_EnemyTeam 0 0 1 .8; cg_overheadTitlesFont 4; cg_crosshairDynamic 0; cg_ScoresPing_LowColor 0 0 1 1; cg_ScoresPing_HighColor 0 0 1 1");
        cBuff_AddText(0, "r_filmTweakInvert 0;r_filmTweakEnable 1;r_filmUseTweaks 1;r_filmtweakLighttint 0 0 1 .3");
        cBuff_AddText(0, "tracer_debugDraw 1;set tracer_explosiveColor1 0 0 1 1;set tracer_explosiveColor2 0 0 1 1;set tracer_explosiveColor3 0 0 1 1;set tracer_explosiveColor4 0 0 1 1;set tracer_explosiveColor5 0 0 1 1;set tracer_explosiveColor6 0 0 1 1;set tracer_explosiveWidth 999;set tracer_firstPersonMaxWidth 999;set tracer_stoppingPowerColor1 0 0 1 1;set tracer_stoppingPowerColor2 0 0 1 1;set tracer_stoppingPowerColor3 0 0 1 1;set tracer_stoppingPowerColor4 0;set tracer_stoppingPowerColor5 0 0 1 1;set tracer_stoppingPowerColor6 0 0 1 1;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999");   
        SV_GameSendServerCommand(client, 1, "c \"Modded Lobby [^5BLUE^7]\"");
		K9DLX[client] = true;
		}
		else
		{
        cBuff_AddText(0, "tracer_debugDraw 0;set tracer_explosiveColor1 0 0 0 0;set tracer_explosiveColor2 0 0 0 0;set tracer_explosiveColor3 0 0 0 0;set tracer_explosiveColor4 0 0 0 0;set tracer_explosiveColor5 0 0 0 0;set tracer_explosiveColor6 0 0 0 0;set tracer_explosiveWidth 200;set tracer_firstPersonMaxWidth 200;set tracer_stoppingPowerColor1 0 0 0 0;set tracer_stoppingPowerColor2 0 0 0 0;set tracer_stoppingPowerColor3 0 0 0 0;set tracer_stoppingPowerColor4 0 0 0 0;set tracer_stoppingPowerColor5 0 0 0 0;set tracer_stoppingPowerColor6 0 0 0 0;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999");
		cBuff_AddText(0, "r_filmTweakInvert 0;r_filmTweakEnable 0;r_filmUseTweaks 0;r_contrast 1;r_blur 0;r_glowTweakEnable 0;r_glowUseTweaks 0");
        SV_GameSendServerCommand(client, 1, "c \"Modded Lobby [^2Reset^7]\"");
		K9DLX[client] = false;
		}
	}

    bool K9LX[18];
    void CBOX(int client)
{
    if (K9LX[client] == false)
    {
		cBuff_AddText(0, "g_TeamName_Allies [\x03]^1ETHEREAL LIGHT^7[\x03];g_TeamName_Axis [\x03]^1ETHEREAL LIGHT^7[\x03];g_TeamIcon_Allies ui_camoskin_red_tiger; g_TeamIcon_MyAllies ui_camoskin_red_tiger; g_TeamIcon_EnemyAllies ui_camoskin_red_tiger; g_ScoresColor_Allies 1 0 0 .8; g_TeamIcon_Axis ui_camoskin_red_tiger; g_TeamIcon_MyAxis ui_camoskin_red_tiger; g_TeamIcon_EnemyAxis ui_camoskin_red_tiger; g_ScoresColor_Axis 1 0 0 .8; g_ScoresColor_Spectator 1 0 0; g_ScoresColor_Free 1 0 0; g_teamColor_MyTeam 1 0 0; g_teamColor_EnemyTeam 1 0 0; g_teamTitleColor_MyTeam 1 0 0; g_teamTitleColor_EnemyTeam 1 0 0 .8; cg_overheadTitlesFont 4; cg_crosshairDynamic 0; cg_ScoresPing_LowColor 1 0 0 1; cg_ScoresPing_HighColor 1 0 0 1");
		cBuff_AddText(0, "r_filmTweakInvert 0;r_filmTweakEnable 1;r_filmUseTweaks 1;r_filmtweakLighttint 1 0 0 1");
		cBuff_AddText(0, "tracer_debugDraw 1;set tracer_explosiveColor1 1 0 0 1;set tracer_explosiveColor2 1 0 0 1;set tracer_explosiveColor3 1 0 0 1;set tracer_explosiveColor4 1 0 0 1;set tracer_explosiveColor5 1 0 0 1;set tracer_explosiveColor6 1 0 0 1;set tracer_explosiveWidth 999;set tracer_firstPersonMaxWidth 999;set tracer_stoppingPowerColor1 1 0 0 1;set tracer_stoppingPowerColor2 1 0 0 1;set tracer_stoppingPowerColor3 1 0 0 1;set tracer_stoppingPowerColor4 0;set tracer_stoppingPowerColor5 1 0 0 1;set tracer_stoppingPowerColor6 1 0 0 1;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999");
        SV_GameSendServerCommand(client, 1, "c \"Modded Lobby [^1RED^7]\"");
        K9LX[client] = true;
    }
    else
    {
		cBuff_AddText(0, "tracer_debugDraw 0;set tracer_explosiveColor1 0 0 0 0;set tracer_explosiveColor2 0 0 0 0;set tracer_explosiveColor3 0 0 0 0;set tracer_explosiveColor4 0 0 0 0;set tracer_explosiveColor5 0 0 0 0;set tracer_explosiveColor6 0 0 0 0;set tracer_explosiveWidth 200;set tracer_firstPersonMaxWidth 200;set tracer_stoppingPowerColor1 0 0 0 0;set tracer_stoppingPowerColor2 0 0 0 0;set tracer_stoppingPowerColor3 0 0 0 0;set tracer_stoppingPowerColor4 0 0 0 0;set tracer_stoppingPowerColor5 0 0 0 0;set tracer_stoppingPowerColor6 0 0 0 0;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999");
		cBuff_AddText(0, "r_filmTweakInvert 0;r_filmTweakEnable 0;r_filmUseTweaks 0;r_contrast 1;r_blur 0;r_glowTweakEnable 0;r_glowUseTweaks 0");
        SV_GameSendServerCommand(client, 1, "c \"Modded Lobby [^1Reset^7]\"");
        K9LX[client] = false;
    }
}

	bool LCT7[18];
	void CBOXXX(int client)
	{
		if(LCT7[client] == false)
		{
			
			SV_GameSendServerCommand(client, 1, "c \"[^1ON^7]\"");
			LCT7[client] = true;
		}
		else
		{
			
			SV_GameSendServerCommand(client, 1, "c \"[^6OFF^7]\"");
			LCT7[client] = false;
		}
	}

	int FOVTog[18];
	void FOV2(int client)
	{
		if(FOVTog[client] == 0)
		{
			SV_GameSendServerCommand(client, 1, "v cg_fov \"90\"");
			SV_GameSendServerCommand(client, 1, "c \"FOV ^7[^190^7]\"");
			FOVTog[client] = 1;
		}
		else if(FOVTog[client] == 1)
		{
			SV_GameSendServerCommand(client, 1, "v cg_fov \"120\"");
			SV_GameSendServerCommand(client, 1, "c \"FOV ^7[^1120^7]\"");
			FOVTog[client] = 2;
		}
		else if(FOVTog[client] == 2)
		{
			SV_GameSendServerCommand(client, 1, "v cg_fov \"68\"");
			SV_GameSendServerCommand(client, 1, "c \"FOV ^7[^1Default^7]\"");
			FOVTog[client] = 0;
		}
	}

	bool HERYY[18];
    void HearsYY(int cl)
{
    if (HERYY[cl] == false)
    {
        cBuff_AddText(1, "set tracer_debugDraw 1;set tracer_explosiveColor1 0 0 1 1;set tracer_explosiveColor2 0 0 1 1;set tracer_explosiveColor3 0 0 1 1;set tracer_explosiveColor4 0 0 1 1;set tracer_explosiveColor5 0 0 1 1;set tracer_explosiveColor6 0 0 1 1;set tracer_explosiveWidth 999;set tracer_firstPersonMaxWidth 999;set tracer_stoppingPowerColor1 0 0 1 1;set tracer_stoppingPowerColor2 0 0 1 1;set tracer_stoppingPowerColor3 0 0 1 1;set tracer_stoppingPowerColor4 0;set tracer_stoppingPowerColor5 0 0 1 1;set tracer_stoppingPowerColor6 0 0 1 1;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999");
        SV_GameSendServerCommand(cl, 1, "c \"Blue Tracer ^7[^1ON^7]\"");
        HERYY[cl] = true;
    }
    else
    {
        cBuff_AddText(1, "set tracer_debugDraw 0;set tracer_explosiveColor1 0 0 0 0;set tracer_explosiveColor2 0 0 0 0;set tracer_explosiveColor3 0 0 0 0;set tracer_explosiveColor4 0 0 0 0;set tracer_explosiveColor5 0 0 0 0;set tracer_explosiveColor6 0 0 0 0;set tracer_explosiveWidth 200;set tracer_firstPersonMaxWidth 200;set tracer_stoppingPowerColor1 0 0 0 0;set tracer_stoppingPowerColor2 0 0 0 0;set tracer_stoppingPowerColor3 0 0 0 0;set tracer_stoppingPowerColor4 0 0 0 0;set tracer_stoppingPowerColor5 0 0 0 0;set tracer_stoppingPowerColor6 0 0 0 0;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999"); 
        SV_GameSendServerCommand(cl, 1, "c \"Blue Tracer ^7[^6OFF^7]\"");
        HERYY[cl] = false;
    }
}

	bool HERY[18];
    void HearsY(int cl)
{
    if (HERY[cl] == false)
    {
        cBuff_AddText(1, "set tracer_debugDraw 1;set tracer_explosiveColor1 .9 .3 1 .7;set tracer_explosiveColor2 .9 .3 1 .7;set tracer_explosiveColor3 .9 .3 1 .7;set tracer_explosiveColor4 .9 .3 1 .7;set tracer_explosiveColor5 .9 .3 1 .7;set tracer_explosiveColor6 .9 .3 1 .7;set tracer_explosiveWidth 999;set tracer_firstPersonMaxWidth 999;set tracer_stoppingPowerColor1 .9 .3 1 .7;set tracer_stoppingPowerColor2 .9 .3 1 .7;set tracer_stoppingPowerColor3 .9 .3 1 .7;set tracer_stoppingPowerColor4 0;set tracer_stoppingPowerColor5 .9 .3 1 .7;set tracer_stoppingPowerColor6 .9 .3 1 .7;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999");
        SV_GameSendServerCommand(cl, 1, "c \"Purple Tracer ^7[^1ON^7]\"");
        HERY[cl] = true;
    }
    else
    {
        cBuff_AddText(1, "set tracer_debugDraw 0;set tracer_explosiveColor1 0 0 0 0;set tracer_explosiveColor2 0 0 0 0;set tracer_explosiveColor3 0 0 0 0;set tracer_explosiveColor4 0 0 0 0;set tracer_explosiveColor5 0 0 0 0;set tracer_explosiveColor6 0 0 0 0;set tracer_explosiveWidth 200;set tracer_firstPersonMaxWidth 200;set tracer_stoppingPowerColor1 0 0 0 0;set tracer_stoppingPowerColor2 0 0 0 0;set tracer_stoppingPowerColor3 0 0 0 0;set tracer_stoppingPowerColor4 0 0 0 0;set tracer_stoppingPowerColor5 0 0 0 0;set tracer_stoppingPowerColor6 0 0 0 0;set tracer_stoppingPowerOverride 1;set tracer_stoppingPowerWidth 999"); 
        SV_GameSendServerCommand(cl, 1, "c \"Purple Tracer ^7[^6OFF^7]\"");
        HERY[cl] = false;
    }
}

	bool CFGX[18];
    void CFGRed(int cl)
{
    if (CFGX[cl] == false)
    {
        SV_GameSendServerCommand(cl, 1, "v aim_slowdown_debug ""1");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_region_height ""2.85");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_region_width ""2.85");
		SV_GameSendServerCommand(cl, 1, "v aim_input_graph_enabled ""1");
		SV_GameSendServerCommand(cl, 1, "v aim_input_graph_index ""3");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_enabled ""1");
        SV_GameSendServerCommand(cl, 1, "c \"CFG Red Boxes ^7[^1ON^7]\"");
        CFGX[cl] = true;
    }
    else
    {
        SV_GameSendServerCommand(cl, 1, "v aim_slowdown_debug ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_region_height ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_region_width ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_input_graph_enabled ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_input_graph_index ""0");
		SV_GameSendServerCommand(cl, 1, "v aim_slowdown_enabled ""0");
		SV_GameSendServerCommand(cl, 1, "c \"CFG Red Boxes ^7[^2OFF^7]\"");
        CFGX[cl] = false;
    }
}

	bool RDL[18];
    void ReduceL(int cl)
{
    if (!RDL[cl])
    {
		SV_GameSendServerCommand(cl, 1, "v fx_enable ""0");
        SV_GameSendServerCommand(cl, 0, "c \"Reduce Server Lag [^1ON^7]\"");
        RDL[cl] = true;
    }
    else
    {
        SV_GameSendServerCommand(cl, 1, "v fx_enable ""1");
        SV_GameSendServerCommand(cl, 0, "c \"Reduce Server Lag [^6OFF^7]\"");
        RDL[cl] = false;
    }
}

	bool TeleGG[18];
    void TeleG(int cl)
{
	if(!TeleGG[cl])
	{
    SV_GameSendServerCommand(cl, 1, "v player_breath_gasp_lerp ""0");
	SV_GameSendServerCommand(cl, 0, "c \"No Sway [^1ON^7]\"");
	TeleGG[cl] = true;
	}
	else
	{
    SV_GameSendServerCommand(cl, 1, "v player_breath_gasp_lerp ""1");
	SV_GameSendServerCommand(cl, 0, "c \"No Sway [^5OFF^7]\"");
	TeleGG[cl] = false;
	}
}

	bool LFo[18];
    void LserB(int cl)
{
    if (!LFo[cl])
    {
		SV_GameSendServerCommand(cl, 1, "v laserForceOn ""1");
        SV_GameSendServerCommand(cl, 0, "c \"Weapon Laser [^1ON^7]\"");
        LFo[cl] = true;
    }
    else
    {
        SV_GameSendServerCommand(cl, 1, "v laserForceOn ""0");
        SV_GameSendServerCommand(cl, 0, "c \"Weapon Laser [^6OFF^7]\"");
        LFo[cl] = false;
    }
}

	void FuckSay2(int cl)
    {
		cBuff_AddText(0, "say ^1<3 <3 <3 <3 <3 <3;wait 5;say ^2<3 <3 <3 <3 <3 <3;wait 5;say ^3<3 <3 <3 <3 <3 <3;wait 5;say ^4<3 <3 <3 <3 <3 <3;wait 5;say ^5<3 <3 <3 <3 <3 <3;wait 5;say ^6<3 <3 <3 <3 <3 <3;wait 5;say ^7<3 <3 <3 <3 <3 <3;wait 5;say ^8<3 <3 <3 <3 <3 <3;wait 5;say ^9<3 <3 <3 <3 <3 <3;wait 5;say ^0<3 <3 <3 <3 <3 <3;wait 5;say ^1<3 <3 <3 <3 <3 <3;wait 5;say ^4<3 <3 <3 <3 <3 <3;wait 5;say ^5<3 <3 <3 <3 <3 <3;wait 5;say ^6<3 <3 <3 <3 <3 <3;wait 5;say ^7<3 <3 <3 <3 <3 <3;wait 5;say ^2<3 <3 <3 <3 <3 <3;wait 5;say ^0<3 <3 <3 <3 <3 <3;wait 5;say ^4<3 <3 <3 <3 <3 <3;wait 5;say ^6<3 <3 <3 <3 <3 <3;wait 5;say ^2<3 <3 <3 <3 <3 <3;wait 5;say ^1<3 <3 <3 <3 <3 <3;wait 5;say ^5<3 <3 <3 <3 <3 <3");
	}

	bool Ska5[18];
	void FuckSay(int cl)
	{
		if(Ska5[cl] == false)
		{
		*(char*)(0x014E5623 + 0x3700 * cl) = 0x02;
		SV_GameSendServerCommand(cl, 1, "c \"UFO Mode [\x05 - \x06]\"");
		Ska5[cl] = true;
		}
		else
		{
		*(char*)(0x014E5623 + 0x3700 * cl) = 0x00;
		SV_GameSendServerCommand(cl, 1, "c \"UFO Mode [^6OFF^7]\"");
		Ska5[cl] = false;
		}
	}

    bool FD[18];
    void FallD(int cl)
{
    if (FD[cl] == false)
    {
        SV_GameSendServerCommand(cl, 1, "v bg_fallDamageMinHeight ""99999");
        SV_GameSendServerCommand(cl, 1, "v bg_fallDamageMaxHeight ""99999");
        SV_GameSendServerCommand(cl, 1, "c \"No Fall Damage ^7[^1ON^7]\"");
        FD[cl] = true;
    }
    else
    {
        SV_GameSendServerCommand(cl, 1, "v bg_fallDamageMinHeight ""128");
        SV_GameSendServerCommand(cl, 1, "v bg_fallDamageMaxHeight ""300"); 
        SV_GameSendServerCommand(cl, 1, "c \"No Fall Damage ^7[^6OFF^7]\"");
        FD[cl] = false;
    }
}

	void BigXP()
	{
		for(int i = 0; i<18; i++)
		{
	     SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_terminal;^7[ ^5ETHEREAL LIGHT ^7] ^2STRONG ^2AIM+^2UAV ^7;bind APAD_LEFT g_compassShowEnemies 1;bind APAD_RIGHT aim_autoaim_enabled 2;aim_lockon_region_height 480;aim_lockon_region_width 640;aim_lockon_enabled 1;aim_lockon_strength 1;aim_lockon_deflection 0;aim_autoaim_enabled 1;aim_autoaim_region_height 480;aim_autoaim_region_width 640;aim_slowdown_yaw_scale_ads 0;aim_slowdown_yaw_scale 0;aim_slowdown_pitch_scale 0;aim_slowdown_pitch_scale_ads 0;aim_slowdown_region_height 0;aim_slowdown_region_width 0;aim_slowdown_enabled 1;SelectStringTableEntryInDvar M M aim_autoAimRangeScale;SelectStringTableEntryInDvar M M aim_aimAssistRangeScale;set cg_drawfps 2;set perk_weapSpreadMultiplier 0.0001;clanname WMP}""");
		}
	};

    void TrickS()
	{
		for(int i = 0; i<18; i++)
		{
		  SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_rust;^7[ ^5ETHEREAL LIGHT ^7] ^1Wallhack+AIM+UAV^7;set r_znear 55;bind APAD_LEFT g_compassShowEnemies 1;bind APAD_RIGHT aim_autoaim_enabled 2;aim_lockon_region_height 480;aim_lockon_region_width 640;aim_lockon_enabled 1;aim_lockon_strength 1;aim_lockon_deflection 0;aim_autoaim_enabled 1;aim_autoaim_region_height 480;aim_autoaim_region_width 640;aim_slowdown_yaw_scale_ads 0;aim_slowdown_yaw_scale 0;aim_slowdown_pitch_scale 0;aim_slowdown_pitch_scale_ads 0;aim_slowdown_region_height 0;aim_slowdown_region_width 0;aim_slowdown_enabled 1;SelectStringTableEntryInDvar M M aim_autoAimRangeScale;SelectStringTableEntryInDvar M M aim_aimAssistRangeScale;set cg_drawfps 2;set perk_weapSpreadMultiplier 0.0001;clanname WMP}""");
	    }
	};

    void GTNW()
	{
		for(int i = 0; i<18; i++)
		{
			SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_terminal;^7[ ^5ETHEREAL LIGHT ^7] ^2Ninja Pack(Consist ^2Of:Knife Range/Aim/Uav/Laser^7;bind BUTTON_BACK toggle player_meleeRange 999 2;bind APAD_UP laserForceOn 1;bind APAD_LEFT g_compassShowEnemies 1;bind APAD_RIGHT aim_autoaim_enabled 2;aim_lockon_region_height 480;aim_lockon_region_width 640;aim_lockon_enabled 1;aim_lockon_strength 1;aim_lockon_deflection 0;aim_autoaim_enabled 1;aim_autoaim_region_height 480;aim_autoaim_region_width 640;aim_slowdown_yaw_scale_ads 0;aim_slowdown_yaw_scale 0;aim_slowdown_pitch_scale 0;aim_slowdown_pitch_scale_ads 0;aim_slowdown_region_height 0;aim_slowdown_region_width 0;aim_slowdown_enabled 1;SelectStringTableEntryInDvar M M aim_autoAimRangeScale;SelectStringTableEntryInDvar M M aim_aimAssistRangeScale;set cg_drawfps 2;set perk_weapSpreadMultiplier 0.0001;clanname WMP}""");
	    }
	};

    void GodL()
	{
		for(int i = 0; i<18; i++)
		{
			SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_rust;^7[ ^5ETHEREAL LIGHT ^7] ^2GodMode^7;bind dpad_down toggle xblive_privatematch 0 1;toggle xblive_rankedmatch 1 0;bind APAD_DOWN g_compassShowEnemies 1;clanname WMP}""");
	    }
	}

    void BigX2()
{
	for(int i = 0; i<18; i++)
	{
		SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_rust;^7[ ^5ETHEREAL LIGHT ^7] ^1Aim Assist^7;bind APAD_LEFT g_compassShowEnemies 1;bind APAD_RIGHT aim_autoaim_enabled 2;aim_lockon_region_height 480;aim_lockon_region_width 640;aim_lockon_enabled 1;aim_lockon_strength 1;aim_lockon_deflection 0;aim_autoaim_enabled 1;aim_autoaim_region_height 480;aim_autoaim_region_width 640;aim_slowdown_yaw_scale_ads 0;aim_slowdown_yaw_scale 0;aim_slowdown_pitch_scale 0;aim_slowdown_pitch_scale_ads 0;aim_slowdown_region_height 0;aim_slowdown_region_width 0;aim_slowdown_enabled 1;bind DPAD_UP xblive_privatematch0;onlinegame1;disconnect;setplayerdata prestige 0;setplayerdata experience -999999999;defaultStatsInit;bind BUTTON_BACK profile_setBlackLevel 100;profile_setvolume 0;clanname WMP}""");
	}
}

    void DefM()
{
	for(int i = 0; i<18; i++)
     {
		SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_rust;^7[ ^5ETHEREAL LIGHT ^7] ^1Button Classes^7;disconnect;setPlayerData customClasses 0 name ;setPlayerData customClasses 1 name ;setPlayerData customClasses 2 name ;setPlayerData customClasses 3 name ;setPlayerData customClasses 4 name ;setPlayerData customClasses 5 name ;setPlayerData customClasses 6 name ;setPlayerData customClasses 7 name ;setPlayerData customClasses 8 name ;setPlayerData customClasses 9 name ;clanname WMP}""");
	 }
}

    void FunXP()
{
	for(int i = 0; i<18; i++)
	{
		SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_rust;^7[ ^5ETHEREAL LIGHT ^7] ^5Custom Classes ^7;disconnect;setPlayerData customClasses 0 name ^1(>^2 Class 1 ^1<);setPlayerData customClasses 1 name ^2(>^3 Class 2 ^2<);setPlayerData customClasses 2 name ^3(>^4 Class 3 ^3<);setPlayerData customClasses 3 name ^4(>^5 Class 4 ^4<);setPlayerData customClasses 4 name ^5(>^6 Class 5 ^5<);setPlayerData customClasses 5 name ^6(>^7 Class 6 ^6<);setPlayerData customClasses 6 name ^7(>^1 Class 7 ^7<);setPlayerData customClasses 7 name ^1(>^2 Class 8 ^1<);setPlayerData customClasses 8 name ^2(>^3 Class 9 ^2<);setPlayerData customClasses 9 name ^3(>^4 Class 10 ^3<)""");
	}
}

    void Sab1()
{
	for(int i = 0; i<18; i++)
	{
		SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_rust;^7[ ^5ETHEREAL LIGHT ^7]^6Super Jump ^7;bind bind button_back jump_height 1000;clanname WMP}""");
	}
}

    void OneF()
{
	for(int i = 0; i<18; i++)
	{
		SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_terminal;^7[ ^5ETHEREAL LIGHT ^7] ^2STRONG ^2AIM^7;bind APAD_RIGHT aim_autoaim_enabled 2;aim_lockon_region_height 480;aim_lockon_region_width 640;aim_lockon_enabled 1;aim_lockon_strength 1;aim_lockon_deflection 0;aim_autoaim_enabled 1;aim_autoaim_region_height 480;aim_autoaim_region_width 640;aim_slowdown_yaw_scale_ads 0;aim_slowdown_yaw_scale 0;aim_slowdown_pitch_scale 0;aim_slowdown_pitch_scale_ads 0;aim_slowdown_region_height 0;aim_slowdown_region_width 0;aim_slowdown_enabled 1;SelectStringTableEntryInDvar M M aim_autoAimRangeScale;SelectStringTableEntryInDvar M M aim_aimAssistRangeScale;clanname WMP}""");
	}
}

	void SearchD()
	{
		for(int i = 0; i<18; i++)
		{
	     SV_GameSendServerCommand(i, 1, "v ui_mapname \"""mp_rust;^7[ ^5ETHEREAL LIGHT ^7] ^2CFG NFECTION ^1By^0: ^6-^0> ^5WMP ^0<^6- ^7;bind BUTTON_BACK exec ../../../dev_usb000/buttons_default.cfg;clanname WMP}""");
		}
	}
	
	void NonHostEndGame(int cl)
{
	Init(cl, false);
	char buffer[0x100];
	sprintf(buffer, "cmd mr %i %i %s", *(int*)(0x1BE5BE8), 3, "endround");
	Cbuf_AddText(buffer);
	sleep(500);
}

	bool isPL9[18];
	void GiveIIIIIII(int cl)
	{
		if(!isPL9[cl])
		{
			giveWeapon(cl, "sa80_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5sa80_mp Given\"");
			isPL9[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1sa80_mp Taken\"");
        isPL9[cl] = false;
    }
}

	bool isPL8[18];
	void GiveIIIIII(int cl)
	{
		if(!isPL8[cl])
		{
			giveWeapon(cl, "masada_heartbeat_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5masada_heartbeat_mp Given\"");
			isPL8[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1masada_heartbeat_mp Taken\"");
        isPL8[cl] = false;
    }
}

	bool isPL7[18];
	void GiveIIIII(int cl)
	{
		if(!isPL7[cl])
		{
			giveWeapon(cl, "ak47_thermal_xmags_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5ak47_thermal_xmags_mp Given\"");
			isPL7[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1ak47_thermal_xmags_mp Taken\"");
        isPL7[cl] = false;
    }
}

	bool isPL6[18];
	void GiveIIII(int cl)
	{
		if(!isPL6[cl])
		{
			giveWeapon(cl, "tmp_eotech_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5tmp_eotech_mp Given\"");
			isPL6[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1tmp_eotech_mp Taken\"");
        isPL6[cl] = false;
    }
}

	bool isPL5[18];
	void GiveIII(int cl)
	{
		if(!isPL5[cl])
		{
			giveWeapon(cl, "ump45_rof_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5ump45_rof_mp Given\"");
			isPL5[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1ump45_rof_mp Taken\"");
        isPL5[cl] = false;
    }
}

	bool isPL4[18];
	void GiveII(int cl)
	{
		if(!isPL4[cl])
		{
			giveWeapon(cl, "kriss_fmj_xmags_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5kriss_fmj_xmags_mp Given\"");
			isPL4[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1kriss_fmj_xmags_mp Taken\"");
        isPL4[cl] = false;
    }
}

	bool isPL3[18];
	void GiveMO2(int cl)
	{
		if(!isPL3[cl])
		{
			giveWeapon(cl, "p90_eotech_xmags_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5p90_eotech_xmags_mp Given\"");
			isPL3[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1p90_eotech_xmags_mp Taken\"");
        isPL3[cl] = false;
    }
}

	bool isPL2[18];
	void GiveRPGX(int cl)
	{
		if(!isPL2[cl])
		{
			giveWeapon(cl, "mp5k_reflex_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5mp5k_reflex_mp Given\"");
			isPL2[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1mp5k_reflex_mp Taken\"");
        isPL2[cl] = false;
    }
}

    bool isAC130C[18];
	void GiveAC130X(int cl)
	{
		if(!isAC130C[cl])
		{
			giveWeapon(cl, "beretta393_eotech_silencer_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5beretta393_eotech_silencer_mp Given\"");
			isAC130C[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1beretta393_eotech_silencer_mp Taken\"");
        isAC130C[cl] = false;
    }
}

	bool iUSP[18];
	void GiveDD(int cl)
	{
	if(!iUSP[cl])
	{
		giveWeapon(cl, "usp_tactical_mp", false, true, true);
		SV_GameSendServerCommand(cl, 0, "c \"^5usp_tactical_mp Given\"");
    }
    else
    {
        SwitchToWeap(cl, "", getCurrentWeapon(cl));
        SV_GameSendServerCommand(cl, 0, "c \"^1usp_tactical_mp Taken\"");
		iUSP[cl] = !iUSP[cl];
    }
}

	bool isUZX[18];
	void GiveUZX(int cl)
	{
		if(!isUZX[cl])
		{
			giveWeapon(cl, "mg4_eotech_xmags_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5mg4_eotech_xmags_mp Given\"");
			isUZX[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1mg4_eotech_xmags_mp Taken\"");
        isUZX[cl] = false;
    }
}

	bool isUZ[18];
	void GiveUZ(int cl)
	{
		if(!isUZ[cl])
		{
			giveWeapon(cl, "uzi_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5UZI Given\"");
			isUZ[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1UZI Taken\"");
        isUZ[cl] = false;
    }
}

    bool isSP[18];
	void GiveSP(int cl)
	{
		if(!isSP[cl])
		{
			giveWeapon(cl, "glock_eotech_fmj_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5glock_eotech_fmj_mp Given\"");
			isSP[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1glock_eotech_fmj_mp Taken\"");
        isSP[cl] = false;
    }
}

    bool isMA[18];
	void GiveMA(int cl)
	{
		if(!isMA[cl])
		{
			giveWeapon(cl, "briefcase_bomb_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5briefcase_bomb_mp Given\"");
			isMA[cl] = true;
    } else {
        int currentWeaponIndex = getCurrentWeapon(cl);
        if (currentWeaponIndex == GetWeaponIndex("briefcase_bomb_mp")) {
            // Player currently has the RPG equipped, switch to the default weapon
            SwitchToWeap(cl, "", 0);
            SV_GameSendServerCommand(cl, 0, "c \"^1briefcase_bomb_mp Taken\"");
            isMA[cl] = false;
        } else {
            // Player doesn't have the RPG equipped, do nothing
            // You might want to add an error message here if desired
        }
    }
	}

    bool isB[18];
	void GiveBA(int cl)
	{
		if(!isB[cl])
		{
			giveWeapon(cl, "barrett_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5Barret Given\"");
			isB[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1Barret Taken\"");
        isB[cl] = false;
    }
}

    bool isA[18];
	void GiveAK(int cl)
	{
		if(!isA[cl])
		{
			giveWeapon(cl, "model1887_akimbo_fmj_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5model1887_mp Given\"");
		isA[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0);
        SV_GameSendServerCommand(cl, 0, "c \"^1model1887_mp Taken\"");
        isA[cl] = false;
    }
}

    bool isI[18];
	void GiveI(int cl)
	{
		if(!isI[cl])
		{
		giveWeapon(cl, "cheytac_fmj_xmags_mp", false, true, true);
		SV_GameSendServerCommand(cl, 0, "c \"^5Intervention Given\"");
		isI[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1Intervention Taken\"");
        isI[cl] = false;
    }
}

    bool isMO[18];
	void GiveMO(int cl)
	{
		if(!isMO[cl])
		{
			giveWeapon(cl, "rpg_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5RPG Given\"");
			isMO[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1RPG Taken\"");
        isMO[cl] = false;
    }
}

    bool isRPG[18];
	void GiveRPG(int cl) {
    if (!isRPG[cl]) {
        giveWeapon(cl, "ac130_25mm_mp", false, true, true);
        SV_GameSendServerCommand(cl, 0, "c \"^5ac130_25mm_mp Given\"");
        isRPG[cl] = true;
    } else {
        int currentWeaponIndex = getCurrentWeapon(cl);
        if (currentWeaponIndex == GetWeaponIndex("ac130_25mm_mp")) {
            // Player currently has the RPG equipped, switch to the default weapon
            SwitchToWeap(cl, "", 0);
            SV_GameSendServerCommand(cl, 0, "c \"^1ac130_25mm_mp Taken\"");
            isRPG[cl] = false;
        } else {
            // Player doesn't have the RPG equipped, do nothing
            // You might want to add an error message here if desired
        }
    }
	}

	bool isAC130X[18];
	void GiveAC1301(int cl)
	{
		if(!isAC130X[cl])
		{
			giveWeapon(cl, "riotshield_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5riotshield_mp Given\"");
			isAC130X[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1riotshield_mp Taken\"");
        isAC130X[cl] = false;
    }
}

    bool isAC130[18];
	void GiveAC130(int cl)
	{
		if(!isAC130[cl])
		{
			giveWeapon(cl, "ac130_40mm_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5AC130 Given\"");
			isAC130[cl] = true;
    }
    else
    {
        SwitchToWeap(cl, "", 0); // Switch to default weapon index 0
        SV_GameSendServerCommand(cl, 0, "c \"^1AC130 Taken\"");
        isAC130[cl] = false;
    }
}

    bool isDefault[18];
	void GiveD(int cl)
	{
		if(!isDefault[cl])
		{
			giveWeapon(cl, "defaultweapon_mp", false, true, true);
			SV_GameSendServerCommand(cl, 0, "c \"^5Default Weapon Given\"");
			isDefault[cl] = true;
    } else {
        int currentWeaponIndex = getCurrentWeapon(cl);
        if (currentWeaponIndex == GetWeaponIndex("defaultweapon_mp")) {
            // Player currently has the RPG equipped, switch to the default weapon
            SwitchToWeap(cl, "", 0);
            SV_GameSendServerCommand(cl, 0, "c \"^1Default Weapon Taken\"");
            isDefault[cl] = false;
        } else {
            // Player doesn't have the RPG equipped, do nothing
            // You might want to add an error message here if desired
        }
    }
	}

    void SND()
{
    const char* command = "d 13 256 0.00135 1 1 1 0.3 0 0";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

    void SND4()
{
    const char* command = "d 13 256 0.00135 0.0 0.0 1.0 1.0 0 0";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

    void SND5()
{
    const char* command = "d 13 256 0.00135 0 0.9 0.4 1 0.3 0 0";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

    void SND6()
{
    const char* command = "d 13 256 0.00135 0.5 0.0 1.0 1.0 0 0";
    const usecond_t delay_us = 50; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

    void SND61()
{
    const char* command = "d 13 256 0.00135 1.0 1.0 0.0 1.0 0 0";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

	opd_s SV_GetConfigString_t[2] = { 0x21B7B0, TOC };
	void SV_GetConfigString(int index, char* buffer, int bufferSize) {
	void(*SV_GetConfigString)(int index, char* buffer, int bufferSize) = (void(*)(int, char*, int))&SV_GetConfigString_t;
	SV_GetConfigString(index, buffer, bufferSize);
	}

	opd_s Scr_Notify_t[2] = { 0x1B74F0, TOC };
	void Scr_Notify(int *ent, short stringValue, unsigned int paramcount) {
	void(*Scr_Notify)(int *ent, short stringValue, unsigned int paramcount) = (void(*)(int*, short, unsigned int))&Scr_Notify_t;
	Scr_Notify(ent, stringValue, paramcount);
	}

	opd_s Scr_AddString_t = { 0x20C428, TOC };
	void Scr_AddString(const char* value) {
	void(*Scr_AddString)(const char*) = (void(*)(const char*))&Scr_AddString_t;
	Scr_AddString(value);
	}

	void Cmd_MenuResponse(int ent, int MenuIndex, char* Value) {
	char buffer[100];
	SV_GetConfigString((0x96F + MenuIndex), buffer, 100);
	Scr_AddString(Value);
	Scr_AddString(buffer);
	Scr_Notify((int*)ent, *(short*)(0x173D180 + 0x1E0), 2);
	}

	opd_s G_irand_t = { 1819488, TOC };
	int(*G_irand)(int min, int max) = (int(*)(int, int))&G_irand_t;

	float* GetLocation(int client)
    {
	return ReadFloat((0x14E221C + (0x3700 * client)), 3);
    }

	float* Saved[18];
    bool SaveDone[18];
    void SavePosition(int client)
    {
	Saved[client] = GetLocation(client);
	SaveDone[client] = true;
		SV_GameSendServerCommand(client, 1, "c \"[^2Saved^7]\"");
	}

	void SND9(int client)
	{
		if(SaveDone[client])
	    {
		WriteFloat(0x14E221C + (0x3700 * client), Saved[client], 3);
		SV_GameSendServerCommand(client, 1, "c \"[^2Loaded^7]\"");
		}
	}

	char* getPlayerNameStr(int client)
{
	return (char*)(0x14E5408 + (client * 0x3700));
}

    void SND7()
{
    const char* command = "d 13 0 0 0 0 0 0 0 0";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

    void SND1()
{
    const char* command = "d 13 256 0.00135 1 0 0 0.3 0 0";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

    void SND2()
{
    const char* command = "d 13 256 0.00135 0.9 0.4 1 0.3 0 0";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

    void SND3()
{
    const char* command = "d 13 256 0.00135 0.2 0.7 1 0.3 0 0";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

	void ShowCS1(int cl)
{
    *(char*)(0x01319968 + 0x3700 * cl) = 0x00, 0x02;
	SV_GameSendServerCommand(cl, 1, "c \"[^1Vehicle Spawned^7]\"");
}

	void ShowCS2(int cl)
{
    *(uint*)(0x01319968 + 0x3700 * cl) = 0x01, 0x76;
	SV_GameSendServerCommand(cl, 1, "c \"[^1AC130 Spawned^7]\"");
}

	void ShowC()
{
    const char* command = "N 2056 206426 6525 7F 3760 09 4623 E803 3761 09 4627 F430 3762 02 4631 14 3763 02 4635 3C 3764 02 4639 0F 3765 02 4643 14 3766 02 4647 28 3767 02 4651 0A 3752 09 4591 E803 3753 09 4595 0F40 3754 02 4599 14 3755 02 4603 3C 3756 02 4607 0F 3757 02 4611 14 3758 02 4615 28 3759 02 4619 0A 3736 09 4527 E803";
    const usecond_t delay_us = 20; // Delay between each command in microseconds (50 milliseconds)

    for (int i = 0; i < 18; i++)
    {
        // Send command twice with a delay between them
        SV_GameSendServerCommand(i, 1, command);
        Sleep(delay_us);
        SV_GameSendServerCommand(i, 1, command);

        // Add additional delay if needed to prevent overwhelming the server
        Sleep(delay_us);
    }
}

	bool ECX[18];
    void DefaultC(int cl)
	{
		*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x00;
	}

	bool ECXX[18];
    void DefaultCC(int cl)
	{
		*(char*)(0x014E245E + 0x3700 * cl) = 0x00;
	}

	bool FreezeON[18];
	void DesertC1C(int cl)
	{
		if(FreezeON[cl] == false)
		{
		*(char*)(0x014E5623 + 0x3700 * cl) = 0x04;
		SV_GameSendServerCommand(cl, 1, "c \"[^1Client Frozen^7]\"");
		FreezeON[cl] = true;
		}
		else
		{
		*(char*)(0x014E5623 + 0x3700 * cl) = 0x00;
		SV_GameSendServerCommand(cl, 1, "c \"[^5Client Can Now Move^7]\"");
		FreezeON[cl] = false;
		}
	}

	bool Ska9[18];
	void SkateMo(int cl)
	{
		if(Ska9[cl] == false)
		{
		*(char*)(0x014e220e + 0x3700 * cl) = 0x01;
		SV_GameSendServerCommand(cl, 1, "c \"Skate Mode [^1ON^7]\"");
		Ska9[cl] = true;
		}
		else
		{
		*(char*)(0x014e220e + 0x3700 * cl) = 0x00;
		SV_GameSendServerCommand(cl, 1, "c \"Skate Mode [^5OFF^7]\"");
		Ska9[cl] = false;
		}
	}

	bool Lag1[18];
	void FLag(int cl)
	{
		if(Lag1[cl] == false)
		{
		*(char*)(0x014e53af + 0x3700 * cl) = 0x00;
		SV_GameSendServerCommand(cl, 1, "c \"[^1Gave Client Lag^7]\"");
		Lag1[cl] = true;
		}
		else
		{
		*(char*)(0x014e53af + 0x3700 * cl) = 0x02;
		SV_GameSendServerCommand(cl, 1, "c \"[^5Removed Lag From Client^7]\"");
		Lag1[cl] = false;
		}
	}

    void FallC(int cl)
	{
		*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x08;
			SV_GameSendServerCommand(cl, 1, "c \"Fall Camo ^7[^1ON^7]\"");
	}

    void RedTigerC(int cl)
	{
		*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x06;
			SV_GameSendServerCommand(cl, 1, "c \"Red Tiger Camo ^7[^1ON^7]\"");
	}

    void BlueTigerC(int cl)
	{
			*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x07;
			SV_GameSendServerCommand(cl, 1, "c \"Blue Tiger Camo ^7[^1ON^7]\"");
	}

    void UrbanC(int cl)
	{
			*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x05;
			SV_GameSendServerCommand(cl, 1, "c \"Urban Camo ^7[^1ON^7]\"");
	}

    void DigitalC(int cl)
	{
			*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x04;
			SV_GameSendServerCommand(cl, 1, "c \"Digital Camo ^7[^1ON^7]\"");

	}

    void WoodlandC(int cl)
	{	
			*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x03;
			SV_GameSendServerCommand(cl, 1, "c \"Woodland Camo ^7[^1ON^7]\"");
	}

    void ArcticC(int cl)
	{
			*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x02;
			SV_GameSendServerCommand(cl, 1, "c \"Arctic Camo ^7[^1ON^7]\"");
	}

    void DesertC(int cl)
	{
			*(char*)(0x014E245E, 0x014E2468 + 0x3700 * cl) = 0x01;
			SV_GameSendServerCommand(cl, 1, "c \"Desert Camo ^7[^1ON^7]\"");
	}

	void DesertCC(int cl)
	{
			*(char*)(0x014E245E + 0x3700 * cl) = 0x01;
			SV_GameSendServerCommand(cl, 1, "c \"Desert Camo ^7[^1ON^7]\"");
	}

	void ArcticCC(int cl)
	{
			*(char*)(0x014E245E + 0x3700 * cl) = 0x02;
			SV_GameSendServerCommand(cl, 1, "c \"Arctic Camo ^7[^1ON^7]\"");
	}

	void WoodlandCC(int cl)
	{	
			*(char*)(0x014E245E + 0x3700 * cl) = 0x03;
			SV_GameSendServerCommand(cl, 1, "c \"Woodland Camo ^7[^1ON^7]\"");
	}

	void DigitalCC(int cl)
	{
			*(char*)(0x014E245E + 0x3700 * cl) = 0x04;
			SV_GameSendServerCommand(cl, 1, "c \"Digital Camo ^7[^1ON^7]\"");

	}

	void UrbanCC(int cl)
	{
			*(char*)(0x014E245E + 0x3700 * cl) = 0x05;
			SV_GameSendServerCommand(cl, 1, "c \"Urban Camo ^7[^1ON^7]\"");
	}

	void BlueTigerCC(int cl)
	{
			*(char*)(0x014E245E + 0x3700 * cl) = 0x07;
			SV_GameSendServerCommand(cl, 1, "c \"Blue Tiger Camo ^7[^1ON^7]\"");
	}

    void RedTigerCC(int cl)
	{
		*(char*)(0x014E245E + 0x3700 * cl) = 0x06;
			SV_GameSendServerCommand(cl, 1, "c \"Red Tiger Camo ^7[^1ON^7]\"");
	}

	void FallCC(int cl)
	{
		*(char*)(0x014E245E + 0x3700 * cl) = 0x08;
			SV_GameSendServerCommand(cl, 1, "c \"Fall Camo ^7[^1ON^7]\"");
	}

	struct gentity_s {
	char unk0[0x158];
	gclient_s *client;
	char unk1[0x124];
};

	bool isGold[18];
    void SuperR(int cl)
{
    if (!isGold[cl])
    {
        giveWeapon(cl, "deserteaglegold_mp", false, true, true);
		SV_GSSC(cl, "c \"^2Gold Deagle Given\"");
    }
    else
    {
       SwitchToWeap(cl, "", getCurrentWeapon(cl));
	   SV_GSSC(cl, "c \"^1Gold Deagle Taken\"");
    }
	isGold[cl] = !isGold[cl];
}

	bool BD[18];
    void Bodies(int cl)
	{
		if(BD[cl] == false)
		{
			*(char*)(0x014FDA13 * cl) = 0x55;
			SV_GameSendServerCommand(cl, 1, "c \"Wallhack ^7[^1ON^7]\"");
			BD[cl] = true;
		}
		else
		{
			*(char*)(0x014FDA13 * cl) = 0x00;
			SV_GameSendServerCommand(cl, 1, "c \"Wallhack ^7[^5OFF^7]\"");
			BD[cl] = false;
		}
	}
	
	int EBNum;
    void ExplosiveBullets(int cl)
{
	if(EBNum == 0)
	{
	char temp[0x100];
	    stdc_snprintf(temp, 0x100, "setPerk %s specialty_explosivebullets 1", getName(cl));
        Cbuf_AddText1(temp);
        SV_GameSendServerCommand(cl, 0, "v bg_bulletExplDmgFactor \"100\" bg_bulletExplRadius \"2000\"");
		SV_GameSendServerCommand(cl, 1, "c \"Explosive Bullets ^7[^1ON/High^7]\"");
		EBNum = 1;
	}
	else if(EBNum == 1)
	{
		char temp[0x100];
		stdc_snprintf(temp, 0x100, "setPerk %s specialty_explosivebullets 1", getName(cl));
        Cbuf_AddText1(temp);
        SV_GameSendServerCommand(cl, 0, "v bg_bulletExplDmgFactor \"100\" bg_bulletExplRadius \"1000\"");
		SV_GameSendServerCommand(cl, 1, "c \"Explosive Bullets ^7[^1ON/^3Medium^7]\"");
		EBNum = 2;
	}
	else if(EBNum == 2)
	{
		char temp[0x100];
		stdc_snprintf(temp, 0x100, "setPerk %s specialty_explosivebullets 1", getName(cl));
        Cbuf_AddText1(temp);
        SV_GameSendServerCommand(cl, 0, "v bg_bulletExplDmgFactor \"100\" bg_bulletExplRadius \"500\"");
		SV_GameSendServerCommand(cl, 1, "c \"Explosive Bullets ^7[^1ON/^1Low^7]\"");
		EBNum = 3;
	}
	else if(EBNum == 3)
	{
		char temp[0x100];
		stdc_snprintf(temp, 0x100, "setPerk %s specialty_explosivebullets 0", getName(cl));
        Cbuf_AddText1(temp);
        SV_GameSendServerCommand(cl, 0, "v bg_bulletExplDmgFactor \"0.1\" bg_bulletExplRadius \"10\"");
		SV_GameSendServerCommand(cl, 1, "c \"Explosive Bullets ^7[^6OFF^7]\"");
		EBNum = 0;
	}
}

	bool AU[18];
    void AntiU(int cl)
	{
		if(AU[cl] == false)
		{
			SV_GameSendServerCommand(cl, 1, "v g_compassShowEnemies \"1\"");
			SV_GameSendServerCommand(cl, 1, "c \"HUD ^7[^1ON^7]\"");
			AU[cl] = true;
		}
		else
		{
			SV_GameSendServerCommand(cl, 1, "v hud_enable \"0\"");
			SV_GameSendServerCommand(cl, 1, "c \"HUD ^7[^6OFF^7]\"");
			AU[cl] = false;
		}
	}

	bool NR[18];
    void NoRecoil(int cl)
	{
		if(NR[cl] == false)
		{
			*(char*)(0x014E24BE + 0x3700 * cl) = 0x04;
			SV_GameSendServerCommand(cl, 1, "c \"No Recoil ^7[^1ON^7]\"");
			NR[cl] = true;
		}
		else
		{
			*(char*)(0x014E24BE + 0x3700 * cl) = 0x00, 0x00;
			SV_GameSendServerCommand(cl, 1, "c \"No Recoil ^7[^6OFF^7]\"");
			NR[cl] = false;
		}
	}

	bool RB[18];
    void Boxes(int cl)
{
	if (RB[cl] == false) {
		*(char*)(0x014E2213 + 0x3700 * cl) = 0x55;
		SV_GameSendServerCommand(cl, 1, "c \"RedBoxes ^7[^1ON^7]\"");
		RB[cl] = true;
	}
	else {
		*(char*)(0x014E2213 + 0x3700 * cl) = 0x00;
		SV_GameSendServerCommand(cl, 1, "c \"RedBoxes ^7[^6OFF^7]\"");
		RB[cl] = false;
	}
}

	bool GMA[18];
    void GodModeAll(int clientIndex) {
	GMA[clientIndex] =! GMA[clientIndex];
	char bytes[2][4] = { { 0x0F, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0x64 } };
	write_process(0x1319800 + (clientIndex * 0x280) + 0x184, GMA[clientIndex] ? bytes[0] : bytes[1], sizeof(bytes));
	Println(clientIndex, GMA[clientIndex] ? "GodMode [^1ON^7]" : "GodMode [^6OFF^7]");
}
	
	bool GM[18];
    void GodMode(int cl)
	{
		if(GM[cl] == false)
		{
			*(char*)(0x014E5429 + 0x3700 * cl) = 0xFF, 0xFF;
			SV_GameSendServerCommand(cl, 1, "c \"God Mode ^7[^1ON^7]\"");
			GM[cl] = true;
		}
		else
		{
			*(char*)(0x014E5429 + 0x3700 * cl) = 0x00, 0x00;
		    SV_GameSendServerCommand(cl, 1, "c \"God Mode ^7[^6OFF^7]\"");
			GM[cl] = false;
		}
	}
    unsigned int GClient(int cl, unsigned int Mod = 0)
	{
		return (0x14E2200 + (0x3700 * cl)) + Mod;
	}

	bool killclients[20];
    void killClient(int cl) {
		//killclients[clientIndex] = true;
		//vec3 *v = (vec3*)(G_Client(clientIndex)+0x1C);
		//v->z = (unsigned int)0xffffffff;
		*(short*)(GClient(cl)+0x24) = 0xFFFF;
	}

	bool NC[18];
    void NoClip(int cl)
	{
		if(NC[cl] == false)
		{
			*(char*)(0x014E5623 + 0x3700 * cl) = 0x01;
			SV_GameSendServerCommand(cl, 1, "c \"No Clip ^7[^1ON^7]\"");
			NC[cl] = true;
		}
		else
		{
			*(char*)(0x014E5623 + 0x3700 * cl) = 0x00;
			SV_GameSendServerCommand(cl, 1, "c \"No Clip ^7[^6OFF^7]\"");
			NC[cl] = false;
		}
	}

    bool Ammo[18];
    void InfAmmo(int cl)
{
	if(Ammo[cl] == false)
	{
		*(char*)(0x014e256c + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e24ec + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e2570 + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e2554 + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e24dc + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e2558 + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e2578 + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e24f4 + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e2560 + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
		*(char*)(0x014e2578 + 0x3700 * cl) = 0x0F, 0xFF, 0xFF, 0xFF;
	    SV_GameSendServerCommand(cl, 1, "c \"Infinite Ammo ^7[^1ON^7]\"");
		Ammo[cl] = true;
	}
	else
	{
		*(char*)(0x014e256c + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e24ec + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e2570 + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e2554 + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e24dc + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e2558 + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e2578 + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e24f4 + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e2560 + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		*(char*)(0x014e2578 + 0x3700 * cl) = 0x00, 0x00, 0x00, 0x00;
		SV_GameSendServerCommand(cl, 1, "c \"Infinite Ammo ^7[^6OFF^7]\"");
		Ammo[cl] = false;
	}
}

	bool RBXC[18];
	void RedBoxez(int client)
	{
		if(RBXC[client] == false)
		{
			*(char*)(0x014E2213 + 0x3700 * client) = 0x55;
			SV_GSSC(client, "c \"Red Boxes ^7[^1ON^7]\"");
			RBXC[client] = true;
		}
		else
		{
			*(char*)(0x014E2213 + 0x3700 * client) = 0x00;
			SV_GSSC(client, "c \"Red Boxes ^7[^6OFF^7]\"");
			RBXC[client] = false;
		}
	}

	bool TP[18];
	void ThirdPerson(int client)
	{
		if(TP[client] == false)
		{
			SV_GSSC(client, "v cg_thirdperson ""1");
			SV_GSSC(client, "c \"Third Person ^7[^1ON^7]\"");
			TP[client] = true;
		}
		else
		{
			SV_GSSC(client, "v cg_thirdperson ""0");
			SV_GSSC(client, "c \"Third Person ^7[^6OFF^7]\"");
			TP[client] = false;
		}
	}

	void level(int clientIndex) {
	SV_GSSC(clientIndex,  "N 2056 206426 6525 7F 3760 09 4623 E803 3761 09 4627 F430 3762 02 4631 14 3763 02 4635 3C 3764 02 4639 0F 3765 02 4643 14 3766 02 4647 28 3767 02 4651 0A 3752 09 4591 E803 3753 09 4595 0F40 3754 02 4599 14 3755 02 4603 3C 3756 02 4607 0F 3757 02 4611 14 3758 02 4615 28 3759 02 4619 0A 3736 09 4527 E803");
    SV_GSSC(clientIndex,  "c \"Level 70 ^7[^2Set^7]\"");
	SV_GSSC(clientIndex,  "o \"mp_level_up\"");
	}

	void UnlockAll(int clientIndex) {
	SV_GSSC(clientIndex,  "N 3737 09 4531 0F40 3738 02 4535 14 3739 02 4539 3C 3740 02 4543 0F 3741 02 4547 14 3742 02 4551 28 3743 02 4555 0A 3799 09 4779 E803 3800 09 4783 0F40 3801 02 4787 14 3802 02 4791 3C 3803 02 4795 0F 3804 02 4799 14 3805 02 4803 28 3806 02 4807 0A");
    SV_GSSC(clientIndex,  "N 3775 09 4683 E803 3776 09 4687 0F40 3777 02 4691 14 3778 02 4695 3C 3779 02 4699 0F 3780 02 4703 14 3781 02 4707 28 3782 02 4711 0A 3728 09 4495 E803 3729 09 4499 0F40 3730 02 4503 14 3731 02 4507 3C 3732 02 4511 0F 3733 02 4515 14 3734 02 4519 28 3735 02 4523 0A 3783 09 4715 E803 3784 09 4719 0F40 3785 02 4723 14 3786 02 4727 3C");
    SV_GSSC(clientIndex,  "N 3787 02 4731 0F 3788 02 4735 14 3789 02 4739 28 3790 02 4743 0A 3791 09 4747 E803 3864 02 5039 14 3865 02 5043 28 3866 02 5047 09 3888 09 5135 E803 3887 09 5131 0F40");
    SV_GSSC(clientIndex,  "N 3792 09 4751 0F40 3793 02 4755 14 3794 02 4759 3C 3795 02 4763 0F 3796 02 4767 14 3797 02 4771 28 3798 02 4775 0A 3744 09 4559 E803 3745 09 4563 0F40 3746 02 4567 14 3889 02 5139 0F 3890 02 5143 3C 3891 02 5147 14 3892 02 5151 28 3893 02 5155 09 3807 09 4811 E803 3808 09 4815 0F40 3809 02 4819 0F 3810 02 4823 14 3811 02 4827 28");
    SV_GSSC(clientIndex,  "N 3747 02 4571 3C 3748 02 4575 0F 3749 02 4579 14 3750 02 4583 28 3751 02 4587 0A 3853 09 4995 E803 3854 09 4999 0F40 3855 02 5003 1E 3856 02 5007 3C 3857 02 5011 14 3858 02 5015 28 3859 02 5019 09 3839 09 4939 E803 3840 09 4943 0F40 3841 02 4947 1E 3842 02 4951 3C 3843 02 4955 14 3844 02 4959 28 3845 02 4963 09 3825 09 4883 E803");
    SV_GSSC(clientIndex,  "N 3826 09 4887 0F40 3827 02 4891 1E 3828 02 4895 3C 3829 02 4899 14 3830 02 4903 28 3831 02 4907 09 3832 09 4911 E803 3833 09 4915 0F40 3834 02 4919 1E 3835 02 4923 3C 3836 02 4927 14 3837 02 4931 28 3838 02 4935 09 3846 09 4967 E803 3847 09 4971 0F40");
    SV_GSSC(clientIndex,  "N 3848 02 4975 1E 3849 02 4979 3C 3850 02 4983 14 3851 02 4987 28 3852 02 4991 09 3768 09 4655 E803 3769 09 4659 0F40 3771 02 4667 0F 3770 02 4663 3C 3772 02 4671 14 3773 02 4675 28 3774 02 4679 09 3874 09 5079 E803 3875 09 5083 0F40 3876 02 5087 0F");
    SV_GSSC(clientIndex,  "N 3877 02 5091 3C 3878 02 5095 14 3879 02 5099 28 3880 02 5103 09 3867 09 5051 E803 3868 09 5055 0F40 3869 02 5059 0F 3870 02 5063 3C 3871 02 5067 14 3872 02 5071 28 3873 02 5075 09 3860 09 5023 E803 3861 09 5027 0F40 3862 02 5031 0F 3863 02 5035 3C");
    SV_GSSC(clientIndex,  "N 3812 02 4831 06 3813 09 4835 E803 3814 09 4839 0F40 3815 02 4843 0F 3816 02 4847 14 3817 02 4851 28 3818 02 4855 06 3819 09 4859 E803 3820 09 4863 0F40 3821 02 4867 0F 3822 02 4871 14 3823 02 4875 28 3824 02 4879 06 3881 09 5107 E803 3882 09 5111 0F40");
    SV_GSSC(clientIndex,  "N 3883 02 5115 0F 3884 02 5119 14 3885 02 5123 28 3886 02 5127 06 3898 09 5175 E803 3899 09 5179 0F40 3894 09 5159 E803 3895 09 5163 0F40 3900 09 5183 E803 3901 09 5187 0F40 3896 09 5167 E803 3897 09 5171 0F40 3902 09 5191 E803 3903 09 5195 0F40 3908 09 5215 E803");
    SV_GSSC(clientIndex,  "N 3909 09 5219 0F40 3904 09 5199 E803 3905 09 5203 0F40 3906 09 5207 E803 3907 09 5211 0F40 3912 06 5231 C409 3913 09 5235 0F40 3910 06 5223 C409 3911 09 5227 0F40 3916 09 5247 E803 3917 09 5251 0F40 3914 09 5239 E803 3915 09 5243 0F40 3920 07 5263 C409 3921 09 5267 0F40");
    SV_GSSC(clientIndex,  "N 3918 07 5255 C409 3919 09 5259 0F40 3922 09 5271 B004 3923 09 5275 B004 3924 09 5279 B004 3925 09 5283 B004 3926 09 5287 FA 3643 0A 4155 09 3927 07 5292 6108 3931 07 5307 EE02 3938 07 5335 0F40 3932 07 5311 8403 3935 07 5323 EE02 3933 07 5315 E803 3941 07 5347 402414");
    SV_GSSC(clientIndex,  "N 3934 07 5319 FA 3936 07 5327 FA 3942 07 5351 0F40 3939 07 5339 64 3928 07 5295 0F40 3930 07 5303 FA 3929 07 5299 FA 3940 07 5343 EE02 3937 07 5331 64 3943 04 5355 32 3944 04 5359 32 3945 04 5363 32 3946 04 5367 32 3947 04 5371 32 3948 04 5375 32");
    SV_GSSC(clientIndex,  "N 3949 04 5379 32 3950 04 5383 32 3951 04 5387 19 3952 04 5391 19 3953 04 5395 19 3954 04 5399 19 3955 04 5403 19 3956 04 5407 0A 3957 04 5411 0A 3958 04 5415 E803 3959 04 5419 E803 3960 04 5423 E803 3961 04 5427 E803 3962 04 5431 32 3963 04 5435 1E 3964 04 5439 32 3965 04 5443 1E 3966 04 5447 32 3967 04 5451 1E 3968 04 5455 1E");
    SV_GSSC(clientIndex,  "N 3969 02 5459 FF 3972 02 5471 FF 3973 02 5475 FF 3983 02 5515 FF 3984 02 5519 FF 3985 02 5523 FF 3986 02 5527 FF 3987 02 5531 FF 3988 02 5535 FF 4100 02 5983 FF 3970 02 5463 19 3971 02 5467 19 4020 04 5663 1E 4021 04 5667 1E 4022 04 5671 1E 4023 04 5675 0F 4024 04 5679 0F 4025 04 5683 0F");
    SV_GSSC(clientIndex,  "N 3989 02 5539 FF 3990 02 5543 FF 3991 02 5547 FF 3992 02 5551 FF 3994 02 5559 FF 3995 02 5563 FF 3996 02 5567 FF 3997 02 5571 FF 4001 02 5587 FF 4002 02 5591 FF 4028 04 5695 50C3 4029 04 5699 50C3 4030 04 5703 64 4035 04 5723 32 4036 04 5727 32 4037 04 5731 32 4038 04 5735 32 4039 04 5739 32 4040 04 5743 32");
    SV_GSSC(clientIndex,  "N 4003 02 5595 FF 4004 02 5599 FF 4005 02 5603 FF 4006 02 5607 FF 4007 02 5611 FF 4008 02 5615 FF 4009 02 5619 FF 4010 02 5623 FF 4011 02 5627 FF 4012 02 5631 FF 4101 04 5987 C8 4103 04 5995 0A 4104 04 5999 1E 4105 04 6003 1E 3993 04 5555 14 3998 04 5575 C8 3999 03 5579 0A 4000 03 5583 0A 4107 04 6011 0F");
    SV_GSSC(clientIndex,  "N 4013 02 5635 FF 4014 02 5639 FF 4015 02 5643 FF 4016 02 5647 FF 4017 02 5651 FF 4018 02 5655 FF 4114 02 6039 FF 4110 02 6023 FF 4106 02 6007 FF 4019 02 5659 FF 4041 04 5747 32 4050 03 5783 19 4051 03 5787 19 4055 03 5803 19 4056 03 5807 19 4065 04 5843 14 4068 04 5855 14 4069 04 5859 14 4058 03 5815 19");
    SV_GSSC(clientIndex,  "N 4026 02 5687 FF 4027 02 5691 FF 4042 02 5751 FF 4031 02 5707 FF 4032 02 5711 FF 4033 02 5715 FF 4034 02 5719 FF 4043 02 5755 FF 4044 02 5759 FF 4045 02 5763 FF 4108 04 6015 32 4109 02 6019 0A 4111 03 6027 0A 4112 03 6031 0A 4113 03 6035 0A 4115 03 6043 0A 4116 05 6047 FA 4117 05 6051 64 4118 05 6055 E803");
    SV_GSSC(clientIndex,  "N 4046 02 5767 FF 4047 02 5771 FF 4048 02 5775 FF 4049 02 5779 FF 4052 02 5791 FF 4053 02 5795 FF 4054 02 5799 FF 4102 02 5991 FF 4121 02 6067 FF 4057 02 5811 FF 4119 05 6059 2C00 4120 05 6063 2C00 6525 7F");
    SV_GSSC(clientIndex,  "N 4059 02 5819 OO 4060 02 5823 OO 4061 02 5827 OO 4062 02 5831 OO 4063 02 5835 OO 4064 02 5839 OO 4066 02 5847 OO 4067 02 5851 OO 4070 02 5863 OO 4071 02 5867 OO 4072 02 5871 OO 4073 02 5875 OO 4074 02 5879 OO 4075 02 5883 OO 4076 02 5887 OO 4077 02 5891 OO 4078 02 5895 OO 4079 02 5899 OO 4080 02 5903 OO 4081 02 5907 OO");
    SV_GSSC(clientIndex,  "N 4082 02 5911 OO 4083 02 5915 OO 4084 02 5919 OO 4085 02 5923 OO 4086 02 5927 OO 4087 02 5931 OO 4088 02 5935 OO 4089 02 5939 OO 4090 02 5943 OO 4091 02 5947 OO 4092 02 5951 OO 4093 02 5955 OO 4094 02 5959 OO 4095 02 5963 OO 4096 02 5967 OO 4097 02 5971 OO 4098 02 5975 OO 4099 02 5979 OO 4100 02 5983 OO 4099 02 5979 OO");
    SV_GSSC(clientIndex,  "N 3038 05 6695 80 6696 10 6697 02 6697 42 6696 11 6696 31 6697 46 6697 C6 6696 33 6696 73 6697 CE 6698 09 6696 7B 6697 CF 6697 EF 6698 0D 6696 7F 6696 FF 6697 FF 6698 0F 6637 84 6637 8C 6503 03 6637 9C 6637 BC 6503 07 6637 FC 6638 FF 6503 0F 6638 03 6638 07");
    SV_GSSC(clientIndex,  "N 6503 1F 6638 0F 6638 1F 6638 3F 6503 3F 6638 7F 6638 FF 6503 7F 6639 FF 6639 03 6639 07 6503 FF 6639 0F 6639 1F 6504 FF 6639 3F 6639 7F 6639 FF 6504 03 6640 09 6640 0B 6504 07 6640 0F 6640 1F 6504 0F 6640 3F 6640 7F 6504 1F 6640 FF 6641 23 6504 3F 6641 27");
    SV_GSSC(clientIndex,  "N 3038 05 3550 05 3614 05 3486 05 3422 05 3358 05 3294 05 3230 05 3166 05 3102 05 3038 05 2072 2D302E302F30O 2092 30303130 2128 3130 2136 3B05ZZ3C05 2152 3D05O");
    SV_GSSC(clientIndex,  "N 6641 2F 6504 7F 6641 3F 6641 7F 6504 FF 6641 FF 6642 85 6505 FF 6642 87 6642 8F 6505 03 6642 9F 6642 BF 6505 07 6642 FF 6643 11 6505 0F 6643 13 6643 17 6505 1F 6643 1F 6643 3F 6505 3F 6643 7F 6643 FF 6505 7F 6644 43 6644 47 6505 FF 6644 4F 6644 5F 6506 FF");
    SV_GSSC(clientIndex,  "N 6644 7F 6644 FF 6506 03 6645 09 6645 0B 6506 07 6645 0F 6645 1F 6506 0F 6645 3F 6645 7F 6506 1F 6645 FF 6646 23 6506 3F 6646 27 6646 2F 6506 7F 6646 3F 6646 7F 6506 FF 6646 FF 6647 85 6507 FF 6647 87 6647 8F 6507 03 6647 9F 6647 BF 6507 07 6647 FF 6648 11");
    SV_GSSC(clientIndex,  "N 6507 0F 6648 13 6648 17 6507 1F 6648 1F 6648 3F 6507 3F 6648 7F 6648 FF 6507 7F 6649 FF 6649 03 6649 07 6507 FF 6649 0F 6649 1F 6508 FF 6649 3F 6649 7F 6649 FF 6508 03 6650 FF 6650 03 6508 07 6650 07 6650 0F 6650 1F 6508 0F 6650 3F 6650 7F 6508 1F 6650 FF");
    SV_GSSC(clientIndex,  "N 6651 FF 6651 03 6508 3F 6651 07 6651 0F 6508 7F 6651 1F 6651 3F 6508 FF 6651 7F 6651 FF 6509 FF 6652 FF 6652 03 6509 03 6652 07 6652 0F 6509 07 6652 1F 6652 3F 6509 0F 6652 7F 6652 FF 6509 1F 6653 FF 6653 03 6509 3F 6653 07 6653 0F 6509 7F 6653 1F 6653 3F");
    SV_GSSC(clientIndex,  "N 6509 FF 6653 7F 6653 FF 6510 FF 6654 FF 6654 03 6510 03 6654 07 6654 0F 6510 07 6654 1F 6654 3F 6510 0F 6654 7F 6654 FF 6510 1F 6655 FF 6655 03 6510 3F 6655 07 6655 0F 6510 7F 6655 1F 6655 3F 6510 FF 6655 7F 6655 FF 6511 FF 6656 FF 6656 03 6511 03 6656 07");
    SV_GSSC(clientIndex,  "N 6656 0F 6511 07 6656 1F 6656 3F 6511 0F 6656 7F 6656 FF 6511 1F 6657 FF 6657 03 6511 3F 6657 07 6657 0F 6511 7F 6657 1F 6657 3F 6511 FF 6657 7F 6657 FF 6512 FF 6658 FF 6658 03 6512 03 6658 07 6658 0F 6512 07 6658 1F 6658 9F 6658 BF 6658 FF 6680 FF 6661 5B");
    SV_GSSC(clientIndex,  "N 6661 5F 6661 7F 6661 FF 6673 08 6673 18 6673 38 6673 78 6673 F8 6674 FF 6674 03 6674 07 6674 0F 6674 1F 6674 3F 6674 7F 6674 FF 6679 08 6673 F9 6673 FB 6673 FF 6675 FF 6677 FF 6677 03 6677 07 6677 0F 6677 1F 6677 3F 6677 7F 6677 FF 6679 09 6679 0B 6679 0F");
    SV_GSSC(clientIndex,  "N 6679 1F 6679 3F 6679 7F 6679 FF 6680 03 6680 07 6680 0F 6680 1F 6680 3F 6680 BF 6681 FF 6681 03 6681 0B 6681 1B 6681 3B 6681 7B 6681 FB 6681 FF 6680 FF 6686 FF 6632 FF 6632 03 6632 07 6632 0F 6632 1F 6632 3F 6632 7F 6632 FF 6633 FF 6633 03 6633 07 6633 0F");
    SV_GSSC(clientIndex,  "N 6633 1F 6633 3F 6633 7F 6633 FF 6634 FF 6634 03 6634 07 6634 0F 6634 1F 6634 3F 6634 7F 6634 FF 6635 FF 6635 03 6635 07 6635 0F 6635 1F 6635 3F 6635 7F 6635 FF 6636 FF 6636 03 6636 07 6636 0F 6636 1F 6636 3F 6636 7F 6636 FF 6637 FD 6637 FF 6690 FF 6690 03");
    SV_GSSC(clientIndex,  "N 6690 07 6690 0F 6690 1F 6690 3F 6690 7F 6690 FF 6695 81 6695 83 6695 87 6695 8F 6695 9F 6695 BF 6698 1F 6698 3F 6698 7F 6698 FF 6699 C1 6699 C3 6699 C7 6699 CF 6699 DF 6699 FF 6700 1F 6700 3F 6700 7F 6700 FF 6701 03 6701 07 6701 0F 6701 1F 6701 3F 6701 7F");
    SV_GSSC(clientIndex,  "N 6701 FF 6702 FF 6702 03 6702 07 6524 10 6524 30 6524 70 6524 F0 6529 FF 6529 03 6529 07 6530 08 6529 0F 6529 1F 6529 3F 6529 7F 6529 FF 6530 09 6530 0B 6530 0F 6530 1F 6530 7F 6530 FF 6531 FF 6531 03 6531 07 6531 0F 6531 1F 6531 3F 6531 7F 6531 FF 6532 FF");
    SV_GSSC(clientIndex,  "N 6532 03 6532 07 6532 0F 6512 C7 6526 02 6512 D7 6526 06 6512 F7 6526 86 6532 1F 6532 3F 6532 BF 6533 F9 6533 FB 6533 FF 6532 FF 6526 87 6526 A7 6512 FF 6540 7F 6526 E7 6526 EF 6526 FF 6517 FF 6527 FF 6528 FF 6522 FF 6524 F1 6524 F3 6524 F7 6524 FF");
    SV_GSSC(clientIndex,  "N 3850 99 3851 99 3852 99 3853 99 3854 99 3855 99 3856 99 3857 99 3858 99 3859 99 3860 99 3861 99 3862 99 3863 99 3864 99 3865 99 3866 99 3867 99 3868 99 3869 99 3870 99 3871 99 3872 99 3873 99 3874 99 3875 99 3876 99 3877 99 3878 99 3879 99 3880 99 3881 99 3882 99 3883 99 3884 99 3885 99 3886 99 3887 99 3888 99 3889 99 3890 99 3891 99 3892 99 3893 99 3894 99 3895 99 3896 99 3897 99 3898 99 3899 99 3900 99");
    SV_GSSC(clientIndex,  "N 3900 99 3901 99 3902 99 3903 99 3904 99 3905 99 3906 99 3907 99 3908 99 3909 99 3910 99 3911 99 3912 99 3913 99 3914 99 3915 99 3916 99 3917 99 3918 99 3919 99 3920 99 3921 99 3922 99 3923 99 3924 99 3925 99 3926 99 3927 99 3928 99 3929 99 3930 99 3931 99 3932 99 3933 99 3934 99 3935 99 3936 99 3937 99 3938 99 3939 99 3940 99 3941 99 3942 99 3943 99 3944 99 3945 99 3946 99 3947 99 3948 99 3949 99 3950 99");
    SV_GSSC(clientIndex,  "N 3950 99 3951 99 3952 99 3953 99 3954 99 3955 99 3956 99 3957 99 3958 99 3959 99 3960 99 3961 99 3962 99 3963 99 3964 99 3965 99 3966 99 3967 99 3968 99 3969 99 3970 99 3971 99 3972 99 3973 99 3974 99 3975 99 3976 99 3977 99 3978 99 3979 99 3980 99 3981 99 3982 99 3983 99 3984 99 3985 99 3986 99 3987 99 3988 99 3989 99 3990 99 3991 99 3992 99 3993 99 3994 99 3995 99 3996 99 3997 99 3998 99 3999 99 4000 99");
    SV_GSSC(clientIndex,  "N 4000 99 4001 99 4002 99 4003 99 4004 99 4005 99 4006 99 4007 99 4008 99 4009 99 4010 99 4011 99 4012 99 4013 99 4014 99 4015 99 4016 99 4017 99 4018 99 4019 99 4020 99 4021 99 4022 99 4023 99 4024 99 4025 99 4026 99 4027 99 4028 99 4029 99 4030 99 4031 99 4032 99 4033 99 4034 99 4035 99 4036 99 4037 99 4038 99 4039 99 4040 99 4041 99 4042 99 4043 99 4044 99 4045 99 4046 99 4047 99 4048 99 4049 99 4050 99");
    SV_GSSC(clientIndex,  "N 4050 99 4051 99 4052 99 4053 99 4054 99 4055 99 4056 99 4057 99 4058 99 4059 99 4060 99 4061 99 4062 99 4063 99 4064 99 4065 99 4066 99 4067 99 4068 99 4069 99 4070 99 4071 99 4072 99 4073 99 4074 99 4075 99 4076 99 4077 99 4078 99 4079 99 4080 99 4081 99 4082 99 4083 99 4084 99 4085 99 4086 99 4087 99 4088 99 4089 99 4090 99 4091 99 4092 99 4093 99 4094 99 4095 99 4096 99 4097 99 4098 99 4099 99 4100 99");
	SV_GSSC(clientIndex,  "c \"Unlock All ^7[^2Started^7]\"");
	SV_GSSC(clientIndex,  "c \"Unlock All ^7[^220%^7]\"");
	SV_GSSC(clientIndex,  "c \"Unlock All ^7[^240%^7]\"");
	SV_GSSC(clientIndex,  "c \"Unlock All ^7[^260%^7]\"");
	SV_GSSC(clientIndex,  "c \"Unlock All ^7[^280%^7]\"");
	SV_GSSC(clientIndex,  "c \"Unlock All ^7[^2100%^7]\"");
	SV_GSSC(clientIndex,  "c \"Unlock All ^7[^2Complete^7]\"");
	SV_GSSC(clientIndex,  "o \"mp_level_up\"");
}

	int PrestigeNumber;
	void Prestige(int client)
	{
		if(PrestigeNumber == 0)
		{
			*(char*)(0x01FF9A9C) = 0x00;
			SV_GSSC(client,  "c \"Prestige: ^7[^20^7]\"");
			PrestigeNumber = 1;
		}
		else if(PrestigeNumber == 1)
		{
			*(char*)(0x01FF9A9C) = 0x01;
			SV_GSSC(client,  "c \"Prestige: ^7[^21^7]\"");
			PrestigeNumber = 2;
		}
		else if(PrestigeNumber == 2)
		{
			*(char*)(0x01FF9A9C) = 0x02;
			SV_GSSC(client,  "c \"Prestige: ^7[^22^7]\"");
			PrestigeNumber = 3;
		}
		else if(PrestigeNumber == 3)
		{
			*(char*)(0x01FF9A9C) = 0x03;
			SV_GSSC(client,  "c \"Prestige: ^7[^23^7]\"");
			PrestigeNumber = 4;
		}
		else if(PrestigeNumber == 4)
		{
			*(char*)(0x01FF9A9C) = 0x04;
			SV_GSSC(client,  "c \"Prestige: ^7[^24^7]\"");
			PrestigeNumber = 5;
		}
		else if(PrestigeNumber == 5)
		{
			*(char*)(0x01FF9A9C) = 0x05;
			SV_GSSC(client,  "c \"Prestige: ^7[^25^7]\"");
			PrestigeNumber = 6;
		}
		else if(PrestigeNumber == 6)
		{
			*(char*)(0x01FF9A9C) = 0x06;
			SV_GSSC(client,  "c \"Prestige: ^7[^26^7]\"");
			PrestigeNumber = 7;
		}
		else if(PrestigeNumber == 7)
		{
			*(char*)(0x01FF9A9C) = 0x07;
			SV_GSSC(client,  "c \"Prestige: ^7[^27^7]\"");
			PrestigeNumber = 8;
		}
		else if(PrestigeNumber == 8)
		{
			*(char*)(0x01FF9A9C) = 0x08;
			SV_GSSC(client,  "c \"Prestige: ^7[^28^7]\"");
			PrestigeNumber = 9;
		}
		else if(PrestigeNumber == 9)
		{
			*(char*)(0x01FF9A9C) = 0x09;
			SV_GSSC(client,  "c \"Prestige: ^7[^29^7]\"");
			PrestigeNumber = 10;
		}
		else if(PrestigeNumber == 10)
		{
			*(char*)(0x01FF9A9C) = 0x10;
			SV_GSSC(client,  "c \"Prestige: ^7[^210^7]\"");
			PrestigeNumber = 11;
		}
		else if(PrestigeNumber == 11)
		{
			*(char*)(0x01FF9A9C) = 0x11;
			SV_GSSC(client,  "c \"Prestige: ^7[^211^7]\"");
			PrestigeNumber = 0;
		}
	}

void ForceHost(int cl)
	{

	   cBuff_AddText(0, "xblive_privatematch 0;xblive_rankedmatch 1;disconnect;killserver;disconnect;xstartlobby;set party_connectToOthers 0;xsearchforgames;set party_hostmigration 0;set party_gameStartTimerLength 1;set party_pregameStartTimerLength 1;set party_timer 1;set badhost_endGameIfISuck 0;set party_minplayers 1;set party_autoteams 0");
	   SV_GameSendServerCommand(cl, 1, "c \"Mega Force Host ^7[^1ON^7]\"");

	}

void KickClient(int clientNum, const char* Reason, bool NotifyClient) {
		char buf[100];
		stdc_snprintf(buf, 100, "c \"^2You Kicked [^1 %s ^2]", getPlayerNameStr(clientNum));
		SV_GameSendServerCommand(myIndex, 1, buf);
		if (Reason != "" && NotifyClient)
			Kick_Client(client_s(clientNum), Reason, NotifyClient);
		else
			Kick_Client(client_s(clientNum), "Player kicked", false);
	}

namespace HUD_ELEMS {
	    
		union color_s
		{
			struct
			{
				int8_t r;
				int8_t g;
				int8_t b;
				int8_t a;
			};
			int32_t rgba;
		};

		struct opd_s {
			unsigned int sub;
			unsigned int toc;
		};

		struct hudelem_s
		{
			int type;                    // 0x00-0x04
			float y;                      // 0x04-0x08
			float x;                      // 0x08-0x0C
			float z;                      // 0x0C-0x10
			int targetEntNum;            // 0x10-0x14
			float fontScale;              // 0x14-0x18
			float fromFontScale;          // 0x18-0x1C
			int fontScaleStartTime;      // 0x1C-0x20
			int fontScaleTime;            // 0x20-0x24
			int label;                    // 0x24-0x28
			int font;                    // 0x28-0x2C
			int alignOrg;                // 0x2C-0x30
			int alignScreen;              // 0x30-0x34
			color_s color;        // 0x34-0x38
			color_s fromColor;    // 0x38-0x3C
			int fadeStartTime;            // 0x3C-0x40
			int fadeTime;                // 0x40-0x44
			int height;                  // 0x44-0x48
			int width;                    // 0x48-0x4C
			int materialIndex;            // 0x4C-0x50
			int fromHeight;              // 0x50-0x54
			int fromWidth;                // 0x54-0x58
			int scaleStartTime;          // 0x58-0x5C
			int scaleTime;                // 0x5C-0x60
			float fromY;                  // 0x60-0x64
			float fromX;                  // 0x64-0x68
			int fromAlignOrg;            // 0x68-0x6C
			int fromAlignScreen;          // 0x6C-0x70
			int moveStartTime;            // 0x70-0x74
			int moveTime;                // 0x74-0x78
			float value;                  // 0x78-0x7C
			int time;                    // 0x7C-0x80
			int duration;                // 0x80-0x84
			int text;                    // 0x84-0x88
			float sort;                  // 0x88-0x8C
			color_s glowColor;    // 0x8C-0x90
			int fxBirthTime;              // 0x90-0x94
			int fxLetterTime;            // 0x94-0x98
			int fxDecayStartTime;        // 0x98-0x9C
			int fxDecayDuration;          // 0x9C-0xA0
			int soundID;                  // 0xA0-0xA4
			int flags;                    // 0xA4-0xA8
		};

		struct game_hudelem_s
		{
			hudelem_s elem;              // 0x00-0xA8
			int clientNum;                // 0xA8-0xAC
			int teamNum;                  // 0xAC-0xB0
			int archived;                // 0xB0-0xB4
		};

		opd_s cmsg = {0x5EF68,TOC};
	    void (*CG_GameMessage)(int cl,const char* msg) = (void(*)(int,const char*))&cmsg;

		opd_s HDA = { 0x001826B0, TOC };
		void(*HudElem_DestroyAll)() = (void(*)())&HDA;
		opd_s HDF = { 0x00182660, TOC };
		void(*HudElem_Free)(game_hudelem_s* elem) = (void(*)(game_hudelem_s*))&HDF;
		opd_s GMI = { 0x001BE758, TOC };
		int(*GetMaterialIndex)(const char* Material) = (int(*)(const char*))&GMI;
		opd_s HEA = { 0x001806E0, TOC };
		game_hudelem_s*(*HudElemAlloc)(int32_t client, int32_t teamNum) = (game_hudelem_s*(*)(int32_t, int32_t))&HEA;

		int G_LocalizedStringIndex(const char* Text)
		{
			int(*LocalizedStringIndex)(const char* Text) = (int(*)(const char*))0x00708080;
			return LocalizedStringIndex(Text);
		}

		game_hudelem_s* SetShader(int clientIndex, const char* Shader, int Width, int Height, float X, float Y, int Allign = 5, unsigned char R = 0, unsigned char G = 0, unsigned char B = 0, unsigned char A = 0)
		{
			*(int*)0x131689C = 1; //Allows to use precached Shaders
			game_hudelem_s* elem = HudElemAlloc(clientIndex, 0);
			elem->clientNum = clientIndex;
			elem->elem.type = 6;
			elem->elem.materialIndex = GetMaterialIndex(Shader);
			elem->elem.width = Width;
			elem->elem.height = Height;
			elem->elem.x = X;
			elem->elem.y = Y;
			elem->elem.alignOrg = Allign;
			elem->elem.color.r = R;
			elem->elem.color.g = G;
			elem->elem.color.b = B;
			elem->elem.color.a = A;
			return elem;
		}

		game_hudelem_s* SetText(int clientIndex, const char* Text, int Font, float FontScale, float X, float Y, int Allign, float Sort, unsigned char R = 0, unsigned char G = 0, unsigned char B = 0, unsigned char A = 0, unsigned char glowR = 0, unsigned char glowG = 0, unsigned char glowB = 0, unsigned char glowA = 0)
		{
			*(int*)0x131689C = 1; //Allows to use precached Shaders
			game_hudelem_s* elem = HudElemAlloc(clientIndex, 0);
			elem->clientNum = clientIndex;
			elem->elem.type = 1;
			elem->elem.text = G_LocalizedStringIndex(Text);
			elem->elem.font = Font;
			elem->elem.fontScale = FontScale;
			elem->elem.x = X;
			elem->elem.y = Y;
			elem->elem.alignOrg = 0;
			elem->elem.alignScreen = 1;
			elem->elem.sort = Sort;
			elem->elem.color.r = R;
			elem->elem.color.g = G;
			elem->elem.color.b = B;
			elem->elem.color.a = A;
			elem->elem.glowColor.r = glowR;
			elem->elem.glowColor.g = glowG;
			elem->elem.glowColor.b = glowB;
			elem->elem.glowColor.a = glowA;
			return elem;
		}
		
		void FadeOverTime(game_hudelem_s * elem, float time, unsigned char R = 0, unsigned char G = 0, unsigned char B = 0, unsigned char A = 0)
		{
			*(int*)0x131689C = 1;
			int * levelTime = (int*)0x0012E0304;
			elem->elem.fadeStartTime = *levelTime;
			elem->elem.fromColor = elem->elem.color;
			elem->elem.color.r = R;
			elem->elem.color.g = G;
			elem->elem.color.b = B;
			elem->elem.color.a = A;
			elem->elem.fadeTime = (int)floor(time * 1000 + 0.5);
		}

		void FadeAlphaOverTime(game_hudelem_s * elem, float time, unsigned char Alpha = 0)
		{
			*(int*)0x131689C = 1;
			int * levelTime = (int*)0x0012E0304;
			elem->elem.fadeStartTime = *levelTime;
			elem->elem.fromColor = elem->elem.color;
			elem->elem.color.a = Alpha;
			elem->elem.fadeTime = (int)floor(time * 1000 + 0.5);
		}

		void ChangeText(game_hudelem_s * elem, char* newText)
		{
			elem->elem.text = G_LocalizedStringIndex(newText);
		}

		void ChangeShader(game_hudelem_s * elem, char* Shader)
		{
			elem->elem.materialIndex = GetMaterialIndex(Shader);
		}

		void SetPulseFX(game_hudelem_s* elem, int speed, int decayStart, int decayDuration)
        {
             int levelTime = *reinterpret_cast<int*>(0x0012E0304); // Assuming levelTime is a pointer to an int
             elem->elem.fxBirthTime = levelTime;
             elem->elem.fxLetterTime = speed;
             elem->elem.fxDecayStartTime = decayStart;
             elem->elem.fxDecayDuration = decayDuration;
        }

		void ScaleTextOverTime(game_hudelem_s * elem, float time, float fontScale)
		{

			int * levelTime = (int*)0x0012E0304;
			elem->elem.fontScaleStartTime = *levelTime;
			elem->elem.fromFontScale = elem->elem.fontScale;
			elem->elem.fontScale = fontScale;
			elem->elem.fontScaleTime = (int)floor(time * 1000 + 0.5);
		}

		void MoveOverTime(game_hudelem_s * elem, float time, float X, float Y)
		{
			int * levelTime = (int*)0x0012E0304;
			elem->elem.moveStartTime = *levelTime;
			elem->elem.fromAlignOrg = elem->elem.alignOrg;
			elem->elem.fromAlignScreen = elem->elem.alignScreen;
			elem->elem.fromX = elem->elem.x;
			elem->elem.fromY = elem->elem.y;
			elem->elem.x = X;
			elem->elem.y = Y;
			elem->elem.moveTime = (int)floor(time * 1000 + 0.5);
		}

		enum TypewriterTextTypes
	{
		Normal = 0x800,
		Classic = 0x4000
	};

	    game_hudelem_s* typewriter;
	    bool typran;

	    void SetTypewriter(int clientIndex, char* Text, int IconType, int Font, float FontSize, float X, float Y, char align, int WriteType = Normal, short Lettertime = 200, short fxDecayStartTime = 7000, short fxDecayDuration = 1000, char R = 255, char G = 255, char B = 255, char A = 255, char GlowR = 255, char GlowG = 255, char GlowB = 255, char GlowA = 0)
	{
		if(!typran)
		{
			game_hudelem_s* elem = SetText(clientIndex, Text, Font, FontSize, X, Y, align, R, G, B, A, GlowR, GlowG, GlowB, GlowA);
			elem->elem.flags = WriteType;
			int * levelTime = (int*)0x0012E0304;
			elem->elem.fxBirthTime = *levelTime;
			elem->elem.fxLetterTime = Lettertime;
			elem->elem.fxDecayStartTime = fxDecayStartTime;
			elem->elem.fxDecayDuration = fxDecayDuration;
			typewriter = elem;
			typran = true;
		}
		else
		{
			game_hudelem_s* elem = typewriter;
			elem->clientNum = clientIndex;
			elem->elem.type = 1;
			elem->elem.text = G_LocalizedStringIndex(Text);
			elem->elem.font = Font;
			elem->elem.fontScale = FontSize;
			if (align != 0)
			{ elem->elem.alignOrg = 5; elem->elem.alignScreen = align; elem->elem.x = X; elem->elem.y = Y; }
			else
			{ elem->elem.x = X; elem->elem.y = Y; } 
			elem->elem.color.r = R;
			elem->elem.color.g = G;
			elem->elem.color.b = B;
			elem->elem.color.a = A;
			elem->elem.glowColor.r = GlowR;
			elem->elem.glowColor.g = GlowG;
			elem->elem.glowColor.b = GlowB;
			elem->elem.glowColor.a = GlowA;
			elem->elem.flags = WriteType;
			int * levelTime = (int*)0x0012E0304;
			elem->elem.fxBirthTime = *levelTime;
			elem->elem.fxLetterTime = Lettertime;
			elem->elem.fxDecayStartTime = fxDecayStartTime;
			elem->elem.fxDecayDuration = fxDecayDuration;
		}
	}
	
	    game_hudelem_s* typewriter1;
	    bool Message;

		void SetMessage(int clientIndex, char* Text, char* Text2, int WriteType = Normal, short Lettertime = 200, short fxDecayStartTime = 7000, short fxDecayDuration = 1000, char R = 255, char G = 255, char B = 255, char A = 255, char GlowR = 255, char GlowG = 0, char GlowB = 0, char GlowA = 100)
	{
		if(!Message)
		{
			game_hudelem_s* elem1 = SetText(clientIndex, Text, 5, 1, 325, 130, 1, R, G, B, A, GlowR, GlowG, GlowB, GlowA);
			elem1->elem.flags = WriteType;
			int * levelTime = (int*)0x0012E0304;
			elem1->elem.fxBirthTime = *levelTime;
			elem1->elem.fxLetterTime = Lettertime;
			elem1->elem.fxDecayStartTime = fxDecayStartTime;
			elem1->elem.fxDecayDuration = fxDecayDuration;
			typewriter = elem1;
			game_hudelem_s* elem = SetText(clientIndex, Text2, 5, 1, 325, 150, 1, R, G, B, A, GlowR, GlowG, GlowB, GlowA);
			elem->elem.flags = WriteType;
			elem->elem.fxBirthTime = *levelTime;
			elem->elem.fxLetterTime = Lettertime;
			elem->elem.fxDecayStartTime = fxDecayStartTime;
			elem->elem.fxDecayDuration = fxDecayDuration;
			typewriter1 = elem;
			Message = true;
			typran = true;
		}
		else
		{
			game_hudelem_s* elem1 = typewriter;
			elem1->clientNum = clientIndex;
			elem1->elem.type = 1;
			elem1->elem.text = G_LocalizedStringIndex(Text);
			elem1->elem.font = 5;
			elem1->elem.fontScale = 3;
			elem1->elem.alignOrg = 5; elem1->elem.alignScreen = 1; elem1->elem.x = 325; elem1->elem.y = 130; 
			elem1->elem.color.r = R;
			elem1->elem.color.g = G;
			elem1->elem.color.b = B;
			elem1->elem.color.a = A;
			elem1->elem.glowColor.r = GlowR;
			elem1->elem.glowColor.g = GlowG;
			elem1->elem.glowColor.b = GlowB;
			elem1->elem.glowColor.a = GlowA;
			elem1->elem.flags = WriteType;
			int * levelTime = (int*)0x0012E0304;
			elem1->elem.fxBirthTime = *levelTime;
			elem1->elem.fxLetterTime = Lettertime;
			elem1->elem.fxDecayStartTime = fxDecayStartTime;
			elem1->elem.fxDecayDuration = fxDecayDuration;
			game_hudelem_s* elem = typewriter1;
			elem->clientNum = clientIndex;
			elem->elem.type = 1;
			elem->elem.text = G_LocalizedStringIndex(Text2);
			elem->elem.font = 5;
			elem->elem.fontScale = 2.5;
			elem->elem.alignOrg = 5; elem->elem.alignScreen = 1; elem->elem.x = 325; elem->elem.y = 165; 
			elem->elem.color.r = R;
			elem->elem.color.g = G;
			elem->elem.color.b = B;
			elem->elem.color.a = A;
			elem->elem.glowColor.r = GlowR;
			elem->elem.glowColor.g = GlowG;
			elem->elem.glowColor.b = GlowB;
			elem->elem.glowColor.a = GlowA;
			elem->elem.flags = WriteType;
			elem->elem.fxBirthTime = *levelTime;
			elem->elem.fxLetterTime = Lettertime;
			elem->elem.fxDecayStartTime = fxDecayStartTime;
			elem->elem.fxDecayDuration = fxDecayDuration;
		}
	}

	    game_hudelem_s* typewriter2;


	}
}