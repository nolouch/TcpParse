#pragma once

#include "ParseType.h"
#include "ReadPcap.h"
typedef struct
{
	u64		TS;
	u32		SeqNo;
	u32		Length;

	u8*		Payload;
}TCPBuffer;

class TcpStream
{
private:
	int			fd;
	
	u32			SeqNo;
	u32			FlowID;

	u32			BufferListPos;
	u32			BufferListMax;
	TCPBuffer*	BufferList[1*1024];

	char		Path[1024];
	u64			LastTSC;

	bool		isFileCreate;
	bool		isPcapFile;

	u64			WritePos;


	
	
public:
	TcpStream(char* OutputName,u32 FlowID);	
	~TcpStream(void);

	void TCPStream_Open();
	void TCPStream_PacketAdd();
	void TCPStream_PktAdd(ReadPcap * pcap);
};

