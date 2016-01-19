
#include "stdafx.h"
#include "TcpStream.h"
#include <sys/stat.h>
 #include <fcntl.h>
#include <sys/types.h>
#include<assert.h>
TcpStream::TcpStream(char * OutputName,u32 FlowID)
{
	this->SeqNo =0;
	this->BufferListPos = 0;
	this->BufferListMax = 1*1024;
	this->fd = -1;
	this->FlowID = FlowID;
	this->isFileCreate = false;
	this->WritePos = 0;
	this->F = NULL;
	memset(BufferList,0,sizeof(BufferList));
	strcpy(Path,OutputName);
	

}

void	TcpStream::TCPStream_Open()
{
	// 以二进制方式打开
    int Flags =  O_RDWR | O_BINARY |O_CREAT ;
    Flags |= (!isFileCreate) ? O_TRUNC : 0;

    fd = open(Path, Flags,742);
	
	//this->F = fopen(Path,"wb+");
	
	//if(this->F == NULL)
	if (fd<0)
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
		//fwrite(&pcap->Header,1,sizeof(pcap->Header),this->F);
		WritePos += sizeof(pcap->Header);
	}
	{
		int rlen = write(fd,pcap->Pkt,sizeof(PCAPPacket));
		//int rlen = fwrite(pcap->Pkt,1,sizeof(PCAPPacket),this->F);
		WritePos += rlen;
	
		rlen = write(fd,pcap->Pkt+1,pcap->Pkt->LengthCapture);
		//rlen = fwrite(pcap->Pkt+1,1,pcap->Pkt->LengthCapture,this->F);
		WritePos += rlen;
		TRACE(" Rlen %d ",rlen);
	}
}



void TcpStream::TCPStream_OutputPayload(u32 Length, u8 * PayLoad){
	if(isFileCreate == false){
		TCPStream_Open();
	}
	this->SeqNo += Length;
	{
		int rlen = write(fd,PayLoad,Length);
//		int rlen = fwrite(PayLoad,1,Length,this->F);
		WritePos += rlen;
	}

}

void TcpStream::TCPStream_PacketAdd(TCPHeader* TCP, u32 Length, u8* Payload){
	if(TCP_FLAG_SYN(TCP->Flags)){
		this->SeqNo = swap32(TCP->SeqNo)+1;
		for(int i=0;i<BufferListPos;i++)
		{
			TCPBuffer * B = BufferList[i];
			//free(B->Payload);
			B->Payload = NULL;
			free(B);

		}
		this->BufferListPos = 0;
	
	}

	if (Length != 0){
		u32 seqno = swap32(TCP->SeqNo);
		s32 dSeq =  this->SeqNo  - seqno;
		if (dSeq == 0){
			//对齐SeqNo  则输出
			TCPStream_OutputPayload(Length,Payload);
			while(true){
				//将Buffer里面的冗余删除和对齐包输出
				bool Hit = false;
				for(int i = 0;i<BufferListPos;i++){
					TCPBuffer * B = BufferList[i];
					if(B->SeqNo == this->SeqNo){
						Hit = true;
						TCPStream_OutputPayload(B->Length,B->Payload);
					}

					if(B->SeqNo < this->SeqNo){
						Hit = true;
					}

					if(Hit == true){
						//free(B->Payload);
						B->Payload = NULL;
						free(B);
						 for (int j=i; j < BufferListPos; j++)
                            {
                                BufferList[j] = BufferList[j+1];
                            }
                            BufferListPos--;
						break;
							
					}
				}
				if(!Hit) break;
			}
		}else{
			s32 dRemain = (seqno+Length) - this->SeqNo;
			//数据包有重叠部分
			if((dRemain > 0)&&(seqno<this->SeqNo)&&(dRemain<1500)){
				s32 PayloadOffset = this->SeqNo - seqno;
				assert(PayloadOffset > 0);
				TCPStream_OutputPayload(dRemain,Payload+PayloadOffset);
			}
			else if(this->BufferListPos<this->BufferListMax )
			{
				TCPBuffer* B = (TCPBuffer*)malloc( sizeof(TCPBuffer));
				memset(B, 0, sizeof(TCPBuffer));
				B->Length = Length;
				B->SeqNo = seqno;
				B->Payload  = Payload;
				this->BufferList[ this->BufferListPos++ ] = B;

				if(this->SeqNo == 0)
                {
                    this->SeqNo = swap32(TCP->SeqNo) + Length;
                }
			}


		}
		
	
	}


}

TcpStream::~TcpStream(void)
{
	if(fd>0)
		close(fd);
	if(F != NULL)
		fclose(F);

}

