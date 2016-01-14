


#include "stdafx.h"
#include "TcpStream.h"

TcpStream::TcpStream(char * OutputName,u32 FlowID)
{
	this->SeqNo =0;
	this->BufferListPos = 0;
	this->BufferListMax = 1*1024;
	this->fd = -1;
	this->FlowID = FlowID;
	this->isFileCreate = false;
	this->WritePos = 0;
	strcpy(Path,OutputName);
	

}

void	TcpStream::TCPStream_Open()
{
	// 以二进制方式打开
    int Flags = O_CREAT | O_RDWR | O_BINARY;
    Flags |= (!isFileCreate) ? O_TRUNC : 0;


    fd = open(Path, Flags);
    if (fd < 0)
    {
        fprintf(stderr, "failed to create output file [%s] errno:%i\n", Path, errno);
        exit(-1);
	}else{
		isFileCreate = true;
	}

}

void TcpStream::TCPStream_PktAdd(ReadPcap * pcap){

	isPcapFile = TRUE;
	if(isFileCreate == false){
		TCPStream_Open();
		write(fd, &pcap->Header, sizeof(pcap->Header));
		WritePos += sizeof(pcap->Header);
	}
	{
		int rlen = write(fd,pcap->Pkt,sizeof(PCAPPacket));
		WritePos += rlen;
	
		rlen = write(fd,pcap->Pkt+1,pcap->Pkt->LengthCapture);
		WritePos += sizeof(rlen);
		TRACE(" Rlen %d ",rlen);
	}
}


TcpStream::~TcpStream(void)
{
	close(fd);
}

