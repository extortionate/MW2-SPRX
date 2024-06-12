#define sleep(ms) (sys_timer_usleep(ms*1000))	
float floatArray[100];
	char byteArray[100];
	int intArray[100];
	char returnRead[100];

	float* ReadFloat(int address, int length) {
		for (int i = 0; i < 100; i++)
			floatArray[i] = 0;
		for (int i = 0; i < length; i++) {
			floatArray[i] = *(float*)(address + (i * 0x04));
		}
		return floatArray;
	}
	void WriteFloat(int address, float* input, int length) {
		for (int i = 0; i < length; i++) {
			*(float*)(address + (i * 0x04)) = input[i];
		}
	}

	char* ReadBytes(int address, int length) {
		for (int i = 0; i < 100; i++)
			byteArray[i] = 0;
		for (int i = 0; i < length; i++) {
			byteArray[i] = *(char*)(address + (i));
		}
		return byteArray;
	}

	void WriteBytes(int address, char* input, int length) {
		for (int i = 0; i < length; i++) {
			*(char*)(address + (i)) = input[i];
		}
	}

	int32_t write_process(uint64_t ea, const void * data, uint32_t size)
	{
		system_call_4(905, (uint64_t)sys_process_getpid(), ea, size, (uint64_t)data);
		return_to_user_prog(int32_t);
	}
	void* read_process(uint64_t ea, uint32_t size)
	{
		void* data;
		system_call_4(904, (uint64_t)sys_process_getpid(), ea, size, (uint64_t)data);
		return data;
	}

	int* ReadInt(int address, int length) {
		for (int i = 0; i < 100; i++)
			intArray[i] = 0;
		for (int i = 0; i < length; i++) {
			intArray[i] = *(int*)(address + (i * 0x04));
		}
		return intArray;
	}

	void WriteInt(int address, int* input, int length) {
		for (int i = 0; i < length; i++) {
			*(int*)(intArray + (i * 0x04)) = input[i];
		}
	}


	int WriteString(int address, char* string) {
		int FreeMem = 0x1D00000;
		int strlength = strlen(string);
		char* strpointer = *(char**)FreeMem = string;
		char* StrBytes = ReadBytes(*(int*)FreeMem, strlength);
		WriteBytes(address, StrBytes, strlength);
		return strlength;
	}


	const char* ReadString(int address) {
		int strlength = 100;
		char* StrBytes = ReadBytes(address, strlength);
		char StopBytes = 0x00;

		for (int i = 0; i < strlength; i++)
			returnRead[i] = 0;
		for (int i = 0; i < strlength; i++) {
			if (StrBytes[i] != StopBytes)
				returnRead[i] = StrBytes[i];
		}
		return returnRead;
	}

	void reverse(char s[]) {
		int i, j;
		char c;

		for (i = 0, j = strlen(s) - 1; i<j; i++, j--) {
			c = s[i];
			s[i] = s[j];
			s[j] = c;
		}
	}

	/* itoa:  convert n to characters in s */
	void itoa(int n, char s[]) {
		int i, sign;

		if ((sign = n) < 0)  /* record sign */
			n = -n;          /* make n positive */
		i = 0;
		do {		/* generate digits in reverse order */
			s[i++] = n % 10 + '0';	/* get next digit */
		} while ((n /= 10) > 0);	/* delete it */
		if (sign < 0)
			s[i++] = '-';
		s[i] = '\0';
		reverse(s);
	}

	bool isdigit(char Num) {
		return (Num >= 0x30 && Num <= 0x39);
	}

	int Atoi(const char *c) {
		int value = 0;
		int sign = 1;
		if (*c == '+' || *c == '-') {
			if (*c == '-') sign = -1;
			c++;
		}
		while (isdigit(*c)) {
			value *= 10;
			value += (int)(*c - '0');
			c++;
		}
		return value * sign;
	}

	typedef void* PVOID;
	int Memcopy(PVOID destination, const PVOID source, size_t size) {
		system_call_4(905, (uint64_t)sys_process_getpid(), (uint64_t)destination, size, (uint64_t)source);
		__dcbst(destination);
		__sync();
		__isync();
		return_to_user_prog(int);
	}

	void *Memset(void *m, int c, size_t n)
	{
		char *s = (char*)m;
		while (n-- != 0) {
			*s++ = (char) c;
		}
		return m;
	}

	void PatchInJump(int Address, int Destination, bool Linked) {
		int FuncBytes[4];
		Destination = *(int*)Destination;
		FuncBytes[0] = 0x3D600000 + ((Destination >> 16) & 0xFFFF); // lis 	%r11, dest>>16
		if(Destination & 0x8000) // If bit 16 is 1
		FuncBytes[0] += 1;
		FuncBytes[1] = 0x396B0000 + (Destination & 0xFFFF); // addi	%r11, %r11, dest&0xFFFF
		FuncBytes[2] = 0x7D6903A6; // mtctr	%r11
		FuncBytes[3] = 0x4E800420; // bctr
		if(Linked)
		FuncBytes[3] += 1; // bctrl
		Memcopy((void*)Address, FuncBytes, 4*4);
	}

	void HookFunctionStart(int Address, int saveStub, int Destination) {
	// Get the actual addresses
		saveStub = *(int*)saveStub;
		int BranchtoAddress = Address + (4*4);
		// use this data to copy over the stub
		int StubData[8];
		StubData[0] = 0x3D600000 + ((BranchtoAddress >> 16) & 0xFFFF); // lis 	%r11, dest>>16
		if(BranchtoAddress & 0x8000) // If bit 16 is 1
		StubData[0] += 1;
		StubData[1] = 0x396B0000 + (BranchtoAddress & 0xFFFF); // addi	%r11, %r11, dest&0xFFFF
		StubData[2] = 0x7D6903A6; // mtctr	%r11
		Memcopy(&StubData[3], (void*)Address, 4*4);// copy the 4 instructions
		StubData[7] = 0x4E800420; // bctr
		Memcopy((void*)saveStub, StubData, 8*4);
		PatchInJump(Address, Destination, false);
	}

	//KEYBOARD
	//ps3 keyboard
	char* ReadBytesC(int address, int length)
	{
		char bytesArray[100];
		for (int i = 0; i < length; i++)
		{
			bytesArray[i] =  *(char*)(address + (i));
		}
		return bytesArray;
	}

	char* ReadStringC(int address)
	{
		char returnReads[100];
		memset(&returnReads[0], 0, sizeof(returnReads));
		int strlength = 75;
		char* StrBytes = ReadBytesC(address, strlength);
		for (int i = 0; i < strlength; i++)
		{
			if (StrBytes[i] != 0x00)
				returnReads[i] = StrBytes[i];
		else
			break;
	}
	return returnReads;
	}