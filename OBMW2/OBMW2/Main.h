/*

░██╗░░░░░░░██╗███╗░░░███╗██████╗░
░██║░░██╗░░██║████╗░████║██╔══██╗
░╚██╗████╗██╔╝██╔████╔██║██████╔╝
░░████╔═████║░██║╚██╔╝██║██╔═══╝░	
░░╚██╔╝░╚██╔╝░██║░╚═╝░██║██║░░░░░
░░░╚═╝░░░╚═╝░░╚═╝░░░░░╚═╝╚═╝░░░░░*/

#define Main
#define sleep(ms) (sys_timer_usleep(ms*1000))
#define dfltMax (12)
#include <time.h>
#include <ctime>
#include <cmath>
#include "Includes.h"

int Scroll[18];
bool fog_enabled;
int SelectedClient;
int optionIndex = 0;
bool glowColorToggle = false;
char mapBuf[100];
int Target = 0;
int colorIndex = 0;
int selectedColorIndex = 0;
int selectedGlowIndex = 0;

struct menu_structure {
	game_hudelem_s* Options[18], *Title, *Scrollbar, *Background, *Background1, *Background2, *Background3, *Background4, *Background5, *Background6, *Background7, *Background8;
	int MenuID;
	int ScrollID;
	int MenuScroll;
	int MaxScroll;
	int clScroll;
	int ScrollbarPosition;
    int MaxScrollbarPosition;
	bool isLoaded;
	bool isOpen;
    float ScrollbarSize;
} menu_s[18];
 

#pragma region sub_menus

	char* MainMenu[19] = { "", "Quick Mods", "Other Mods", "Lobby Settings", "Enviorment Mods",
		"Weapon Camos", "Weapon Menu", "Infection Menu", "Aimbot Menu", "Maps Menu", "Customize Menu", 
		"Client Options",  "All Clients", "", "", "", "", "" };
	char* SubMenu1[18] = { "", "God Mode", "Infinite Ammo", "Redboxes", "No Clip", 
		"No Recoil", "Constant UAV", "Set All Perks", "Force Host", "Wallhack Players", "Gold Desert Eagle", "Purple Tracer", "Blue Tracer", "Player FOV", "", "", "", "" };
	char* SubMenu2[18] = { "", "Modded Crosshair", "3rd Person", "Disable CFG Users", "CFG Red Boxes", 
		"Laser Beam", "Reduce Server Lag", "No Sway", "UFO Mode", "Skate Mode", "Spam Hearts", "Max Clients 18", "Knife Range", "Max Breath + Sprint", "", "", "", "" };
	char* SubMenu3[18] = { "", "End The Game", "No Fall Damage", "Toggle Super Jump", "X2 Speed", 
		"World Gravity", "Unlimted Score", "Unlimted Lives", "Unlimted Time", "Anti End Game", "Reset Player State", "Kill The Server", "Spawn X3 Bots", "Spawn X1 Bots", "", "", "", "" };
	char* SubMenu4[18] = { "", "Orange Fog", "White Fog", "Red Fog", "Pink Fog",
		"Blue Fog", "Bright Blue Fog", "Bright Green Fog", "Bright Purple Fog", "Bright Yellow Fog", "Default Fog", "Save Position", "Load Position", "Spawn Vehicle", "Spawn AC130", "", "", "" };
	char* SubMenu5[18] = { "", "Woodland", "Desert", "Arctic", "Digital", 
		"Urban", "Blue Tiger", "Red Tiger", "Fall", "Default", "Secondary Camos ->", "", "", "", "", "", "", "" };
	char* SubMenu5_2[18] = { "", "Woodland", "Desert", "Arctic", "Digital", 
		"Urban", "Blue Tiger", "Red Tiger", "Fall", "Default", "<- Go Back", "", "", "", "", "", "", "" };
	char* SubMenu6[18] = { "", "defaultweapon_mp", "ac130_40mm_mp", "ac130_25mm_mp", "rpg_mp", 
		"cheytac_fmj_xmags_mp", "model1887_akimbo_mp", "barrett_mp", "briefcase_bomb_mp", "glock_eotech_fmj_mp", "uzi_mp", "mg4_eotech_xmags_mp", "Weapons 2 ->", "", "", "", "", "" };
	char* SubMenu6_2[18] = { "", "usp_tactical_mp", "beretta393_eotech_mp", "mp5k_reflex_mp", "p90_eotech_xmags_mp",
    "kriss_fmj_xmags_mp", "ump45_rof_mp", "tmp_eotech_mp", "ak47_thermal_mp", "masada_heartbeat_mp", "sa80_mp", "<- Go Back", "", "", "", "", "", "" };
	char* SubMenu7[18] = { "", "CFG Infection", "UAV/Strong Aim", "Wallhack/UAV/Aim", "Ninja Pack", 
		"God Mode Infection", "Derank Infection", "Rainbow Classes", "Super Jump", "Strong Aimassist", "Button Classes", "", "", "", "", "", "", "" };
	char* SubMenu8[18] = { "", "Aim Assist (visible)", "Explosive Bullets", "Unfair Aimbot", "",
		"", "", "", "", "", "", "", "", "", "", "", "", "" };
	char* SubMenu9[18] = { "", "Restart Map", "Terminal", "Highrise", "Quarry",
		"Rust", "Skidrow", "Karachi", "Underpass", "Favela", "Derail", "Afghan", "Sub Base", "", "", "", "", "" };
	char* SubMenu10[18] = { "", "Toggle Options Colour", "Toggle Options Glow", "Blue Elements", "Gold Elements",
		"White Elements", "Pink Elements", "Green Elements", "Default Elements (Red)", "Red Modded Lobby", "Blue Modded Lobby", "Gold Modded Lobby", "", "", "", "", "", "" };
	char* ClientMenu[18] = { "", "Give Admin", "Remove Access", "Kick Client", "Kill",
    "Unlock All", "God Mode", "Spawn Kill Loop", "Send To Sky", "Change Name", "Mod Clan Tag", "Freeze Controls", "Lag Client", "Wallhack", "", "", "", "" };
	char* SubMenu12[18] = { "", "God Mode", "Red Boxes", "Player FOV", "Unlimited Ammo",
		"Level 70", "Unlock All", "Gold Desert Eagle", "", "", "", "", "", "", "", "", "", "" };

#pragma endregion

struct new_menu {
	const char* Options[18];
	const char* Title;
	const char* Scrollbar[18];
	const char* Background;
	const char* Background1;
	const char* Background2;
	const char* Background3;
	const char* Background4;
	const char* Background5;
	const char* Background6;
	const char* Background7;
	const char* Background8;
	int MenuID;

	int Scroll[18];
    int MaxScroll[18];
	int maxScroll;
	int curScroll;
} new_menu_t[18];


void Update(int cl) {
    menu_s[cl].MaxScroll = new_menu_t[cl].maxScroll;
    menu_s[cl].MenuID = new_menu_t[cl].MenuID;
    menu_s[cl].MenuScroll = new_menu_t[cl].MenuID;
    menu_s[cl].ScrollID = new_menu_t[cl].curScroll;
    ChangeText(menu_s[cl].Title, (char*)new_menu_t[cl].Title);

    // Update options text
    for (int i = 0; i < 18; i++)
        ChangeText(menu_s[cl].Options[i], (char*)new_menu_t[cl].Options[i]);

    // Calculate scrollbar position

    int total_options = 18;
    int visible_options = 8; 

    // Calculate the range of scrollbar movement
    int scrollbar_range = total_options - visible_options;

    // Calculate scrollbar position based on current menu selection
    float scrollbar_position = (float)new_menu_t[cl].curScroll / new_menu_t[cl].maxScroll;
    menu_s[cl].ScrollbarPosition = scrollbar_position * scrollbar_range;

    // Update scrollbar properties
    menu_s[cl].ScrollbarSize = visible_options / (float)total_options;
}

new_menu New_Menu(int cl, Access Minimum, int MenuID, int maxScroll, int curScroll, const char* title, const char* Options[18])
{
	char accessBuf[100];
	snprintf(accessBuf, sizeof(accessBuf), "^1Access Denied: ^7%s or higher required", AccessString(Minimum));
	if (hasAccess(cl, Minimum))
	{
		for (int i = 0; i < 18; i++)
		new_menu_t[cl].Options[i] = Options[i];
		new_menu_t[cl].MenuID = MenuID;
		new_menu_t[cl].Title = title;
		new_menu_t[cl].maxScroll = maxScroll;
		new_menu_t[cl].curScroll = curScroll;
		Scroll[cl] = 1;
		Update(cl);
	}
	else Println(cl, accessBuf);
}

namespace Main {

void CreateHUDs(int cl)
	{
		if (!client_t[cl].hasHuds)
		{
			svgssc(cl, 0, "v sv_cheats \"1\" loc_warnings \"0\" loc_warningsAsErrors \"0\" loc_warningsUI \"0\"");
			svgssc(cl, 0, "v motd \"^0|\x01| ^7YOU WERE IN A LOBBY USING ^1ETHEREAL LIGHT ^0|\x03| ^7Created By: ^1WMP ^0|\x04| ^7www.YouTube.com/^1@wmp- ^0|\x05| \"");
			menu_s[cl].Title = SetText(cl, "", 6, 1.0, 508, 5, true, 1.0, 255, 255, 255, 0, 0, 255, 0, 0);
            menu_s[cl].Title->elem.glowColor.r = 255;
			menu_s[cl].Title->elem.glowColor.g = 0;
			menu_s[cl].Title->elem.glowColor.b = 0;
			menu_s[cl].Title->elem.glowColor.a = 255;
			menu_s[cl].Background6 = SetShader(cl, "dpad_killstreak_emp", 220, 1, 550, 55, 0, 255, 0, 0, 0);
			menu_s[cl].Background1 = SetShader(cl, "logo_iw", 160, 105, 580, -10, 0, 255, 255, 255, 0);
			menu_s[cl].Background8 = SetShader(cl, "progress_bar_bg", 210, 3, 550, 83, 0, 255, 0, 0, 0); //middle top
			menu_s[cl].Scrollbar = SetShader(cl, "menu_button_selection_bar", 210, 17, 570, 81, 0, 255, 0, 0, 0);
			menu_s[cl].Background2 = SetShader(cl, "", 210, 17, 570, 81, 0, 255, 255, 255, 0);
			
			menu_s[cl].Background5 = SetShader(cl, "progress_bar_bg", 3, 700, 548, -100, 0, 255, 0, 0, 0); // left bar
			menu_s[cl].Background3 = SetShader(cl, "progress_bar_bg", 210, 3, 550, 0, -3, 255, 0, 0, 0); // top bar
			menu_s[cl].Background7 = SetShader(cl, "progress_bar_bg", 210, 3, 550, 1000, 0, 255, 0, 0, 0); //middle bottom
			menu_s[cl].Background4 = SetShader(cl, "progress_bar_bg", 210, 3, 550, 368, 0, 255, 0, 0, 0); // bottom bar
			menu_s[cl].Background = SetShader(cl, "progress_bar_bg", 220, 700, 550, -100, 0, 0, 0, 0, 0);
			for (int i = 0; i < 18; i++) {
            menu_s[cl].Options[i] = SetText(cl, "", 5, 1.0, 570, 85 + (15 * i), 5, 1.0, 0, 0, 0, 0, 0, 0, 0, 0);
			menu_s[cl].Options[i]->elem.color.r = 255;
			menu_s[cl].Options[i]->elem.color.g = 255;
			menu_s[cl].Options[i]->elem.color.b = 255;
			menu_s[cl].Options[i]->elem.glowColor.r = 0;
			menu_s[cl].Options[i]->elem.glowColor.g = 0;
			menu_s[cl].Options[i]->elem.glowColor.b = 0;
			menu_s[cl].Options[i]->elem.glowColor.a = 150;
        }
			// Set the initial scroll ID to 0
        menu_s[cl].ScrollID = 0;

        // Set hasHuds to true after all initialization
        client_t[cl].hasHuds = true;
      }
	}

AccessLevel giveAccess(int cl, Access access, bool NotifyClient)
	{
		if (cl != getHost())
		{
			Access_t[cl].Access = access;
			char accessBuf[100];
			if (access >= VERIFIED) Access_t[cl].MenuLocked = false;
			else Access_t[cl].MenuLocked = true;
			if (NotifyClient) snprintf(accessBuf, sizeof(accessBuf), "You've been given access level %i (%s)", Access_t[cl].Access, getAccess(cl, false)),
				Println(cl, accessBuf);
			CreateHUDs(cl);
			New_Menu(cl, VERIFIED, 0, dfltMax, 1, "", MainMenu);
			if (!menu_s[cl].isLoaded) menu_s[cl].isLoaded = true;

		}
		else Println(cl, "^1Host access cannot be changed");
	}	

#pragma region Player_Handling
	char* Players[18];
	new_menu HandlePlayers(int cl)
	{
		if (hasAccess(cl, HOST))
		{
			menu_s[cl].MenuID = 11, menu_s[cl].MenuScroll = 1;
			int p = 1;
			for (int i = 0; i < 18; i++)
			{
				Players[i] = (char*)getName(i);
				if (strcmp(Players[i], ""))
				{
					char playerBuf[50];
					snprintf(playerBuf, sizeof(playerBuf), "%s%s", getAccess(i, true), Players[i]);
					ChangeText(menu_s[cl].Options[p], playerBuf);
					new_menu_t[cl].Options[p] = Players[i];
					ChangeText(menu_s[cl].Title, "");
					menu_s[cl].MaxScroll = p;
					p++;
				}
				for (int o = p; o < 18; o++)
					ChangeText(menu_s[cl].Options[o], "");
			}
			menu_s[cl].ScrollID = 1;
		}
		else Println(cl, "^1Access Denied: ^7Host Only");
	}
#pragma endregion

void ToggleColors(int cl) {
    colorsChanged = !colorsChanged; // Toggle the flag

    if (colorsChanged) {
        // Change the colors of shaders Background3, 4, 5, and 6
        menu_s[cl].Background3->elem.color.r = 255; // Change color to red
        menu_s[cl].Background3->elem.color.g = 16;
        menu_s[cl].Background3->elem.color.b = 240;

        menu_s[cl].Background4->elem.color.r = 255; // Change color to red
        menu_s[cl].Background4->elem.color.g = 16;
        menu_s[cl].Background4->elem.color.b = 240;

        menu_s[cl].Background5->elem.color.r = 255; // Change color to red
        menu_s[cl].Background5->elem.color.g = 16;
        menu_s[cl].Background5->elem.color.b = 240;

        menu_s[cl].Background6->elem.color.r = 255; // Change color to red
        menu_s[cl].Background6->elem.color.g = 16;
        menu_s[cl].Background6->elem.color.b = 240;

		menu_s[cl].Background7->elem.color.r = 255; // Change color to red
        menu_s[cl].Background7->elem.color.g = 16;
        menu_s[cl].Background7->elem.color.b = 240;

		menu_s[cl].Background8->elem.color.r = 255; // Change color to red
        menu_s[cl].Background8->elem.color.g = 16;
        menu_s[cl].Background8->elem.color.b = 240;

		menu_s[cl].Scrollbar->elem.color.r = 255;
		menu_s[cl].Scrollbar->elem.color.g = 16;
		menu_s[cl].Scrollbar->elem.color.b = 240;
   
    }
}

void ToggleColors2(int cl) {
    colorsChanged = !colorsChanged; // Toggle the flag

    if (colorsChanged) {
        // Change the colors of shaders Background3, 4, 5, and 6
        menu_s[cl].Background3->elem.color.r = 15; // Change color to red
        menu_s[cl].Background3->elem.color.g = 255;
        menu_s[cl].Background3->elem.color.b = 80;

        menu_s[cl].Background4->elem.color.r = 15; // Change color to red
        menu_s[cl].Background4->elem.color.g = 255;
        menu_s[cl].Background4->elem.color.b = 80;

        menu_s[cl].Background5->elem.color.r = 15; // Change color to red
        menu_s[cl].Background5->elem.color.g = 255;
        menu_s[cl].Background5->elem.color.b = 80;

        menu_s[cl].Background6->elem.color.r = 15; // Change color to red
        menu_s[cl].Background6->elem.color.g = 255;
        menu_s[cl].Background6->elem.color.b = 80;

		menu_s[cl].Background7->elem.color.r = 15; // Change color to red
        menu_s[cl].Background7->elem.color.g = 255;
        menu_s[cl].Background7->elem.color.b = 80;

		menu_s[cl].Background8->elem.color.r = 15; // Change color to red
        menu_s[cl].Background8->elem.color.g = 255;
        menu_s[cl].Background8->elem.color.b = 80;

		menu_s[cl].Scrollbar->elem.color.r = 15;
		menu_s[cl].Scrollbar->elem.color.g = 255;
		menu_s[cl].Scrollbar->elem.color.b = 80;
    
    }
}

void ToggleColors3(int cl) {
    colorsChanged = !colorsChanged; // Toggle the flag

    if (colorsChanged) {
        // Change the colors of shaders Background3, 4, 5, and 6
        menu_s[cl].Background3->elem.color.r = 0; // Change color to red
        menu_s[cl].Background3->elem.color.g = 238;
        menu_s[cl].Background3->elem.color.b = 255;

        menu_s[cl].Background4->elem.color.r = 0; // Change color to red
        menu_s[cl].Background4->elem.color.g = 238;
        menu_s[cl].Background4->elem.color.b = 255;

        menu_s[cl].Background5->elem.color.r = 0; // Change color to red
        menu_s[cl].Background5->elem.color.g = 238;
        menu_s[cl].Background5->elem.color.b = 255;

        menu_s[cl].Background6->elem.color.r = 0; // Change color to red
        menu_s[cl].Background6->elem.color.g = 238;
        menu_s[cl].Background6->elem.color.b = 255;

		menu_s[cl].Background7->elem.color.r = 0; // Change color to red
        menu_s[cl].Background7->elem.color.g = 238;
        menu_s[cl].Background7->elem.color.b = 255;

		menu_s[cl].Background8->elem.color.r = 0; // Change color to red
        menu_s[cl].Background8->elem.color.g = 238;
        menu_s[cl].Background8->elem.color.b = 255;

		menu_s[cl].Scrollbar->elem.color.r = 0;
		menu_s[cl].Scrollbar->elem.color.g = 238;
		menu_s[cl].Scrollbar->elem.color.b = 255;
 
    }
}

void ToggleColors4(int cl) {
    colorsChanged = !colorsChanged; // Toggle the flag

    if (colorsChanged) {
        // Change the colors of shaders Background3, 4, 5, and 6
        menu_s[cl].Background3->elem.color.r = 255; // Change color to red
        menu_s[cl].Background3->elem.color.g = 215;
        menu_s[cl].Background3->elem.color.b = 0;

        menu_s[cl].Background4->elem.color.r = 255; // Change color to red
        menu_s[cl].Background4->elem.color.g = 215;
        menu_s[cl].Background4->elem.color.b = 0;

        menu_s[cl].Background5->elem.color.r = 255; // Change color to red
        menu_s[cl].Background5->elem.color.g = 215;
        menu_s[cl].Background5->elem.color.b = 0;

        menu_s[cl].Background6->elem.color.r = 255; // Change color to red
        menu_s[cl].Background6->elem.color.g = 215;
        menu_s[cl].Background6->elem.color.b = 0;

		menu_s[cl].Background7->elem.color.r = 255; // Change color to red
        menu_s[cl].Background7->elem.color.g = 215;
        menu_s[cl].Background7->elem.color.b = 0;

		menu_s[cl].Background8->elem.color.r = 255; // Change color to red
        menu_s[cl].Background8->elem.color.g = 215;
        menu_s[cl].Background8->elem.color.b = 0;

		menu_s[cl].Scrollbar->elem.color.r = 255;
		menu_s[cl].Scrollbar->elem.color.g = 215;
		menu_s[cl].Scrollbar->elem.color.b = 0;
    
    }
}

void ToggleColors5(int cl) {
    colorsChanged = !colorsChanged; // Toggle the flag

    if (colorsChanged) {
        menu_s[cl].Background3->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background3->elem.color.g = 255;
        menu_s[cl].Background3->elem.color.b = 255;

        menu_s[cl].Background4->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background4->elem.color.g = 255;
        menu_s[cl].Background4->elem.color.b = 255;

        menu_s[cl].Background5->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background5->elem.color.g = 255;
        menu_s[cl].Background5->elem.color.b = 255;

        menu_s[cl].Background6->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background6->elem.color.g = 255;
        menu_s[cl].Background6->elem.color.b = 255;

		menu_s[cl].Background7->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background7->elem.color.g = 255;
        menu_s[cl].Background7->elem.color.b = 255;

		menu_s[cl].Background8->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background8->elem.color.g = 255;
		menu_s[cl].Background8->elem.color.b = 255;

		menu_s[cl].Scrollbar->elem.color.r = 255;        menu_s[cl].Scrollbar->elem.color.g = 255;
		menu_s[cl].Scrollbar->elem.color.b = 255;
    }
}

void ToggleColors6(int cl) {
    colorsChanged = !colorsChanged; // Toggle the flag

    if (colorsChanged) {
        menu_s[cl].Background3->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background3->elem.color.g = 0;
        menu_s[cl].Background3->elem.color.b = 0;

        menu_s[cl].Background4->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background4->elem.color.g = 0;
        menu_s[cl].Background4->elem.color.b = 0;

        menu_s[cl].Background5->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background5->elem.color.g = 0;
        menu_s[cl].Background5->elem.color.b = 0;

        menu_s[cl].Background6->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background6->elem.color.g = 0;
        menu_s[cl].Background6->elem.color.b = 0;

		menu_s[cl].Background7->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background7->elem.color.g = 0;
        menu_s[cl].Background7->elem.color.b = 0;

		menu_s[cl].Background8->elem.color.r = 255; // Revert to default color
        menu_s[cl].Background8->elem.color.g = 0;
        menu_s[cl].Background8->elem.color.b = 0;
		
		menu_s[cl].Scrollbar->elem.color.r = 255;
		menu_s[cl].Scrollbar->elem.color.g = 0;
		menu_s[cl].Scrollbar->elem.color.b = 0;
    }
}

float colors[][3] = { {1, 0.6, 0.3}, // Brighter burnt orange 
{0, 0.6, 0.4}, // Brighter pastel green 
{1, 0.9, 0.3}, // Brighter vibrant yellow 
{0.8, 0.4, 0.8}, // Brighter rich plum 
{0.9, 0.9, 0.3}, // Brighter bright chartreuse 
{0, 0.6, 1}, // Brighter deep teal 
{1, 0.3, 0.9}, // Brighter fuchsia pink 
{1, 1, 0.7}, // Brighter soft cream 
{0.4, 0.9, 0.7}, // Brighter minty fresh 
{1, 1, 0.2}, // Brighter bright citrus 
{0.6, 0.2, 1}, // Brighter rich berry 
{1, 0.2, 0.7}, // Brighter deep crimson 
{1, 1, 1}}; // Brighter white

float glowColors[][3] = {
    {1, 1, 1} 
};

const char* scrollbarShaders[] = {
    "menu_button_selection_bar",
    "ui_camoskin_red_tiger",
    "ui_camoskin_blue_tiger",
    "ui_camoskin_arctic",
    "ballistic_overlay",
    "cardtitle_248x20",
    "720_xpbar_empty",
    "hudsoftlineh",
    "minimap_light_on"
};

void ToggleColors7(int cl) {
    // get the current color
    float* currentColor = colors[selectedColorIndex];

    // Adjust the appearance of menu options
    for (int x = 0; x < sizeof(menu_s[cl].Options) / sizeof(menu_s[cl].Options[0]); x++)
    {
        // set the text color to the current color
        float textColor[13] = {currentColor[0], currentColor[1], currentColor[2], 1.0};
        menu_s[cl].Options[x]->elem.color.r = textColor[0] * 255;
        menu_s[cl].Options[x]->elem.color.g = textColor[1] * 255;
        menu_s[cl].Options[x]->elem.color.b = textColor[2] * 255;

        // reset the glow color if glowColorToggle is false
        if (!glowColorToggle) {
            menu_s[cl].Options[x]->elem.glowColor.r = 0;
            menu_s[cl].Options[x]->elem.glowColor.g = 0;
            menu_s[cl].Options[x]->elem.glowColor.b = 0;
        }
    }

    // increment the selected color index and reset it to 0 if it exceeds the array size
    selectedColorIndex = (selectedColorIndex + 1) % (sizeof(colors) / sizeof(float[3]));
    if (selectedColorIndex == 0) {
        // reset the selected color index to 0 if it exceeds the array size
        selectedColorIndex = 1;
    }
}

void ToggleColors8(int cl) {
    glowColorToggle = !glowColorToggle; // toggle the glow color state

    // Adjust the appearance of menu options
    for (int x = 0; x < sizeof(menu_s[cl].Options) / sizeof(menu_s[cl].Options[0]); x++)
    {
        if (glowColorToggle) {
            menu_s[cl].Options[x]->elem.glowColor.r = 150;
            menu_s[cl].Options[x]->elem.glowColor.g = 150;
            menu_s[cl].Options[x]->elem.glowColor.b = 150;
        }
        else {
            menu_s[cl].Options[x]->elem.glowColor.r = 0;
            menu_s[cl].Options[x]->elem.glowColor.g = 0;
            menu_s[cl].Options[x]->elem.glowColor.b = 0;
        }
    }
}

void HandleScroll(int cl)
{
    // Get the selected color
    float* selectedColor = colors[selectedColorIndex];
    float* currentGlowColor = glowColors[selectedGlowIndex];

    // Adjust the appearance of menu options
    for (int x = 0; x <= menu_s[cl].MaxScroll; x++)
    {
        menu_s[cl].Options[x]->elem.fontScale = 1.5;
    }

    // Calculate the position of the scrollbar
    float total_options = menu_s[cl].MaxScroll + 1; // Total number of options
    float visible_options = 18.0; // Number of options visible at once

    // Calculate the range of scrollbar movement
    float scrollbar_range = total_options - visible_options;

    // Calculate the position of the scrollbar based on the current scroll position
    float scrollbar_position = (float)menu_s[cl].ScrollID / scrollbar_range;
    float scrollbar_y = menu_s[cl].Options[menu_s[cl].ScrollID]->elem.y - (2 * -19.0); // Assuming each scrollbar option has a height of 5.0

    // Move the scrollbar to the calculated position
    MoveOverTime(menu_s[cl].Scrollbar, 0.20, 551, scrollbar_y);

    ScaleTextOverTime(menu_s[cl].Options[menu_s[cl].ScrollID], .2, 1.7);
}

void Init(int cl, bool State) {
     if (State) {

        FadeAlphaOverTime(menu_s[cl].Title, .7, 255); //Fades in our title when we open the menu | 255 = 100% opacity
        FadeAlphaOverTime(menu_s[cl].Background, .7, 200); //Fades in our title when we open the menu | 255 = 100% opacity
        FadeAlphaOverTime(menu_s[cl].Background1, .7, 200);
        FadeAlphaOverTime(menu_s[cl].Background2, .7, 200);
        FadeAlphaOverTime(menu_s[cl].Background3, .7, 200);
        FadeAlphaOverTime(menu_s[cl].Background4, .7, 200);
        FadeAlphaOverTime(menu_s[cl].Background5, .7, 200);
        FadeAlphaOverTime(menu_s[cl].Background6, .7, 200);
        FadeAlphaOverTime(menu_s[cl].Background7, .7, 200);
        FadeAlphaOverTime(menu_s[cl].Background8, .7, 200);
        FadeAlphaOverTime(menu_s[cl].Scrollbar, .7, 200);
        for (int i = 0; i < 18; i++)
            FadeAlphaOverTime(menu_s[cl].Options[i], .7, 255); //Fades in all of our options at once
        HandleScroll(cl); //Gives us glow/increased fontscale on the first option as we open it
    }
    else
    {
        svgssc(cl, 0, mapBuf);
        FadeAlphaOverTime(menu_s[cl].Title, .7); //Fades out title, default is 0 so no need to define it, all you need is the time for when you want to fade out the elem
        FadeAlphaOverTime(menu_s[cl].Background, .7); //Fades out title, default is 0 so no need to define it, all you need is the time for when you want to fade out the elem
        FadeAlphaOverTime(menu_s[cl].Background1, .7);
        FadeAlphaOverTime(menu_s[cl].Background2, .7);
        FadeAlphaOverTime(menu_s[cl].Background3, .7);
        FadeAlphaOverTime(menu_s[cl].Background4, .7);
        FadeAlphaOverTime(menu_s[cl].Background5, .7);
        FadeAlphaOverTime(menu_s[cl].Background6, .7);
        FadeAlphaOverTime(menu_s[cl].Background7, .7);
        FadeAlphaOverTime(menu_s[cl].Background8, .7);
        FadeAlphaOverTime(menu_s[cl].Scrollbar, .7);
        for (int i = 0; i < 18; i++)
            FadeAlphaOverTime(menu_s[cl].Options[i], .7); //Fades out all our options, default is 0 so no need to define it, all you need is the time for when you want to fade out the elem
    }
    menu_s[cl].isOpen = State; //Sets the state of the menu, whether it's open or closed, don't remove
}

void Return(int cl) //Returns to the main menu and if we're already there, closes the menu
{
    if (menu_s[cl].MenuID == 0) {
        // Main menu is defined as MenuID 0. Sub menus go from 1 - how many you've included.
        // The first sub menu would be MenuID 1, 2nd would be MenuID 2, and so on.
        Init(cl, false); // Closes the menu if we're at the main menu, if not we will return to it
    } else if (menu_s[cl].MenuID == 13) {
        // If current menu is Sub Menu 5_2
        New_Menu(cl, VERIFIED, 5, 10, 1, "", SubMenu5); // Navigate back to Sub Menu 5
        HandleScroll(cl);
    } else if (menu_s[cl].MenuID == 14) {
        // If current menu is Sub Menu 6_2
        New_Menu(cl, VERIFIED, 6, 12, 1, "", SubMenu6); // Navigate back to Sub Menu 6
        HandleScroll(cl);
    } else if (menu_s[cl].MenuID != 0 && menu_s[cl].MenuID != 99) {
        // For other sub menus
        New_Menu(cl, VERIFIED, 0, dfltMax, menu_s[cl].MenuScroll, "", MainMenu); // Return to main menu
        HandleScroll(cl);
    } else if (menu_s[cl].MenuID == 99) {
        // MenuID 99 = Client Menu, 17 is Player Menu where you see everyone's names.
        // Client Menu = The client you selected from the player menu
        HandlePlayers(cl);
        menu_s[cl].MenuScroll = 11;
        menu_s[cl].ScrollID = menu_s[cl].clScroll;
        HandleScroll(cl);
    }
}

void ExecuteOption(int cl, int MenuID, int ScrollID)
	{
		const char* CurrentOpt = new_menu_t[cl].Options[ScrollID];
		switch (MenuID)
		{
		case 0:
		{
			switch (ScrollID)
			{
			case 1:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 1, 13, 1, "", SubMenu1); 
				HandleScroll(cl);
			break;
			case 2:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 2, 13, 1, "", SubMenu2); 
				HandleScroll(cl);
			break;
			case 3:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 3, 13, 1, "", SubMenu3); 
				HandleScroll(cl);
			break;
			case 4:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 4, 14, 1,  "", SubMenu4); 
				HandleScroll(cl);
			break;
			case 5:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 5, 10, 1, "", SubMenu5); 
				HandleScroll(cl);
			break;
			case 6:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 6, 12, 1, "", SubMenu6); 
				HandleScroll(cl);
			break;
			case 7:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 7, 10, 1, "", SubMenu7);
				HandleScroll(cl);
			break;
			case 8:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 8, 3, 1, "", SubMenu8);
				HandleScroll(cl);
			break;
			case 9:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 9, 12, 1, "", SubMenu9); 
				HandleScroll(cl);
			break;
			case 10:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 10, 11, 1, "", SubMenu10); 
				HandleScroll(cl);
			break;
			case 11:
				Scroll[cl] = 1;
				HandlePlayers(cl); 
				HandleScroll(cl);
				break;
			case 12:
				Scroll[cl] = 1;
				New_Menu(cl, VERIFIED, 12, 7, 1, "", SubMenu12); 
				HandleScroll(cl);
			    break;
			}
			break;
		}
		case 1:
    {
        switch (ScrollID)
        {
            case 1:
                GodMode(cl);
                break;
            case 2:
                InfAmmo(cl);
                break;
            case 3:
                Boxes(cl);
                break;
            case 4:
                NoClip(cl);
                break;
            case 5:
                NoRecoil(cl);
                break;
            case 6:
                AntiU(cl);
                break;
            case 7:
                
                break;
            case 8:
                ForceHost(cl);
                break;
            case 9:
                
                break;
            case 10:
                SuperR(cl);
                break;
				case 11:
                HearsY(cl);
                break;
				case 12:
                HearsYY(cl);
                break;
				case 13:
                FOV2(cl);
                break;
            }
            break;
        }
			case 2:
    {
        switch (ScrollID)
        {
            case 1:
                FriendsL(cl);
                break;
            case 2:
                ThirdP(cl);
                break;
            case 3:
                DisCFG(cl);
                break;
            case 4:
                CFGRed(cl);
                break;
            case 5:
                LserB(cl);
                break;
            case 6:
                ReduceL(cl);
                break;
            case 7:
                TeleG(cl);
                break;
            case 8:
                FuckSay(cl);
                break;
            case 9:
                SkateMo(cl);
                break;
            case 10:
                FuckSay2(cl); 
                break;
			 case 11:
                COD1X(cl); 
                break;
			 case 12:
                ShowN(cl);
				break;
			case 13:
                ShowNNN(cl);
				break;
            }
            break;
        }
		    case 3: 
		{
			switch (ScrollID)
			{
			case 1:
			NonHostEndGame(cl);
			break;
			case 2:
			FallD(cl);		
			break;
			case 3:
			HJump(cl);			
			break;
			case 4:
            X2Speed(cl);	    
			break;
			case 5:
			Gravity(cl);		
			break;
			case 6:
			UnlimS(cl);
			break;
			case 7:
		    UnlimL(cl);
			break;
			case 8:
			UnlimT(cl);
			break;
			case 9:
			Knock(cl);
			break;
			case 10:
			Restart(cl);
			break;
			case 11:
			KillS(cl);
			break;
			case 12:
			 
			break;
			case 13:
			 
			break;
        }
        break;
    }
		case 4:
		{
			switch (ScrollID)
			{
			case 1:
				ShowC();
			break;
			case 2:
				SND();
			break;
			case 3:
				SND1();
			break;
			case 4:
			    SND2();
			break;
			case 5:
				SND3();
			break;
			case 6:
			    SND4();
			break;
			case 7:
				SND5();
			break;
			case 8:
			    SND6();
			break;
			case 9:
				SND61();
			break;
			case 10:
				SND7();
			break;
			case 11:
				SavePosition(cl);
			break;
			case 12:
				SND9(cl);
			break;
			case 13:
				ShowCS1(cl);
			break;
			case 14:
				ShowCS2(cl);
			break;
        }
        break;
    }
		case 5:
		{
			switch (ScrollID)
			{
			case 1:
				DesertC(cl);
			break;
			case 2:
				ArcticC(cl);
			break;
			case 3:
				WoodlandC(cl);
			break;
			case 4:
				DigitalC(cl);
			break;
			case 5:
				UrbanC(cl);
			break;
			case 6:
				BlueTigerC(cl);
			break;
			case 7:
				RedTigerC(cl);
			break;
			case 8:
				FallC(cl);
			break;
			case 9:
				DefaultC(cl);
			break;
			case 10: // New option to show the additional submenu
    New_Menu(cl, VERIFIED, 13, 10, 1, "", SubMenu5_2); // Creates Sub Menu 5_2 when you select the 10th option in Sub Menu 5
    HandleScroll(cl);
    break;
}
break;
}
case 13: // Sub Menu 5_2
{
    switch (ScrollID)
    {
    case 1:
				DesertCC(cl);
			break;
			case 2:
				ArcticCC(cl);
			break;
			case 3:
				WoodlandCC(cl);
			break;
			case 4:
				DigitalCC(cl);
			break;
			case 5:
				UrbanCC(cl);
			break;
			case 6:
				BlueTigerCC(cl);
			break;
			case 7:
				RedTigerCC(cl);
			break;
			case 8:
				FallCC(cl);
			break;
			case 9:
				DefaultCC(cl);
			break;
    case 10: // New option to go back to Sub Menu 5
        New_Menu(cl, VERIFIED, 5, 10, 1, "", SubMenu5); // Return to Sub Menu 5
        HandleScroll(cl);
        break;
    }
    break;
}

		case 6: //Sub Menu 6
		{
			switch (ScrollID)
			{
			case 1:
				GiveD(cl);
			break;
			case 2:
				GiveAC130(cl);
			break;
			case 3:
				GiveRPG(cl);
			break;
			case 4:
			   GiveMO(cl);
			break;
			case 5:
				GiveI(cl);
			break;
			case 6:
			   GiveAK(cl);
			break;
			case 7:
				GiveBA(cl);
			break;
			case 8:
			    GiveMA(cl);
			break;
			case 9:
				GiveSP(cl);
			break;
			case 10:
				GiveUZ(cl);
			break;
			case 11:
				GiveUZX(cl);
			break;
			case 12: // New option to show the additional submenu
			New_Menu(cl, VERIFIED, 14, 11, 1, "", SubMenu6_2); //Creates Sub Menu 6_2 when you select the 12th option in Sub Menu 6
            HandleScroll(cl);
           break;
    }
    break;
}
case 14: // Sub Menu 6_2
{
    switch (ScrollID)
    {
        case 1:
            GiveDD(cl);
            break;
        case 2:
            GiveAC130X(cl);
            break;
        case 3:
            GiveRPGX(cl);
            break;
        case 4:
            GiveMO2(cl);
            break;
        case 5:
            GiveII(cl);
            break;
        case 6:
            GiveIII(cl);
            break;
        case 7:
            GiveIIII(cl);
            break;
        case 8:
            GiveIIIII(cl);
            break;
        case 9:
            GiveIIIIII(cl);
            break;
        case 10:
            GiveIIIIIII(cl);
            break;
        case 11: // New option to go back to Sub Menu 6
            New_Menu(cl, VERIFIED, 6, 12, 1, "", SubMenu6); // Return to Sub Menu 6
            HandleScroll(cl);
            break;
    }
    break;
}
		case 7: //Sub Menu 7 
		{
			switch (ScrollID)
			{
			case 1:
				SearchD();
			break;
			case 2:
				BigXP();
			break;
			case 3:
				TrickS();
			break;
			case 4:
			   GTNW();
			break;
			case 5:
				GodL();
			break;
			case 6:
			   BigX2();
			break;
			case 7:
				FunXP();
			break;
			case 8:
			   Sab1();
			break;
			case 9:
				OneF();
			break;
			case 10:
				DefM();
			break;
        }
        break;
    }
		case 8: //Sub Menu 8
		{
			switch (ScrollID)
			{
			case 1:
				
			break;
			case 2:
				ExplosiveBullets(cl);
			break;
			case 3:
			
			break;
			case 4:
				
			break;
			case 5:

			break;
			case 6:

			break;
			case 7:

			break;
			case 8:

			break;
			case 9:

			break;
			case 10:

			break;
        }
        break;
    }
		case 9: //Sub Menu 1 - This is how you add the functions to an option in a sub menu. Sub menu 2 would be case 2: and so on.
		{
			switch (ScrollID)
			{
			case 1:
				Map1(cl);
			break;
			case 2:
				Map2(cl);
			break;
			case 3:
				Map3(cl);
			break;
			case 4:
				Map4(cl);
			break;
			case 5:
				Map5(cl);
			break;
			case 6:
				Map6(cl);
			break;
			case 7:
				Map7(cl);
			break;
			case 8:
				Map8(cl);
			break;
			case 9:
				Map9(cl);
			break;
			case 10:
				Map10(cl);
			break;
			case 11:
				Map12(cl);
			break;
        }
        break;
    }
		case 10: //Sub Menu 10
		{
			switch (ScrollID)
			{
			case 1:
				ToggleColors7(cl);
			break;
			case 2:
				ToggleColors8(cl);
			break;
			case 3:
				ToggleColors3(cl);
			break;
			case 4:
				ToggleColors4(cl);
			break;
			case 5:
				ToggleColors5(cl);
			break;
			case 6:
	            ToggleColors(cl);
			break;
			case 7:
            ToggleColors2(cl);
            break;
			case 8:
			ToggleColors6(cl);
			break;
			case 9:
			CBOX(cl);	
			break;
			case 10:
			CBOXDXX(cl);
			break;
			case 11:
			CBO3XDXX(cl);
			break;
			}
		break;
		}
		case 11:
    for (int i = 0; i < 18; i++)
    {
        if (!strcmp((char*)getName(i), CurrentOpt)) // Compares the selected player's name to the current option so we select the correct client
        {
            Target = i; // Target is the client we select when selecting a player in the player menu.
            menu_s[cl].clScroll = menu_s[cl].ScrollID; // Gets the current scroll so we return to the correct client
            New_Menu(cl, HOST, 99, 13, 1, "", ClientMenu); /* Creates our client options menu, change the number 5 to how many options you have.
                                                               As of now there are 5 options, Give Admin, VIP, Verified, Remove Access and Kick Client */
            menu_s[cl].MenuScroll = menu_s[cl].clScroll; // Sets the menuscroll to the client number so we return to the same client as stated above
            HandleScroll(cl);
        }
    }
    break;
case 12: // Sub Menu 12
{
    switch (ScrollID)
    {
        // Add actions for Sub Menu 12 here
        case 1:
            for(int i = 0; i < 18; i++)
					GodModeAll(i);
				break;
        case 2:
           for(int i = 0; i < 18; i++)
			   RedBoxez(i);
				break;
        case 3:
            for(int i = 0; i < 18; i++)
						FOV2(i);
				break;
        case 4:
            for(int i = 0; i < 18; i++)
						InfAmmo(i);
				break;
        case 5:
           for(int i = 0; i < 18; i++)
						level(i);
				break;
        case 6:
           for(int i = 0; i < 18; i++)
						UnlockAll(i);
				break;
		case 7:
			for(int i = 0; i < 18; i++)
				SuperR(i);
			break;
			case 8:
			for(int i = 0; i < 18; i++)
				
			break;
			}
		break;
		}
			case 99:
{
    switch (ScrollID)
    {
        char nameBuf[100];
        case 1:
            giveAccess(Target, ADMIN, true);
            if (Target != getHost())
                snprintf(nameBuf, sizeof(nameBuf), "Gave %s ^1%s^7 Access", (char*)getName(Target), getAccess(Target, false)), Println(cl, nameBuf);
            break;
        case 2:
            {
            giveAccess(Target, USER, true);
            if (Target != getHost())
            {
                snprintf(nameBuf, sizeof(nameBuf), "Removed %s's access", (char*)getName(Target));
                Println(cl, nameBuf);
                if (menu_s[Target].isOpen)
                    Init(Target, false);
            }
        }
            break;
        case 3:
            if (Target != getHost())
                KickClient(Target, "", false);
            else
                Println(cl, "^1Can't kick the host client");
            break;
        case 4:
            killClient(Target);
        break;
        case 5:
           UnlockAll(Target);
            break;
        case 6:
            GodMode(Target);
            break;
        case 7:
            SpawnKill(Target);
            break;
        case 8:
            TeleportOutClient(Target);
            break;
        case 9:
           ChangeClientName(Target); 
            break;
        case 10:
             IceClan(Target);
            break;
        case 11:
            DesertC1C(Target);
            break;
        case 12:
           FLag(Target);
            break;
		case 13:
		    MiniMap1(Target);
		break;
                 }
            }
            break;
		}
	}

void MonitorButtons(int cl)
	{
		if (!Access_t[cl].MenuLocked)
		{
			if (!menu_s[cl].isOpen)
			{
				if (DPAD_UP(cl))
					Init(cl, true), RESET_BTN(cl);
			}
			else
			{
				if (BUTTON_X(cl))
					Return(cl), RESET_BTN(cl);
				else if (BUTTON_A(cl))
					ExecuteOption(cl, menu_s[cl].MenuID, menu_s[cl].ScrollID), RESET_BTN(cl);
				else if (DPAD_UP(cl) || DPAD_DOWN(cl))
				{
					menu_s[cl].ScrollID -= DPAD_UP(cl);
					if (menu_s[cl].ScrollID < 1) menu_s[cl].ScrollID = menu_s[cl].MaxScroll;
					menu_s[cl].ScrollID += DPAD_DOWN(cl);
					if (menu_s[cl].ScrollID > menu_s[cl].MaxScroll) menu_s[cl].ScrollID = 1;
					HandleScroll(cl), RESET_BTN(cl);
				}
			}
		}
	}

namespace ButtonMonitoring
{  
    enum Buttons
    {
                L1 = 0x41,
                R1 = 0x4D,
                Square = 0x29,
                R3 = 0xD1,
                DpadUp = 0xF5,
                DpadDown = 0x105,
                Cross = 0x11,
                Triangle = 0x34,
                Circle = 0x1D,
                L2 = 0xDC,
                R2 = 0xE8,
                L3 = 0xC4,
                Left = 0x10D,
                Right =  0x119,
                Select = 0xB8,
                Start = 0xAC
    };
    bool ButtonPressed(Buttons Button)
    {
	    return (*(char*)(0x0095C08A + Button) != 0);
    }
}

void onPlayerSpawned(int clientIndex)
{
	sleep(1000);
	char buf[100];
	stdc_snprintf(buf, 100, "WELCOME [ %s ]", (char*)0x014E5408 + (clientIndex * 0x3700));
	SetMessage(clientIndex, buf, "PRESS [{+actionslot 1}] TO OPEN ETHEREAL LIGHT", 2048, 50, 7000, 1500, 255, 255, 255, 255, 0, 255, 0, 0);
}

bool isAlive(int clientIndex)
{
		return *(bool*)(G_Client(clientIndex) + 0x3193) == false;
}

void PLUGIN_START(uint64_t arg)
	{
		for (;;) 
		{
			for (int i = 0; i < 18; i++)
			{
				if (Dvar_GetBool("cl_ingame")) //Checks if we're in-game

				{
					if (!menu_s[i].isLoaded)
					{

						if (isHost(i))
							Access_t[i].Access = HOST; //gives us access if we are the host
						if (hasAccess(i)) //Checks if we have access before anything is done
						{
							*(int*)0x131689C = 1;
							Access_t[i].MenuLocked = false;

							CreateHUDs(i);
							snprintf(mapBuf, sizeof(mapBuf), "Q %s", Dvar_GetString("ui_mapname")); //Gets the mapname once and copies it to our mapBuffer above to reset vision to default
							New_Menu(i, VERIFIED, 0, dfltMax, 1, "", MainMenu);//Creates the main menu when the menu gets loaded
							menu_s[i].isLoaded = true; //Sets our loaded boolean to true and makes it so the above code will only be run once per game
						}
					}
					else
						MonitorButtons(i); //If menu is loaded we will start our button monitoring
				}
				else //means we're not in-game
				{
					if (menu_s[i].isLoaded) //Checks if menu is loaded, if it is we will unload it when we're no longer in game. This resets the menu & removes access
					{
						client_t[i].hasHuds = false, client_t[i].localizedStrings = false;
						Access_t[i].MenuLocked = true, *(int*)0x131689C = 0;
						menu_s[i].Title = NULL;
						menu_s[i].Background = NULL;
						for (int x = 0; x < 18; x++)
							menu_s[i].Options[x] = NULL;
						Access_t[i].Access = USER, menu_s[i].isLoaded = false;
					}
					sleep(25);
				}
			}
		}
	}
}