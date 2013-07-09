#include "tools.h"

char* CTools::getmacaddress()//http://www.codeguru.com/Cpp/I-N/network/networkinformation/article.php/c5451
{
    //This is a NO-OP function. TODO: Figure out how to get the MAC address in Linux
	//static char retval[20];
    //sprintf(retval, "%02X-%02X-%02X-%02X-%02X-%02X", "00", "00", "00", "00", "00", "00");
	//return retval;
}

char* CTools::md5buffer(CBuffer *buff)
{
	static char a[33];
	CalculateMD5(buff->data, buff->count, a);
	return a;
}

char* CTools::md5string(char *str)
{
	static char a[33];
	CalculateMD5(str, (int)strlen(str), a);
	return a;
}

unsigned int CTools::ip2uint(char *ip)
{
	return inet_addr(ip);
}

char* CTools::uint2ip(unsigned int ip)
{
	in_addr a;
	a.s_addr = (u_long)ip;
	return inet_ntoa(a);
}

bool CTools::netconnected()
{
	//Another NO-OP function. TODO: Figure out for Linux.
	return true;
}

void CTools::encryptedbuffer(CBuffer *buff, char*key)
{
	char *inp = buff->data;
	unsigned int inplen = buff->count;
	char KeyBox[257];
	int keylen = std::min((int)strlen(key), 256);
	if(keylen <= 0)return;
	unsigned long i, j, t, x;
	char temp;
	i = j = t = 0;
	for(i = 0; i < 256; i++)
		KeyBox[i] = (char)i;
	for(i = 0; i < 256; i++)
	{
		j = (j + (unsigned long) KeyBox[i] + key[i % keylen]) % 256;
		temp =  KeyBox[i];
		KeyBox[i] = KeyBox[j];
		KeyBox[j] =  temp;
	}
	i = j = 0;
	for(x = 0; x < inplen; x++)
	{
		i = (i + 1U) % 256;
		j = (j + (unsigned long) KeyBox[i]) % 256;
		temp = KeyBox[i];
		KeyBox[i] = KeyBox[j] ;
		KeyBox[j] = temp;
		t = ((unsigned long) KeyBox[i] + (unsigned long) KeyBox[j]) %  256 ;
		inp[x] = (inp[x] ^ KeyBox[t]);
	}
}

bool CTools::comparematch(char*string, char*wild)
{
	char *cp = 0, *mp = 0;
	while ((*string) && (*wild != '*')) {
		if((*wild != *string) && (*wild != '?')) {
			return 0;
		}
		wild++;
		string++;
	}
	while (*string) {
		if (*wild == '*') {
			if(!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}
	while (*wild == '*') {
		wild++;
	}
	return !*wild;
}
unsigned int CTools::adler32(CBuffer*buff)
{
	char*data = buff->data;
	unsigned int len = buff->count;
	unsigned int a = 1, b = 0;
	while (len) {
		unsigned tlen = len > 5550 ? 5550 : len;
		len -= tlen;
		do {
			a += *data++;
			b += a;
		} while (--tlen);
		a = (a & 0xffff) + (a >> 16) * (65536-65521);
		b = (b & 0xffff) + (b >> 16) * (65536-65521);
	}
	if (a >= 65521)
		a -= 65521;
	b = (b & 0xffff) + (b >> 16) * (65536-65521);
	if (b >= 65521)
		b -= 65521;
	return b << 16 | a;
}


