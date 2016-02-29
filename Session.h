#include "stdafx.h"
#include "Stream.h"

struct SOCKET_DATA {
	SOCKET				socket;
	SOCKADDR_IN			addrInfo;
};

typedef enum {
	IO_READ,
	IO_WRITE,
	IO_ERROR,
} IO_OPERATION;
#define IO_DATA_MAX     (2)

class IoData {
	OVERLAPPED		overlapped;
	IO_OPERATION	ioType;
	size_t  		totalBytes;
	size_t			currentBytes;
	array<char, SOCKET_BUF_SIZE> buffer;
public:
	IoData();

	void clear();

	bool needMoreIO(size_t transferSize);
	int32_t setupTotalBytes();
	size_t totalByte();

	IO_OPERATION &type();
	void setType(IO_OPERATION type);

	WSABUF wsabuf();
	char *data();
	bool setData(Stream &stream);
	LPWSAOVERLAPPED Overlapped();
};



class Session
{
private:
	SOCKET_DATA		    socketData;
	UINT64				id;
	int8_t				type_;
	bool				setSocketOpt();
public:
	Session();
	~Session();
	bool onAccept(SOCKET socket, SOCKADDR_IN addrInfo);
	//void sendPacket(Packet *packet);
	//Package* onRecv(size_t transferSize);
	void recvStandBy() {};
	void onClose();

	SOCKET&	socket();
	string clientAddress();

	UINT Id();
	void setId(UINT id);

	int8_t type();
	void setType(int8_t type);

	array<IoData, IO_DATA_MAX> ioData;

private:
	void			initialize();

	void			checkErrorIO(DWORD ret);

	void			recv(WSABUF wsaBuf);
	bool			isRecving(size_t transferSize);

	void			send(WSABUF wsaBuf);
};