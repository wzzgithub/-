#include 	 "ymodem.h"

//int fd;
char CrcFlag=0;
unsigned long  int FileLen=0;
unsigned long  int FileLenBkup=0;


int main()
{
	char* fp;
	int trans_ind;
	char* device;

	device="/dev/ttyS2";
	fp="RTC.bin";

	//set_com_common(device);
	trans_ind=control_send(fp,device);
	
	if(trans_ind)
	{
		printf("trans_ind:0x%x\n",trans_ind);
		printf("sorry! get error in transmission\n");
	}
	return 0;
}


