struct scr_entref_t {
	unsigned short entnum;
	unsigned short classnum;
};
struct VariableStackBuffer {
	const char *pos;
	unsigned short size;
	unsigned short bufLen;
	unsigned int localId;
	char time;
	char buf[1];
};
struct VariableStruct {
	int intValue;
	float floatValue;
	short stringValue;
	const char *string;
	float *vectorValue;
	int objectClassnum;
	int objectNum;
	int objectOffset;
	int type;
	int arraySize;
};
union VariableUnion {
	int intValue;
	float floatValue;
	unsigned int stringValue;
	float *vectorValue;
	const char *codePosValue;
	unsigned int pointerValue;
	scr_entref_t entref;
	VariableStackBuffer *stackValue;
	unsigned int entityOffset;
};
struct VariableValue {
	VariableUnion u;
	int type;
};
struct function_stack_t {
	const char *pos;
	unsigned int localId;
	unsigned int localVarCount;
	VariableValue *top;
	VariableValue *startTop;
};
struct function_frame_t {
	function_stack_t fs;
	int topType;
};
struct scrVmPub_t
{
	VariableValue stack[0x400];
	char unk_0[0x1C];
	unsigned int *localVars;
	VariableValue *maxstack;
	int function_count;
	function_frame_t *function_frame;
	VariableValue *top;
	int breakpointOutparamcount;
	unsigned int inparamcount;
	unsigned int outparamcount;
	function_frame_t function_frame_start[32];
};
struct OnNotifyReturn {
	VariableValue* top;
	int stringValue;
	const char* string;
	int classnum;
	int entnum;
	int paramcount;
};
struct SprintState
{
	int sprintButtonUpRequired;
	int sprintDelay;
	int lastSprintStart;
	int lastSprintEnd;
	int sprintStartMaxLength;
};
struct MantleState
{
	float yaw;
	int timer;
	int transIndex;
	int flags;
};
struct WeaponState
{
	int weapAnim;
	int weaponTime;
	int weaponDelay;
	int weaponRestrictKickTime;
	int weaponstate;
	int weapHandFlags;
	int weaponShotCount;
};
struct ActionSlotParam_SpecifyWeapon
{
	unsigned int index;
};
struct ActionSlotParam
{
	ActionSlotParam_SpecifyWeapon specifyWeapon;
};
struct objective_t
{
	int state;
	float origin[3];
	int entNum;
	int teamNum;
	int icon;
};
struct WeaponData {
	unsigned int weaponIdx : 8;
	unsigned int weaponVariation : 6;
	unsigned int weaponScopes : 3;
	unsigned int weaponUnderBarrels : 2;
	unsigned int weaponOthers : 7;
	unsigned int scopeVariation : 6;
};
union Weapon {
	WeaponData __s0;
	unsigned int data;
};
struct usercmd_s { //size 0x28
	int serverTime; //0x00 - 0x04
	unsigned int buttons; //0x04 - 0x08
	int angles[3]; //0x08 - 0x14
	Weapon weapon; //0x14 - 0x18
	char offHand[2]; //0x18 - 0x1A
	char forwardmove; //0x1A - 0x1B
	char rightmove; //0x1B - 0x1C 
	unsigned short airburstMarkDistance; //0x1C - 0x1E
	unsigned short meleeChargeEnt; //0x1E - 0x20
	char meleeChargeDist; //0x20 - 0x21
	char selectedLoc[2]; //0x21 - 0x23
	char selectedLocAngle; //0x23 - 0x24
	char __0x1[0x1]; //0x24 - 0x25
};
enum clientConnected_t
{
	CON_DISCONNECTED = 0x0,
	CON_CONNECTING = 0x1,
	CON_CONNECTED = 0x2,
};
enum sessionState_t
{
	SESS_STATE_PLAYING = 0x0,
	SESS_STATE_DEAD = 0x1,
	SESS_STATE_SPECTATOR = 0x2,
	SESS_STATE_INTERMISSION = 0x3,
};
struct playerTeamState_t {
	int location;
};
typedef struct clientState_s
{
	int clientIndex;
	int team;
	int modelindex;
	int dualWielding;
	int riotShieldNext;
	int attachModelIndex[6];
	int attachTagIndex[6];
	char name[0x20];
	float maxSprintTimeMultiplier;
	int rank;
	int prestige;
	int perks[2];
	int diveState;
	int voiceConnectivityBits;
	char clanAbb[4];
} clientState_s;

struct playerState_s {
	int commandTime; //0x00 - 0x04
	int pm_type; //0x04 - 0x08
	int bobCycle; //0x08 - 0x0C
	int pm_flags; //0x0C - 0x10
	int otherFlags; //0x10 - 0x14
	int linkFlags; //0x14 - 0x18
	int pm_time; //0x18 - 0x1C
	float origin[3]; //0x1C - 0x28
	float velocity[3]; //0x28 - 0x34
	int weaponTime; //0x34 - 0x38
	int weaponDelay; //0x38 - 0x3C
	int grenadeTimeLeft; //0x3C - 0x40
	int throwBackGrenadeOwner; //0x40 - 0x44
	int remoteEyesEnt; //0x44 - 0x48
	int remoteEyesTagname; //0x44 - 0x4C
	int remoteControlEnt; //0x4C - 0x50
	int foliageSoundTime; //0x50 - 0x54
	int gravity; //0x54 - 0x58
	float leanf; //0x58 - 0x5C
	int speed; //0x5C - 0x60
	float delta_angles[3]; //0x60 - 0x6C
	int groundEntityNum; //0x6C - 0x70
	float vLadderVec[3]; //0x70 - 0x7C
	int jumpTime; //0x7C - 0x80
	float jumpOriginZ; //0x80 - 0x84
	int legsTimer; //0x84 - 0x8C
	int legsAnim; //0x8C - 0x90
	int torsoTimer; //0x90 - 0x94
	int torsoAnim; //0x94 - 0x98
	int legsAnimDuration; //0x98 - 0x9C
	int torsoAnimDuration; //0x9C - 0x100
	int damageTimer; //0x100 - 0x104
	int damageDuration; //0x104 - 0x108
	int flinchYawAnim; //0x108 - 0x10C
	int corpseIndex; //0x10C - 0x110
	int movementDir; //0x110 - 0x114
	int eFlags; //0x114 - 0x118
	int eventSequence;
	int events[4];
	unsigned int eventParms[4];
	int oldEventSequence;
	int unpredictableEventSequence;
	int unpredictableEventSequenceOld;
	int unpredictableEvents[4];
	unsigned int unpredictableEventParms[4];
	int clientNum;
	int viewmodelIndex;
	float viewAngles[3];
	int viewHeightTarget;
	float viewHeightCurrent;
	char unknown_data8[0x94];
	int locationSelectionInfo;
	SprintState sprintState;
	float holdBreathScale;
	int holdBreathTimer;
	float moveSpeedScaleMultiplier;
	MantleState mantleState;
	WeaponState weaponStates[2];
	Weapon weaponsEquipped[15]; //0x220 - 0x25C
	char unknown_data[0x4C]; //0x25C - 0x2A8
	Weapon offHand; //0x2A8 - 0x2AC
	int offhandPrimary; //0x2AC - 0x2B0
	int offhandSecondary; //0x2B0 - 0x2B4
	Weapon weapon; //0x2B4 - 0x2B8
	unsigned int primaryWeaponForAltMode; //0x2B8 - 0x2BC
	int weapFlags; //0x2BC - 0x2C0
	float fWeaponPosFrac; //0x2C0 - 0x2C4
	float aimSpreadScale; //0x2C4 - 0x2C8
	int adsDelayTime; //0x2C8 - 0x2CC
	int spreadOverride; //0x2CC - 0x2D0
	int spreadOverrideState; //0x2D0 - 0x2D4
	int isDualWeilding; //0x2D4 - 0x2D8
	int __pad; //0x2D8 - 0x2DC
	int unk1_ammo; //0x2DC - 0x2E0
	char __pad2[0xC]; //0x2E0 - 0x2EC
	int unk2_ammo; //0x2EC - 0x2F0
	char __pad3[0x64]; //0x2F0 - 0x354
	int unk3_ammo; //0x354 - 0x358
	char __pad4[0x8]; //0x358 - 0x360
	int unk4_ammo; //0x360 - 0x364
	char __pad5[0x8]; //0x364 - 0x36C
	int unk5_ammo; //0x36C - 0x370
	char __pad6[0x8]; //0x370 - 0x378
	int unk6_ammo; //0x378 - 0x37C
	char unknown_data1[0x88];
	int weapLockedFlags;
	int weapLockedEntnum;
	float weapLockedPos[3];
	int weaponIdleTime;
	float meleeChargeYaw;
	int meleeChargeDist;
	int meleeChargeTime;
	int perks[2];
	int perkSlots[8];
	int actionSlotType[4];
	ActionSlotParam actionSlotParam[4];
	int weaponHudIconOverrides[6];
	int animScriptedType;
	int shellshockIndex;
	int shellshockTime;
	int shellshockDuration;
	float dofNearStart;
	float dofNearEnd;
	float dofFarStart;
	float dofFarEnd;
	float dofNearBlur;
	float dofFarBlur;
	float dofViewmodelStart;
	float dofViewmodelEnd;
	objective_t objective[0x20];
	char unknown_data3[0x2958];
};
struct clientSession_t {
	sessionState_t sessionState;
	int uknown6;
	int score;
	int deaths;
	int kills;
	int assists;
	short scriptpersid;
	char undefined;
	clientConnected_t connected;
	usercmd_s cmd;
	usercmd_s oldcmd;
	int localClient;
	int predictItemPickup;
	char newnetname[0x20];
	int maxHealth;							//0x3228
	int enterTime;							//0x322C
	playerTeamState_t teamState;			//0x3230
	int voteCount;							//0x3234
	int teamVoteCount;						//0x3238
	float moveSpeedScaleMultiplier;			//0x323C
	int viewmodelIndex;						//0x3240
	int noSpectate;							//0x3244
	int teamInfo;							//0x3248
	clientState_s cs;						//0x324C - 0x32D0
	int __padding;							//0x32D0 - 0x32D4
	int title;								//0x32D4 - 0x32D8
	int emblem;								//0x32D8 - 0x32DC
	int __padding2;							//0x32DC - 0x32E0
	int psOffsetTime;						//0x32E0 - 0x32E4
	int hasRadar;							//0x32E4 - 0x32E8
	int isRadarBlocked;						//0x32E8 - 0x32EC
	char unknown_data5[0x130];				//0x32EC - 0x341C
};
struct gclient_s {
	playerState_s ps;
	clientSession_t sess;
	int spectatorclient;
	int flags;
	char __0x8[0x8];
	int buttons;
	int oldbuttons;
	int latched_buttons;
	int buttonsSinceLastFrame;
	char __0x2C4[0x2C4];
};
struct EntHandle {
	unsigned short number;
	unsigned short infoIndex;
};
struct LerpEntityStatePlayer
{
	int leanf;
	int movementDir;
	int torsoPitch;
	int waistPitch;
	int offhandWeapon;
	int lastSpawnTime;
};
struct LerpEntityStateAnonymous
{
	int data[9];
};
struct LerpEntityStateMissile
{
	int launchTime;
	int flightMode;
};
struct LerpEntityStateScriptMover
{
	int entToTakeMarksFrom;
	int xModelIndex;
	int animIndex;
	int animTime;
};
struct LerpEntityStateSoundBlend
{
	int lerp;
};
struct LerpEntityStateLoopFx
{
	int cullDist;
	int period;
};
struct LerpEntityStateTurret
{
	float gunAngles[3];
	int lastBarrelRotChangeTime;
	int lastBarrelRotChangeRate;
	int lastHeatChangeTime;
	int lastHeatChangeLevel;
};
struct LerpEntityStateVehicle
{
	int indices;
	int flags;
	int bodyPitch;
	int bodyRoll;
	int steerYaw;
	int gunPitch;
	int gunYaw;
	int playerIndex;
	int pad;
};
struct LerpEntityStatePlane
{
	int ownerNum;
	int enemyIcon;
	int friendIcon;
};
union LerpEntityStateTypeUnion
{
	LerpEntityStateTurret turret;
	LerpEntityStateLoopFx loopFx;
	LerpEntityStatePlayer player;
	LerpEntityStateScriptMover scriptMover;
	LerpEntityStateVehicle vehicle;
	LerpEntityStatePlane plane;
	LerpEntityStateMissile missile;
	LerpEntityStateSoundBlend soundBlend;
	LerpEntityStateAnonymous anonymous;
};
struct trajectory_t
{
	int trType;
	int trTime;
	int trDuration;
	float trBase[3];
	float trDelta[3];
};
struct LerpEntityState
{
	int eFlags;
	trajectory_t pos;
	trajectory_t apos;
	LerpEntityStateTypeUnion u;
};
struct entityState_s
{
	int number;	// 0x0
	int eType;	// 0x4
	LerpEntityState lerp;	// 0x8
	int time2;	// 0x78
	int otherEntityNum;	// 0x7C
	int attackerEntityNum;	// 0x80
	int groundEntityNum;	// 0x84
	int surfType;	// 0x88
	int loopSound;	// 0x8C
	union
	{
		int brushmodel;
		int item;
		int xmodel;
		int primaryLight;
	} index;	// 0x90
	int clientNum;	// 0x94
	int iHeadIcon;	// 0x98
	int iHeadIconTeam;	// 0x9C
	int solid;	// 0xA0
	int eventParm;	// 0xA4
	int eventSequence;	// 0xA8
	int events[4];	// 0xAC
	int eventParms[4];	// 0xBC
	Weapon weapon;	// 0xCC
	int legsAnim;	// 0xD0
	int torsoAnim;	// 0xD4
	union
	{
		int scale;
		int eventParm2;
		int helicopterStage;
	} un1;	// 0xD8
	union
	{
		int hintString;
		int grenadeInPickupRange;
		int vehicleXModel;
	} un2;	// 0xDC
	int clientLinkInfo;
	int partBits[6];	// 0xE4
	int clientMask[1];	// 0x0
};
struct enitityShared_t
{
	char linked;	// 0x4
	char bmodel;	// 0x5
	char svFlags;	// 0x6
	char inuse;	// 0x7
	float mins[3];	// 0x8
	float maxs[3];	// 0x14
	int contents;	// 0x20
	float absmin[3];	// 0x24
	float absmax[3];	// 0x30
	EntHandle ownerNum;	// 0x54
	int eventTime;	// 0x58
};
struct gentity_s
{
	entityState_s s; // 0x0
	enitityShared_t r; // 0xFC
	gclient_s *client; // 0x158
	int pTurretInfo; // 0x15C
	int scr_vehicle; // 0x160
	int physicsBody; // 0x164
	short model; // 0x168
	char physicsObject; // 0x16A
	char takedamage;  // 0x16B
	char active; // 0x16C
	char handler; // 0x16D
	char team; // 0x16E
	char freeAfterEvent;
	short code_classname; // 0x172
	short classname; // 0x174
	short script_linkname; // 0x176
	unsigned short target; // 0x178
	unsigned short targetname; // 0x17A
	int attachIgnoreCollision; // 0x17C
	int spawnFlags; // 0x180
	int flags; // 0x184
	int unlinkAfterEvent; // 0x188
	int clipMask;  // 0x18C
	int processedFrame; // 0x190
	EntHandle parent; // 0x194
	int nextthink; // 0x198
	int health; // 0x19C
	int maxhealth; // 0x1A0
	int damage; // 0x1A4
	int count; // 0x1A8
	char __0x1AC[0x60]; //0x1AC
	EntHandle projEntity;	// 0x20C
	EntHandle linkedEntity;// 0x210
	int tagInfo;	// 0x214
	gentity_s *tagChildren;// 0x218
	unsigned short attachModelNames[19];// 0x21C
	unsigned short attachTagNames[19];// 0x242
	int useCount;	// 0x268
	int nextFree;	// 0x26C
	int birthtime; // 0x270
	char unknown[0xC]; // 0x274
};

union DvarValue
{
	bool enabled;
	int integer;
	unsigned int unsignedInt;
	float value;
	float vector[4];
	const char *string;
	char color[4];
}; //0x10
union DvarLimits
{
	struct {
		int stringCount;
		const char **strings;
	} enumeration;

	struct {
		int min;
		int max;
	} integer;

	struct {
		float min;
		float max;
	} value, vector;
}; //0x8
struct dvar_s
{
	const char *name; //0x0
	const char *description; //0x4
	unsigned short flags; //0x8
	char type; //0xA
	bool modified; //0xB
	DvarValue current; //0xC
	DvarValue latched; //0x1C
	DvarValue reset; //0x2C
	DvarLimits domain; //0x3C
	dvar_s * next; //0x44
};


//nonhost ui
struct Font_s
{
	int fontName;
	float pixelHeight;
	int glyphCount;
	int material;
	int glowMaterial;
	int glyphs;
};
struct ScreenPlacement
{
	int64_t	 scaleVirtualToReal;
	int64_t scaleVirtualToFull;
	int64_t scaleRealToVirtual;
	int64_t virtualViewableMin;
	int64_t virtualViewableMax;
	int64_t virtualTweakableMin;
	int64_t virtualTweakableMax;
	int64_t realViewportBase;
	int64_t realViewportSize;
	int64_t realViewportMid;
	int64_t realViewableMin;
	//int64_t realViewableMax; you can split any var in here by spliting up the 64 bit value to a 32 bit value for x/y
	int32_t realViewableMaxX;
	int32_t realViewableMaxY;
	int64_t realTweakableMin;
	int64_t realTweakableMax;
	int64_t subScreen;
};
