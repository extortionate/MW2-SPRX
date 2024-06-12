typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned long long QWORD;
typedef unsigned char BYTE;
typedef unsigned char* PBYTE;
typedef void* PVOID;
typedef long long __int64;
__int64 ThreadRTOC;

typedef unsigned long DWORD;
typedef char CHAR;
typedef int BOOL;
typedef float FLOAT;
typedef CHAR *PCHAR;
#define VOID void
#define CONST const
#define FXMATERIAL             "fxMaterial"
#define FXGLOWMATERIAL         "fxGlowMaterial"
typedef unsigned short WORD;
typedef DWORD *PDWORD;
#define AngleToShort(X) ((int)((X) * 65536 / 360) & 65535)
#define ShortToAngle(X) ((X) * (360 / 65536))
#define GetPointer(X) *(int*)(X)
#define TRUE true
#define FALSE false

sys_ppu_thread_t create_thread(void (*entry)(uint64_t), int priority, size_t stacksize, const char* threadname,sys_ppu_thread_t id) {	
	if(sys_ppu_thread_create(&id, entry, 0, priority , stacksize, 0, threadname) == CELL_OK) {
		//console_write("\n\n Mw2_Menu_Loaded!\n\n");	
	}	
    return id;
}

int Memcpy(void* destination, const void* source, size_t size) {
	system_call_4(905, (QWORD)sys_process_getpid(), (QWORD)destination, size, (QWORD)source);
	__dcbst(destination); __sync(); __isync();
	return_to_user_prog(int);
}

void WriteByte(int Address, char* Value) {
	Memcpy((void*)Address, &Value, sizeof(Value));
}

int console_write(const char * s) {
	int len = strlen(s);
	system_call_4(SYS_TTY_WRITE, 0, (uint64_t)s, len, (uint64_t)&len);
	return 0;
}

sys_pid_t get_process_id() {
	system_call_0(1);
	return_to_user_prog(sys_pid_t);
}

int CountString(char* text, char Find)
{
	int i = 0;
	while(*text) {
		if(*text == Find)
			i++;
		text++;
	}
	return i+1;
}

void ReadFloat(int address, int length, float* out) {
	for (int i = 0; i < length; i++) {
		out[i] = *(float*)(address + (i * 0x04));
	}
}

float floatArray1337[100];
float* ReadFloatR(int address, int length)
{
	for (int i = 0; i < length; i++)
	{
		floatArray1337[i] = *(float*)(address + (i * 0x04));
	}
	return floatArray1337;
}

int VSprintf(char *ptr, const char *fmt, va_list ap) {

	if(!ptr || !fmt)
		return -1;

	__int64 Int;
	int sLen;
	char* String;
	char* aPtr;
	byte bTemp;
	char Buf[0x100];
	
	while(*fmt) {
		char Temp = *fmt;
		int Length = -1;

		if(Temp == '%') {
			fmt++;

			if(*fmt == '0') {
				fmt++;
				Length = 0;
				char* aPtr = Buf;
				while(isdigit(Temp = *fmt)) {
					*aPtr = Temp;
					aPtr++;
					fmt++;
					*aPtr = 0;
				}
				Length = Atoi(Buf);
			}

			switch(*fmt) {

			case 's':
				String = va_arg(ap, char*);
				if(String) {
					while(*String) {
						*ptr = *String;
						String++;
						if(*String)
							ptr++;
					}
				}
				break;

			case 'p':
				Length = 8;
			case 'X':
			case 'x':
				bool Found;
				bool Caps;
				Int = va_arg(ap, __int64);
				if(Length == -1)
					Int = Int & 0xFFFFFFFF;
				Caps = *fmt != 'x';
				Found = false;
				for(int i = 0; i < 8; i++) { // for each bit :: 0 0   0 0   0 0   0 0
					bTemp = (Int >> ( 28 - (i * 4))) & 0xF;
					if((8-i) <= Length)
						Found = true;
					if(Length == -1 && !Found && bTemp)
						Found = true;
					if(Found) {
						if(bTemp <= 9)// decimal
							*ptr = 0x30 + bTemp;
						else// hex
							*ptr = (Caps ? 0x40 : 0x60) + bTemp - 9;
						if(i != 7)
							*ptr++;
					}
				}
				break;
				
			case 'i':
			case 'd':
				memset(Buf, '0', 0xFF);
				Int = va_arg(ap, __int64);
				if(Length == -1)
					Int = Int & 0xFFFFFFFF;
				aPtr = ptr;
				itoa((int)Int, ptr);
				sLen = strlen(ptr);
				if(Length == -1)
					Length = sLen;
				Memcopy(Buf + 0xFF - sLen, aPtr, sLen);
				Memcopy(aPtr, Buf + 0xFF - Length, Length);
				ptr = aPtr + Length - 1;
				break;

			case 'f':

				break;
			}
		} else
			*ptr = Temp;
		ptr++;
		fmt++;
	}
	*ptr = 0;
	return 1;
}

int Sprintf(char *ptr, const char *fmt, ...) {// %s, %i, %d, %x, %X, %p
	
	if(!ptr || !fmt)
		return -1;
	
	va_list ap;
	va_start(ap, fmt);

	VSprintf(ptr, fmt, ap);
	
	va_end(ap);
	return 1;
}

char vaBuffer[0x2000];
char* va(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	VSprintf(vaBuffer, fmt, ap);
	
	va_end(ap);
	return vaBuffer;
}

int Printf(const char * s, ...) {
	char conBuffer[0x2000];
	va_list ap;
	va_start(ap, s);

	VSprintf(conBuffer, s, ap);
	
	va_end(ap);

	int len = strlen(conBuffer);
	system_call_4(SYS_TTY_WRITE, 0, (uint64_t)conBuffer, len, (uint64_t)&len);
	return_to_user_prog(int);
}

void GetRTOC() {
	__asm("mr %r3, %r2");
}

void SetRTOC(uint64_t RTOC) {
	__asm("mr %r2, %r3");
}

void Newbyte(int Address,int nb)
{
	for (int i = 0; i <= nb; i++)
	{
		*(char*)(Address + (i)) = 0x00;
	}
}


bool IsResolution1080() {
	CellVideoOutConfiguration config;
    cellVideoOutGetConfiguration(CELL_VIDEO_OUT_PRIMARY, &config, NULL);
	if (config.resolutionId == CELL_VIDEO_OUT_RESOLUTION_1080)
		return true;
	else
		return false;
}

bool IsResolution720() {
	CellVideoOutConfiguration config;
    cellVideoOutGetConfiguration(CELL_VIDEO_OUT_PRIMARY, &config, NULL);
	if (config.resolutionId == CELL_VIDEO_OUT_RESOLUTION_720)
		return true;
	else
		return false;
}