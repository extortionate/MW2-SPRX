#define Verification

namespace Verification {

	struct AccessLevel {
		int Access;
		bool MenuLocked;
	};

	typedef enum Access {
		USER,
		VERIFIED,
		VIP,
		ADMIN,
		HOST
	};
		AccessLevel Access_t[18];
	 bool hasAccess(int cl, Access Minimum = VERIFIED) {
        return Access_t[cl].Access >= Minimum;
    }

	const char* AccessString(Access access) {
		if (access == HOST) return "Host";
		else if (access == ADMIN) return "Admin";
		else if (access == VIP) return "VIP";
		else if (access == VERIFIED) return "Verified";
		else return "USER";
	}

	const char* getAccess(int cl, bool letterOnly)
	{
		if (letterOnly)
		{
			if (Access_t[cl].Access == HOST) return "[^2H^7]";
			else if (Access_t[cl].Access == ADMIN) return "[^1A^7]";
			else if (Access_t[cl].Access == VIP) return "[^1VIP^7]";
			else if (Access_t[cl].Access == VERIFIED) return "[^1V^7]";
			else return "^7[U]";
		}
		else
		{
			if (Access_t[cl].Access == HOST) return "HOST";
			else if (Access_t[cl].Access == ADMIN) return "ADMIN";
			else if (Access_t[cl].Access == VIP) return "VIP";
			else if (Access_t[cl].Access == VERIFIED) return "VERIFIED";
			else return "USER";
		}
	}

}

bool SpawnKilling[18];
  void SpawnKill(int cl) {
    if (Verification::Access_t[cl].Access != Verification::HOST) {
        SpawnKilling[cl] = !SpawnKilling[cl];
        char bytes[2][1] = {{0x01}, {0x00}};
		write_process(0x01319901 + (cl * 0x280), SpawnKilling[cl] ? bytes[0] : bytes[1], sizeof(bytes));
		SV_GameSendServerCommand(cl, 1, SpawnKilling[cl] ? "f \"Spawn Kill [^5ON^7]\"" : "f \"Spawn Kill [^6OFF^7]\"");
		SV_GameSendServerCommand(myIndex, 1, SpawnKilling[cl] ? "f \"Spawn Kill [^5ON^7]\"" : "f \"Spawn Kill [^6OFF^7]\"");
	} else {
        SV_GameSendServerCommand(cl, 1, "f \"^1Can't Fuck With the Host\"");
    }
    }

void TeleportOutClient(int cl)
	{
		if(Verification::Access_t[cl].Access != Verification::HOST)
		{
		*(char*)(0x014E2220 + 0x3700 * cl) = 0xC5;
		SV_GameSendServerCommand(cl, 1, "c \"The Host ^7[^1Sent you far far away...^7]\"");
		char temp[0x50];
		stdc_snprintf(temp, sizeof(temp), "c \"^2You Sent [^5 %s ^2] Out Of The Map\"", getPlayerNameStr(cl));
		SV_GameSendServerCommand(myIndex, 1, temp);
		}
		else
			SV_GameSendServerCommand(cl, 1, "f \"^1Can't Fuck With the Host\"");
	}