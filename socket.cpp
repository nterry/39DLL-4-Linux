#include <netdb.h>
#include "socket.h"
#include <unistd.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#define closesocket(s) close(s);

int SenderAddrSize = sizeof(sockaddr);
sockaddr_in CSocket::SenderAddr;
int CSocket::sockid=0;
bool CSocket::tcpconnect(char *address, int port, int mode)
{
	int SOCKET_TIMEOUT=3;
	struct timeval tv;
	tv.tv_sec = SOCKET_TIMEOUT;
	tv.tv_usec = 0 ;
	setsockopt (sockid, SOL_SOCKET, SO_SNDTIMEO, (void*)&tv, sizeof tv);
	setsockopt (sockid, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof tv);

	sockaddr_in addr;
	hostent* hostEntry;
	if((sockid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
        return false;
	if((hostEntry = gethostbyname(address)) == NULL)
	{
		closesocket(sockid);
		return false;
	}
	addr.sin_family = AF_INET;
	bcopy ( hostEntry->h_addr, &(addr.sin_addr.s_addr), hostEntry->h_length);
	addr.sin_port = htons((u_short)port);
	if(mode ==2)setsync(1);
	if(connect(sockid, (struct sockaddr*)&addr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
        	closesocket(sockid);
        	return false;
	}
	if(mode ==1)setsync(1);
	return true;
}

bool CSocket::tcplisten(int port, int max, int mode)
{
	if((sockid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) return false;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if(mode)setsync(1);
	if(bind(sockid, (struct sockaddr*)&addr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		closesocket(sockid);
		return false;
	}
	if(listen(sockid, max) == SOCKET_ERROR)
	{
		closesocket(sockid);
		return false;
	}
	return true;
}

CSocket::CSocket(int sock)
{
	sockid = sock;
	udp = false;
	format = 0;
}
CSocket::CSocket()
{
	udp = false;
	format = 0;
}



CSocket::~CSocket()
{
	if(sockid<0)return;
	shutdown(sockid, 1);
	closesocket(sockid);
}
CSocket* CSocket::tcpaccept(int mode)
{
	if(sockid<0)return NULL;
	int sock2;
	if((sock2 = accept(sockid, (sockaddr *)&SenderAddr, (socklen_t*)&SenderAddrSize)) != INVALID_SOCKET)
	{
		CSocket*sockit = new CSocket(sock2);
		if(mode >=1)sockit->setsync(1);
		return sockit;
	}
	return NULL;
}

char* CSocket::tcpip()
{
	if(sockid<0)return NULL;
	if(getpeername(sockid, (sockaddr *)&SenderAddr, (socklen_t*)&SenderAddrSize) == SOCKET_ERROR)return NULL;
	return inet_ntoa(SenderAddr.sin_addr);
}

void CSocket::setnagle(bool enabled)
{
	if(sockid<0)return;
	setsockopt(sockid, IPPROTO_TCP, TCP_NODELAY,(char*)&enabled, sizeof(bool));
}

bool CSocket::tcpconnected()
{
	if(sockid<0)return false;
	char b;
	if(recv(sockid, &b, 1, MSG_PEEK) == SOCKET_ERROR)
		return false;
	return true;
}

int CSocket::setsync(int mode)
{
	if(sockid < 0)return -1;
	u_long i = mode;
	return ioctl(sockid, FIONBIO, &i);
}

bool CSocket::udpconnect(int port, int mode)
{
	sockaddr_in addr;
	if((sockid = socket(AF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
        return false;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if(mode)setsync(1);
	if(bind(sockid,(sockaddr *)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		closesocket(sockid);
		return false;
	}
	udp = true;
	return true;
}

int CSocket::sendmessage(char *ip, int port, CBuffer *source)
{

	if(sockid<0)return -1;
	int size = 0;
	sockaddr_in addr;
	if(udp)
	{
		size = std::min(source->count, 8195);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip);
		size = sendto(sockid, source->data, size, 0, (sockaddr *)&addr, sizeof(sockaddr_in));
	}
	else
	{
		CBuffer sendbuff;
		sendbuff.clear();
		if(format == 0)
		{
			sendbuff.writeushort(source->count);
			sendbuff.addBuffer(source);
			size = send(sockid, sendbuff.data, sendbuff.count, 0);
		}else if(format == 1)
		{
			sendbuff.addBuffer(source);
			sendbuff.writechars(formatstr);
			size = send(sockid, sendbuff.data, sendbuff.count, 0);
		}else if(format == 2)
			size = send(sockid, source->data, source->count, 0);
	}
	if(size == SOCKET_ERROR)return SOCKET_ERROR;
	return size;
}

int CSocket::receivetext(char*buf, int max)
{
	int len = (int)strlen(formatstr);
	if((max = recv(sockid, buf, max, MSG_PEEK)) != SOCKET_ERROR)
	{
		int i, ii;
		for(i = 0; i < max; i ++)
		{
			for(ii = 0; ii < len; ii++)
				if(buf[i+ii] != formatstr[ii])
					break;
			if(ii == len)
				return recv(sockid, buf, i + len, 0);
		}
	}
	return -1;
}
int CSocket::receivemessage(int len, CBuffer*destination)
{
	if(sockid<0)return -1;
	int size = -1;
	char* buff = NULL;
	if(udp)
	{
		size = 8195;
		buff = new char[size];
		size = recvfrom(sockid, buff, size, 0, (sockaddr *)&SenderAddr, (socklen_t*)&SenderAddrSize);
	} else
	{
		if(format == 0 && !len)
		{
			unsigned short length;
			if(recv(sockid, (char*)&length, 2, 0) == SOCKET_ERROR)return -1;
			buff = new char[length];
			size = recv(sockid, buff, length, 0);
		} else if(format == 1 && !len)
		{
			size = 65536;
			buff = new char[size];
			size = receivetext(buff, size);
		} else if(format == 2 || len > 0)
		{
			buff = new char[len];
			size = recv(sockid, buff, len, 0);
		}
	}
	if(size > 0)
	{
		destination->clear();
		destination->addBuffer(buff, size);
	}
	if(buff != NULL)delete buff;
	return size;
}

int CSocket::peekmessage(int size, CBuffer*destination)
{
	if(sockid<0)return -1;
	if(size == 0)size = 65536;
	char *buff = new char[size];
	size = recvfrom(sockid, buff, size, MSG_PEEK, (sockaddr *)&SenderAddr, (socklen_t*)&SenderAddrSize);
	if(size < 0)
	{
		delete buff;
		return -1;
	}
	destination->clear();
	destination->addBuffer(buff, size);
	delete buff;
	return size;
}

int CSocket::lasterror()
{
	return SOCKET_ERROR;
}

char* CSocket::GetIp(char*address)
{
	sockaddr_in addr;
	hostent* hostEntry;
	if((hostEntry = gethostbyname(address)) == NULL) return NULL;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	return inet_ntoa(addr.sin_addr);
}

char* CSocket::lastinIP(void)
{
	return inet_ntoa(SenderAddr.sin_addr);
}

unsigned short CSocket::lastinPort(void)
{
	return ntohs(SenderAddr.sin_port);
}

int CSocket::SetFormat(int mode, char* sep)
{
	int previous = format;
	format = mode;
	if(mode == 1 && strlen(sep)>0)
		strcpy(formatstr, sep);
	return previous;
}

int CSocket::SockExit(void)
{
	closesocket(sockid);
	return 1;
}
int CSocket::SockStart(void)
{
	//WSADATA wsaData;
	//WSAStartup(MAKEWORD(1,1),&wsaData);
	return 1;
}

char* CSocket::myhost()
{
	static char buf[16];
	gethostname(buf, 16);
	return buf;
}
