#include 	 "ymodem.h"

// 1 for malloc
// 0 for file

typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;

    /* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;

#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)



#define NAK_TIMEOUT             (0x300000)
#define MAX_ERRORS              (5)


unsigned char Eot_Send=0;           /*if control program has send "C" to request send,then it should be set to 1*/
unsigned char FirstPackSend=0;
unsigned char SOH_Idx=0;	        /*SOH_Idx:1 128 bytes; 2:1024 bytes(STX:02)*/
unsigned int blk_num_send=0;
unsigned char file_send_end=0;       /*indicate the file has been sended*/
unsigned char file_send_end_ACK=0;   /*indicate the file has been sended and receiver responsed*/
unsigned char file_name_send_ACK=0;  /*indicate the file name packet has been sended*/
unsigned char FirstSend=1;           /*indicate the file name packet start to be sended*/
unsigned char PacketStartFlagSend=0; /*indicate the file content packet start to be sended*/
unsigned char NoWait=0;              /*if get 'G' command, no wait the next cmd*/
unsigned char No_More_File=0;        /*indicate all the files has been sended*/
unsigned char Empty_Ind=0;

unsigned int NumBlks;	             /*the number of blocks of the file*/
unsigned int LenLastBlk;             /*length of the last block*/
unsigned char SOHLastBlk;            /*the SOH ind of the last block*/

int fp_file_bin;
int get_filedata(char* file_name,uint8_t *p_data,int maxread);

int get_filelen(char* file_name);
char* get_file_name(char* file_name);
int real_send(char* file_name);
int procsend(char* send_buf,char* file_name);
int Procpacket(char* send_buf,char* file_name);


unsigned long int USER_FLASH_SIZE;

static  int fd;


int FileLen;
void Int2Str(uint8_t* str, int32_t intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}


int32_t Receive_Byte (uint8_t *c, uint32_t timeout)
{
	timeout = 1000*timeout;
	//timeout = 100*timeout;
  while (timeout-- > 0)
  {
    printf("helle:%d\n",fd);
    if((read(fd,c,1)) == 1)
    {
    // if(*c == ACK ||*c ==  NAK)
    //	printf("0x%02x",*c);
	 //else
	 	printf("%c",*c);
	//printf("%c",*c);
      return 0;
    }
  }
  printf("line=%d read err",__LINE__);
  return -1;
}


static uint32_t Send_Byte (uint8_t c)
{
  //SerialPutChar(c);
  write(fd,&c,1);
  return 0;
}


int32_t Ymodem_CheckResponse(uint8_t c)
{
  return 0;
}


void Ymodem_PrepareIntialPacket(uint8_t *data, const uint8_t* fileName, uint32_t *length)
{
  uint16_t i, j;
  uint8_t file_ptr[10];
  

  data[0] = SOH;
  data[1] = 0x00;
  data[2] = 0xff;
  

  for (i = 0; (fileName[i] != '\0') && (i < FILE_NAME_LENGTH);i++)
  {
     data[i + PACKET_HEADER] = fileName[i];
  }

  data[i + PACKET_HEADER] = 0x00;
  
  Int2Str (file_ptr, *length);
  for (j =0, i = i + PACKET_HEADER + 1; file_ptr[j] != '\0' ; )
  {
     data[i++] = file_ptr[j++];
  }
  
  for (j = i; j < PACKET_SIZE + PACKET_HEADER; j++)
  {
    data[j] = 0;
  }
}


void Ymodem_PreparePacket(uint8_t *SourceBuf, uint8_t *data, uint8_t pktNo, uint32_t sizeBlk)
{
  uint16_t i, size, packetSize;
  uint8_t* file_ptr;
  

  packetSize = sizeBlk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
  size = sizeBlk < packetSize ? sizeBlk :packetSize;
  if (packetSize == PACKET_1K_SIZE)
  {
     data[0] = STX;
  }
  else
  {
     data[0] = SOH;
  }
  data[1] = pktNo;
  data[2] = (~pktNo);
  file_ptr = SourceBuf;
  

  for (i = PACKET_HEADER; i < size + PACKET_HEADER;i++)
  {
     data[i] = *file_ptr++;
  }
  if ( size  <= packetSize)
  {
    for (i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++)
    {
      data[i] = 0x1A; /* EOF (0x1A) or 0x00 */
    }
  }
}


uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
  uint32_t crc = crcIn;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}



uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = data+size;

  while(data < dataEnd)
    crc = UpdateCRC16(crc, *data++);
 
  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc&0xffffu;
}


uint8_t CalChecksum(const uint8_t* data, uint32_t size)
{
  uint32_t sum = 0;
  const uint8_t* dataEnd = data+size;

  while(data < dataEnd )
    sum += *data++;

  return (sum & 0xffu);
}


void Ymodem_SendPacket(uint8_t *data, uint16_t length)
{
  uint16_t i;
  i = 0;
  while (i < length)
  {
    Send_Byte(data[i]);
    i++;
  }
}


uint8_t Ymodem_Transmit (uint8_t  *buf, const uint8_t* sendFileName, uint32_t sizeFile)
{
  
  uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
  uint8_t filename[FILE_NAME_LENGTH];
  uint8_t *buf_ptr, tempCheckSum;
  uint16_t tempCRC;
  uint16_t blkNumber;
  uint8_t receivedC[2], CRC16_F = 0, i;
  uint32_t errors, ackReceived, size = 0, pktSize;

  errors = 0;
  ackReceived = 0;
  

 // for (i = 0; i < (FILE_NAME_LENGTH - 1); i++)
  for (i = 0; sendFileName[i] != '\0'; i++)
  {
    filename[i] = sendFileName[i];
    //printf("%d",i);
  }
  filename[i]  = '\0';
  CRC16_F = 1;
    

  Ymodem_PrepareIntialPacket(&packet_data[0], filename, &sizeFile);
  do 
  {

    Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER);
 

    if (CRC16_F)
    {
       tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
       Send_Byte(tempCRC >> 8);
       Send_Byte(tempCRC & 0xFF);
	  
    }
    else
    {
       tempCheckSum = CalChecksum (&packet_data[3], PACKET_SIZE);
       Send_Byte(tempCheckSum);
	  
    }
	
  

    if (Receive_Byte(&receivedC[0], 10000) == 0)  
    {
		
        if (receivedC[0] == ACK)
       { 
			printf("<----ACK",__LINE__);
         ackReceived = 1;
	   Receive_Byte(&receivedC[0], 10000) ;
	   
	   printf("line=%d ,ackReceived \r\n",__LINE__); 
       }
	}	
	  //应该是CRC16 43
    else
       errors++;
  }while (!ackReceived && (errors < 0x1A));
  
  if (errors >=  0x1A)
  {
  	printf("errors line=%d r\n",__LINE__); 
    return errors;
  }
  buf_ptr = buf;
  size = sizeFile;
  blkNumber = 0x01;

  
  

  while (size)
  {
  	
	 printf("size = %d,blkNumber = %d\r\n",size,blkNumber);

    Ymodem_PreparePacket(buf_ptr, &packet_data[0], blkNumber, size);
    ackReceived = 0;
    receivedC[0]= 0;
    errors = 0;
    do
    {
      if (size >= PACKET_1K_SIZE)
      {
        pktSize = PACKET_1K_SIZE;
      }
      else
      {
        pktSize = PACKET_SIZE;
      }
	  
      Ymodem_SendPacket(packet_data, pktSize + PACKET_HEADER);
      if (CRC16_F)
      {
         tempCRC = Cal_CRC16(&packet_data[3], pktSize);
         Send_Byte(tempCRC >> 8);
         Send_Byte(tempCRC & 0xFF);
      }
      else
      {
        tempCheckSum = CalChecksum (&packet_data[3], pktSize);
        Send_Byte(tempCheckSum);
      }


      if ((Receive_Byte(&receivedC[0], 100000) == 0)  && (receivedC[0] == ACK))
      {
		  printf("<----ACK",__LINE__);
        ackReceived = 1;  
        if (size > pktSize)
        {
           buf_ptr += pktSize;  
           size -= pktSize;
            blkNumber++;
        }
        else
        {
          buf_ptr += pktSize;
          size = 0;
        }
      }
      else
      {
        errors++;
		printf("errors++...\n",__LINE__);
	  }
    }while(!ackReceived && (errors < 0x1A));

    
    if (errors >=  0x1A)
    {
       printf("line=  %d,errors\r\n",__LINE__);	
      return errors;
    }
    
  }

  //
  //
  //
  //
  printf("line=%d,blkNumber=%d\r\n",__LINE__,blkNumber);
  printf("line=%dSend EOT \r\n",__LINE__);
  
  ackReceived = 0;
  receivedC[0] = 0x00;
  errors = 0;
  do 
  {
    Send_Byte(EOT);

    if ((Receive_Byte(&receivedC[0], 10000) == 0)  && receivedC[0] == ACK)
    {
		printf("<----ACK",__LINE__);
      ackReceived = 1;  
    }
    else
    {
      errors++;
    }
  }while (!ackReceived && (errors < 0x1A));
    
  if (errors >=  0x1A)
  {
  	printf("linr=%d errors\r\n",__LINE__);
    return errors;
  }


  ackReceived = 0;
  receivedC[0] = 0x00;
  errors = 0;

  packet_data[0] = SOH;
  packet_data[1] = 0;
  packet_data [2] = 0xFF;

  for (i = PACKET_HEADER; i < (PACKET_SIZE + PACKET_HEADER); i++)
  {
     packet_data [i] = 0x00;
  }
  
  do 
  {
    Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER);


    tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
    Send_Byte(tempCRC >> 8);
    Send_Byte(tempCRC & 0xFF);
  

    if (Receive_Byte(&receivedC[0], 10000) == 0)  
    {
      if (receivedC[0] == ACK)
      { 
  printf("<----ACK",__LINE__);
        ackReceived = 1;
      }
    }
    else
    {
        errors++;
    }
  }while (!ackReceived && (errors < 0x1A));


  if (errors >=  0x1A)
  {
  	printf("line=%d errors\r\n",__LINE__);
    return errors;
  }  

  //
  //
  //ackReceived = 0;
 //receivedC[0] = 0x00;
  //errors = 0;
  //printf("line=%dSend EOT \r\n",__LINE__);
  //
  //

  
  do 
  {
    Send_Byte(EOT);
    if ((Receive_Byte(&receivedC[0], 10000) == 0)  && receivedC[0] == ACK)
    {
		printf("<----ACK",__LINE__);
      ackReceived = 1;  
    }
    else
    {
      errors++;
    }
  }while (!ackReceived && (errors < 0x1A));

  if (errors >=  0x1A)
  {
    printf("line=%d errors\r\n",__LINE__);
    return errors;
  }
  
  
  printf("line=%d file transmitted successfully\r\n",__LINE__);

  return 0; 
}



static int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio; 

	tcflush(fd, TCIOFLUSH);
	if(tcgetattr(fd, &newtio) < 0) { 
        	return -1;
	}

	newtio.c_cflag  |= CLOCAL | CREAD ;
	newtio.c_cflag  &= ~CSIZE; 
	
	// raw mode
	cfmakeraw(&newtio);//
	
	switch( nBits ){
	case 7:
        	newtio.c_cflag |= CS7;
        	break;
	case 8:
        	newtio.c_cflag |= CS8;
        	break;
	default:
		fprintf(stderr,"Unsupported data size\n");
		return -1;

	}

	switch( nEvent ){
	case 'O':
	case 'o':
		newtio.c_cflag |= (PARODD | PARENB);  /* ODD parity checking */
		newtio.c_iflag |= INPCK;              /* Disnable parity checking */    
        	break;
	case 'E':
	case 'e':	
		newtio.c_cflag |= PARENB;     /* Enable parity */    
		newtio.c_cflag &= ~PARODD;    /* EVEN parity checking */
		newtio.c_iflag |= INPCK;      /* Disnable parity checking */
        	break;		
	case 'N': 
	case 'n':
		newtio.c_cflag &= ~PARENB; /* Clear parity enable */
		newtio.c_iflag &= ~INPCK;  /* Enable parity checking */
		break;

	case 'S':
	case 's':
		newtio.c_cflag &= ~PARENB;
		newtio.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported parity\n");
		return -1;
		 
	}
	switch (nStop)
	{
	case 1:
		newtio.c_cflag &= ~CSTOPB;
		break;
	case 2:
		newtio.c_cflag |= CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported stop bits\n");
		return -1;
	}

	switch( nSpeed ){
	case 1200:
        	cfsetispeed(&newtio, B1200);
        	cfsetospeed(&newtio, B1200);
        	break;
	case 2400:
        	cfsetispeed(&newtio, B2400);
        	cfsetospeed(&newtio, B2400);
        	break;
	case 4800:
        	cfsetispeed(&newtio, B4800);
        	cfsetospeed(&newtio, B4800);
        	break;
	case 9600:
        	cfsetispeed(&newtio, B9600);
        	cfsetospeed(&newtio, B9600);
        	break;
	case 115200:
		// fall through
	default:
        	cfsetispeed(&newtio, B115200);
        	cfsetospeed(&newtio, B115200);
        	break;
	}

	// flow control OFF
	newtio.c_cflag &= ~CRTSCTS;

#if READ_BLOCK
	newtio.c_cc[VTIME] = 5; 
	newtio.c_cc[VMIN]  = COMMAND_LEN_MAX; 
#endif
	
	if((tcsetattr(fd,TCSANOW,&newtio))!=0){
        	perror("com set error");
        	return -1;
	}
	//printf("port  config done!\n");
	return 0;
}





int open_lock_controller(char *tty_dev)
{
	fd = open(tty_dev, O_RDWR | O_NOCTTY);
	//fd = open(tty_dev,_O_RDWR|_O_BINARY|_O_NONBLOCK );
	if (fd < 0)
	{
		printf("open port\r\n");
		return -1;
	}

	if (set_opt(fd, 115200, 8, 'N', 1) < 0)
	{
		close(fd);
		return -1;
	}

	return 0;
}



int  OpenUartDevice(char *device)
{
   	struct termios Opt;

	if((fd=open(device, O_RDWR))==-1)
	//	return(COM_OPEN_ERR);

	//if((fd=_open(device, _O_RDWR|_O_BINARY|_O_NONBLOCK ))==-1)
	//if((fd=_open(device, _O_RDWR|_O_BINARY))==-1)
		return  -1;
	
	tcgetattr(fd,&Opt);   //获取串口设备的相关属性
    //tcgetattr(fd1,&options);   //获取串口设备的相关属性
    cfsetispeed(&Opt,B9600); //设置串口的输入波特率为115200
    cfsetospeed(&Opt,B9600); //设置串口的输出波特率为115200
    
    Opt.c_cflag &= ~PARENB;    // 校验位 这个操作是清除它的校验位
    Opt.c_cflag &= ~CSTOPB;        // 设置停止位,  这个停止位是1位
    Opt.c_cflag &= ~CSIZE;  // 清除数据位 , 
    Opt.c_cflag |= CS8;    // 设置数据位 为8位 
     // 以上是设置uart为8N1
    Opt.c_lflag &= (~ICANON);   // 关闭标准模式
    Opt.c_lflag &= (~ECHO);


    Opt.c_lflag &= ~(ICANON | ECHO | ECHOE |ISIG);   // 关闭标准模式
Opt.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET);
    Opt.c_iflag &= ~(ICRNL | IXON);//接受特殊字符
	if (tcsetattr(fd,TCSANOW,&Opt) != 0)   
	{ 
		return -1;  
	} 
	//close(fd);
	return fd;
}




int control_send(char* file_name,char* device, char  stepcmd)
{
	char send_next;
	int send_idx;

	char cmd_ind;
	int len;
	int CAN_CNT=0;
	int CAN_CNT1=0;
	
	uint8_t *darap,data;
	int counter = 0;

	uint8_t ret = 0;
	//int fd;


	 if(OpenUartDevice(device)  == -1)   //fd打开并设置串口
	 {
	   printf("line=%d open uart device err!!!\r\n",__LINE__);
	   return -1;
	 }
	
	/*if((fd=_open(device, _O_RDWR|_O_BINARY|_O_NONBLOCK ))==-1)
		return(COM_OPEN_ERR);
*/
//fail_once_again:


    USER_FLASH_SIZE = get_filelen(file_name);
	printf("USER_FLASH_SIZE__the file len = %ld\r\n",USER_FLASH_SIZE);



	darap = (uint8_t *)malloc(USER_FLASH_SIZE);
	if(darap == NULL)
	{
		printf("line=%d err malloc\r\n",__LINE__);
		return (MALLOC_ERROR);
	}
    printf("##%d##",__LINE__);
	
	//sleep(3);
	
	if(USER_FLASH_SIZE != get_filedata(file_name,darap,USER_FLASH_SIZE))
	{
	   printf("line=%d get_filedata ERR \r\n",__LINE__);
	   free(darap);
	   return (MALLOC_ERROR);
	}
	else
		{
		   printf("line=%d get_filedata OK,LEN=%d \r\n",__LINE__,USER_FLASH_SIZE);
		}

	

	while(1)
      {
         Receive_Byte(&data, 10000);
		 if(data == 'C')
			  printf("<----C",__LINE__);
	    	break;
		 Receive_Byte(&data, 10000);
		 if(data == 'C')
			 printf("<----C",__LINE__);
	    	break;
				
				
	  }
	
	ret = Ymodem_Transmit(darap,file_name,USER_FLASH_SIZE);





	if(ret == 0)
	{
	  printf("down ok\r\n");
	 
	}
	else
	{
	   printf("down fail\r\n");
	   printf("down once again\n");
	   printf("sleep 5s\r\n");
	   sleep(5);
	  // goto  fail_once_again;
	}
	
	


	free(darap);



   return ret;
	
	
}

int get_filedata(char* file_name,uint8_t *p_data,int maxread)
{
	FILE* fp_send;
	int c,i = 0;
	uint8_t *p_tmp;
	
	if( (fp_send = fopen( file_name, "rb" )) == NULL )
	{
		printf("can't open file %s for read!\n",file_name);
		return(FILE_OPEN_ERR);
	}

	printf("line=%d  func_get_filedata\r\n",__LINE__);
	/* Cycle until end of file reached: */
    FileLen = 0;
	p_tmp = p_data;
	i = 0;
	while((c=fgetc(fp_send))!=EOF)
		{
		*p_tmp = (uint8_t)c;
/*		
		if(i++%16 ==0) 
			 printf("\r\ni=%d:: ",i);
		printf("%02x ",*p_tmp);
*/		
		FileLen++;
		p_tmp++;
		if(i > maxread)
			{
			printf("\r\ni=%d  == maxread=%d",i,maxread);
			break;
			}
		
		}

	//fclose( fp_send );

	printf("line=%d FileLen = %d",__LINE__,FileLen);
	
	
	return(FileLen);
}



/***************************************************************/
/***************************************************************/
int get_filelen(char* file_name)
{
	FILE* fp_send;
	int c;
	
	if( (fp_send = fopen( file_name, "rb" )) == NULL )
	{
		printf("can't open file %s for read!\n",file_name);
		return(FILE_OPEN_ERR);
	}
	
	/* Cycle until end of file reached: */
	FileLen = 0;

	while((c=fgetc(fp_send))!=EOF)
		FileLen++;


	//fclose( fp_send );


	printf("line=%d FileLen = %d",__LINE__,FileLen);
	
	return(FileLen);
}


/***************************************************************/
/***************************************************************/
/*need be freed*/
char* get_file_name(char* file_name)
{
	unsigned int real_name_len=0,name_len;
	unsigned int i;
	char* filename_real;
	char seperater;


	name_len=strlen(file_name);
	
	for(i=1;i<name_len+1;i++)
	{
		if(*(file_name+(name_len-i))!=seperater)
		{
			real_name_len++;
		}
		else
		{
			break;
		}
	}
	
	if(!real_name_len)
	{
		return((char*)FILE_UNVILID);
	}	
	
	if((filename_real=(char*)malloc(real_name_len+1))==NULL)
	{
		return((char*)MALLOC_ERROR);
	}

	for(i=1;i<real_name_len+1;i++)
	{
		filename_real[real_name_len-i]=*(file_name+(name_len-i));
	}
	filename_real[real_name_len]='\0';
	
	return(filename_real);
}







int put_char(int fd, unsigned char c)
{
        int ret = 0;

        ret = write(fd, &c, 1);
        if (ret < 0)
                perror("write() in put_char()");

        return ret;
}

char get_char(int fd)
{
        int len = 0;
        char c;

        len = read(fd, &c, 1);
        if (len < 0)
                return 0;

        return c;
}

//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void uart_send(int fun,int*data,int len)
{
	int send_buf[32];
	int i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X01;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)Send_Byte(send_buf[i]);	//发送数据到串口1 
}
int	 UART_RecvBufBck[20];
int  test_data[20];
int UART_RecvFin = 0;
void uart_recv()
{
	unsigned char rCh;
	static char rCnt = 0; 
	int  UART_RecvBuf[20];
	
		rCh = get_char(fd);
		UART_RecvBuf[rCnt]=rCh;
		if(rCnt == 0)     //帧头0xAA      
        {  
            rCnt = (0x02 != rCh)?0:rCnt+1; 
        }  
        else if(rCnt == 1) //帧头0x55    
        {  
			rCnt = (0x01 != rCh)?0:rCnt+1;  
        }   
        else if(rCnt == 2) //长度len  
        {  
			rCnt++;  
        }             
        else if(rCnt > 2) //值value  
        {  
			rCnt++;  
			if(rCnt == 4+UART_RecvBuf[2]) // 
			{               
				rCnt = 0;  
				memcpy(UART_RecvBufBck,UART_RecvBuf,80);//缓冲 
				UART_RecvFin = 1;  //通知主循环处理  
			}  
        }  
}
int test_data32(char* device)
{
	int data_recv_ab[20]={0xab,0x01,0x02};//ab 水下发送参数
	int data_recv_ac[20]={0xac,0x03,0x04};//ac 告诉水下开始测量
	int handshake1[20]={0xaa};//aa 水下握手
	int i;
    int h = 0;
	if(OpenUartDevice(device)  == -1)   //fd打开并设置串口
	 {
	   printf("line=%d open uart device err!!!\r\n",__LINE__);
	   return -1;
	 }
	 /****握手******/
	uart_send(0x02,handshake1,1);
    while(h == 0)
    {
	    uart_recv();
	    if(UART_RecvFin == 1)  
        {  
		    switch(UART_RecvBufBck[3])
		    {
			    case 0xaa:			//收到aa表示与水下握手成功 开始发送参数
				    uart_send(0x02,data_recv_ab,3);
				    UART_RecvFin = 0;
				    break;
			    case 0xab:			//水下收到参数   告诉水下开始测量
				    uart_send(0x02,data_recv_ac,3);
				    UART_RecvFin = 0;
				    break;
			    case 0xac:			//水下测量完成 进行数据保存
				    memcpy(test_data,UART_RecvBufBck,80);
				    UART_RecvFin = 0;
                    h = 1;
				    break;		
		    }
        }
    }
	

	return 0;
}
//void(*b(void*))(int *a)
