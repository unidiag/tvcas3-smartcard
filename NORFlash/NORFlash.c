
#include	"_17BD_UserCode.h"
#include	"..\ISO7816\ISO7816.h"
#include	"..\NORFlash\NORFlash.h"

//====================================================
//	Erase the target sector	in fast mode
BYTE	Erase_Page_Fast(BYTEX * pDest)
{
	BYTE	counter = 0;	

	FLCON = 0x33;						    //	0x13-1ms+NRD;0x11-4ms+NRD;0x33-1ms+VRD

retryErase_Fast:
	if(counter == 4)
		FLCON =0x13;						//   1ms+NRD

	FLSDP1 = 0x55;
	FLSDP2 = 0xAA;

	*pDest = 0xFF;							//	Write a FFH to any address in the target sector

	while(!FLSTS_F_OVER);					//	FL_CTL.FL_OVER	
	
	FLSTS_F_OVER = 0;						//	Clear FL_CTL.FL_OVER
	if(FLSTS_F_OP_ERR)						//	Mistaken operation
	{							   	
		FLSTS_F_OP_ERR = 0;
		return WRERROR;
	}
	else if(FLSTS_F_CHKFF_ERR)
	{
		FLSTS_F_CHKFF_ERR = 0;

		if(counter == 4)
		{
			return RDERROR;
		}	

		counter ++;
		goto	retryErase_Fast;
	}
	return	SUCCESS;
}

//====================================================
//	Erase the target sector	in normal mode
BYTE	Erase_Page(BYTEX * pDest)
{
	BYTE	counter = 0;

	FLCON = 0x11;

retryErase_Normal:	
	FLSDP1 = 0x55;
	FLSDP2 = 0xAA;

	*pDest = 0xFF;							//	Write a FFH to any address in the target sector

	while(!FLSTS_F_OVER);					//	FL_CTL.FL_OVER	
	
	FLSTS_F_OVER = 0;						//	Clear FL_CTL.FL_OVER
	if(FLSTS_F_OP_ERR)						//	Mistaken operation
	{							   	
		FLSTS_F_OP_ERR = 0;
		return WRERROR;
	}
	else if(FLSTS_F_CHKFF_ERR)
	{
		FLSTS_F_CHKFF_ERR = 0;

		if(counter == 2)
			return	RDERROR;

		counter ++;
		goto	retryErase_Normal;
	}
	return	SUCCESS;
}
//====================================================
//	Write n bytes of flash memory.
BYTE	Write_Bytes(BYTEX * pDest,BYTEX * pSrc,HALFW len)
{
	HALFW	i;

 	for(i = 0;i < len;i ++)
	{
		FLCON = 0x01;
		FLSDP1 = 0xAA;
		FLSDP2 = 0x55;	

		*(pDest + i) = pSrc[i];

		while(!FLSTS_F_OVER);					//	FL_CTL.FL_OVER	
	
		FLSTS_F_OVER = 0;						//	Clear FL_CTL.FL_OVER
		if(FLSTS_F_OP_ERR)						//	Mistaken operation
		{							   	
			FLSTS_F_OP_ERR = 0;
			return	WRERROR;
		}
	}
	if(mem_cmp(pDest, pSrc, len) == 0)			//	Check by reading back
	{											
		return RDERROR;							//	6504
	}

	return SUCCESS;
}

//====================================================
//	Write one-Page bytes consecutively of flash memory.
BYTE	Write_Bytes_Page(BYTEX * pDest,BYTEX * pSrc)
{
	HALFW	srcaddr,i;

	srcaddr = (HALFW)pSrc;

	for(i=0;i<PageSize;i+=RowSize)
	{
		FLRPVAD = (BYTE)(srcaddr+i);
		FLRPVAD1 =	(BYTE)((srcaddr+i) >> 8);
		FLRPVLEN =	(BYTE)RowSize;
		FLRPVLEN1 = (BYTE)(RowSize >> 8);
	
		FLCON = 0x40;
		FLSDP1 = 0xAA;
		FLSDP2 = 0x55;	
	
		*(pDest+i) = pSrc[i];
	
		while(!FLSTS_F_OVER);					//	FL_CTL.FL_OVER	
		
		FLSTS_F_OVER = 0;						//	Clear FL_CTL.FL_OVER
		if(FLSTS_F_OP_ERR)						//	Mistaken operation
		{							   	
			FLSTS_F_OP_ERR = 0;
			return	WRERROR;
		}
		if(mem_cmp((pDest+i), (pSrc+i), RowSize) == 0)			//	Check by reading back
		{											
						
			return RDERROR;							//	6504
		}
	}
	return SUCCESS;
}