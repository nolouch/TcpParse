#pragma once

#include "ParseType.h"
#include "ReadPcap.h"

#define TCP_FLAG_SYN(a) ((a >>(8+1))&1)

typedef struct
{
	u32		SeqNo;
	u32		Length;

	u8*		Payload;
}TCPBuffer;

class TcpStream
{
private:
	int			fd;
	FILE*		F;
	
	u32			SeqNo;
	u32			FlowID;

	u32			BufferListPos;
	u32			BufferListMax;
	TCPBuffer*	BufferList[1*1024];

	char		Path[1024];

	bool		isFileCreate;
	bool		isPcapFile;

	u64			WritePos;


	
	
public:
	TcpStream(char* OutputName,u32 FlowID);	
	~TcpStream(void);

	void TCPStream_Open();
	void TCPStream_PacketAdd(TCPHeader* TCP, u32 Length, u8* Payload);
	void TCPStream_OutputPayload(u32 Length, u8 * PayLoad);
	void TCPStream_PktAdd(ReadPcap * pcap);
};

