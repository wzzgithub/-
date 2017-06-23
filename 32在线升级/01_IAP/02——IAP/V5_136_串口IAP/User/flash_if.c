/*
*********************************************************************************************************
*	                                  
*	ģ������ : flash_ifģ��
*	�ļ����� : flash_if.c
*	��    �� : V1.0
*	˵    �� : flash�������
*	�޸ļ�¼ :
*		�汾��    ����          ����             ˵��
*		v1.0    2015-03-05     Eric2013	      �����Թٷ�����
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "flash_if.h"


static uint32_t GetSector(uint32_t Address);

/*
*********************************************************************************************************
*	�� �� ��: FLASH_If_Init
*	����˵��: flash������ɽ���д����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void FLASH_If_Init(void)
{ 
	FLASH_Unlock(); 

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
}

/*
*********************************************************************************************************
*	�� �� ��: FLASH_If_Erase
*	����˵��: ɾ�������û�flash��
*	��    ��: StartSector  ��ʼ����
*	�� �� ֵ: 0 �û�flash���ɹ�ɾ��
*             1 ���ʹ���
*********************************************************************************************************
*/
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
	uint32_t UserStartSector = FLASH_Sector_1, i = 0;

	/* �û�flash�����ڵ����� */
	UserStartSector = GetSector(APPLICATION_ADDRESS);

	for(i = UserStartSector; i <= FLASH_Sector_11; i += 8)
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
		{
			return (1);
		}
	}
  
	return (0);
}

/*
*********************************************************************************************************
*	�� �� ��: FLASH_If_Write
*	����˵��: ��flash��д���ݣ�4�ֽڶ���
*	��    ��: FlashAddress flash��ʼ��ַ
*             Data         ���ݵ�ַ
*             DataLength   ���ݳ��ȣ�4�ֽڣ�
*	�� �� ֵ: 0 �ɹ�
*             1 дflash�����г���
*             2 д��flash�е����ݸ������Ĳ���ͬ
*********************************************************************************************************
*/
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
	uint32_t i = 0;

	for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
	{
		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
		be done by word */ 
		if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
		{
			/* ���д������� */
			if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
			{
				/* �����ĺ�д��Ĳ���ͬ */
				return(2);
			}
			
			/* ��ַ���� */
			*FlashAddress += 4;
		}
		else
		{
			/* ��flash��д����ʱ�������� */
			return (1);
		}
	}

	return (0);
}

/*
*********************************************************************************************************
*	�� �� ��: FLASH_If_GetWriteProtectionStatus
*	����˵��: �û�flash����д����״̬
*	��    ��: ��
*	�� �� ֵ: 0 �û�flash����û��д����
*             1 �û�flash��ĳЩɽ������д����
*********************************************************************************************************
*/
uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
	uint32_t UserStartSector = FLASH_Sector_1;

	/* �û�flash�����ڵ����� */
	UserStartSector = GetSector(APPLICATION_ADDRESS);

	/* ����û�flash���е�ĳЩ�����Ƿ���д���� */
	if ((FLASH_OB_GetWRP() >> (UserStartSector/8)) == (0xFFF >> (UserStartSector/8)))
	{ 
		/* û��д���� */
		return 1;
	}
	else
	{ 
		/* ĳЩ������д���� */
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: FLASH_If_DisableWriteProtection
*	����˵��: �⿪д����
*	��    ��: ��
*	�� �� ֵ: 0 �Ᵽ���ɹ�
*             1 �Ᵽ��ʧ��
*********************************************************************************************************
*/
uint32_t FLASH_If_DisableWriteProtection(void)
{
	__IO uint32_t UserStartSector = FLASH_Sector_1, UserWrpSectors = OB_WRP_Sector_1;

	/* �û�flash�����ڵ����� */
	UserStartSector = GetSector(APPLICATION_ADDRESS);

	UserWrpSectors = 0xFFF-((1 << (UserStartSector/8))-1);

	/* ���� Option Bytes */
	FLASH_OB_Unlock();

	/* �����û������Ᵽ�� */
	FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);

	/* ���� Option Bytes ���. */  
	if (FLASH_OB_Launch() != FLASH_COMPLETE)
	{
		/* �Ᵽ��ʧ�� */
		return (2);
	}

	/* �Ᵽ���ɹ� */
	return (1);
}

/*
*********************************************************************************************************
*	�� �� ��: GetSector
*	����˵��: ���ݵ�ַ���������׵�ַ
*	��    ��: Address  ָ����ַ
*	�� �� ֵ: �����׵�ַ
*********************************************************************************************************
*/
static uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;  
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_Sector_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;  
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
	{
		sector = FLASH_Sector_11;  
	}
	return sector;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
