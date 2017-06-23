#include 	 "ymodem.h"


char CrcFlag=0;
unsigned long  int FileLen=0;
unsigned long  int FileLenBkup=0;


int main()
{
	char* fp;
	int trans_ind;
	char* device;

	device="/dev/ttyS3";
	fp="RTC.bin";
/***********在线升级********************/
/***********************************
	//set_com_common(device);
	trans_ind=control_send(fp,device);
	if(trans_ind)
	{
		printf("trans_ind:0x%x\n",trans_ind);
		printf("sorry! get error in transmission\n");
	}
	**********************************/
	
	trans_ind = test_data32(device);
	if(trans_ind)
	{
		printf("trans_ind:0x%x\n",trans_ind);
		printf("sorry! get error in transmission\n");
	}
    printf("%x\n",test_data[0]);
    printf("%x\n",test_data[1]);
    printf("%x\n",test_data[2]);
    printf("%x\n",test_data[3]);
    printf("%x\n",test_data[4]);
    printf("%x\n",test_data[5]);
	return 0;
}


