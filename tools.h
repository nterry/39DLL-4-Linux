#pragma once
#include "includes.h"
#include "buffer.h"
void CalculateMD5(char *, int, char *);
class CTools
{
public:
	static char* getmacaddress();
	static char* md5buffer(CBuffer*buff);
	static char* md5string(char*str);
	static unsigned int ip2uint(char*ip);
	static char* uint2ip(unsigned int ip);
	static bool netconnected();
	static void encryptedbuffer(CBuffer*buff, char*key);
	static bool comparematch(char*string, char*wild);
	static unsigned int adler32(CBuffer*buff);
};
