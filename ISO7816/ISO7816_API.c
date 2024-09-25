
#include	"_17BD_UserCode.h"
#include	"..\ISO7816\ISO7816.h"
#include	<intrins.h>

const BYTE	VectorTable[0x60]={0x02,0x00,0x3B,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,
							   0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,
							   0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,
							   0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0xFF,0xE4,0xF6,0xD8,
							   0xFD,0x90,0x00,0x00,0x7F,0x1C,0x7E,0x04,0xEF,0x70,0x03,0xEE,0x60,0x08,0x0E,0xE4,
							   0xF0,0xA3,0xDF,0xFC,0xDE,0xFA,0x75,0x90,0x03,0x75,0xC3,0x01,0x02,0x80,0x00,0xFF};

								 
//====================================================
// WHEN FIRST START (WRITE FLASH)
void first_start(void){
	unsigned char	i = 0;
	ReadFlash(ROM,0x7000,0);
	if(ROM[0]==0xFF){
		for(i=0;i<85;i++){
			ROM[i] = IOBuf[i];
		}
		UpdateFlash(0x7000,ROM,255);
	}
	// clear buffer
	for(i=0;i<85;i++){
		IOBuf[i] = 0x00;
	}
}
						 

//====================================================
//	Receive one byte by UART
BYTE	receive_byte(void)
{
	UCR = 0x00;						//	UART mode,H/W check parity err,Rx
	while(!(USR&0x02))
	{;}								//	Wait for receicing
	return	UBUF;					//	Return the received byte
}

//====================================================
//	Receive datagram 48 bytes by UART
void RX_data(void){
	short	i = 0;
	short	ii = 0;
	for(i = 0; i < 48; i++){
		RXBuf[i] = receive_byte();
	}
	
	// decrypt
	if(P2==0x80){			// ecm80
		ii = 32;
	}else if(P2==0x81){		//ecm81
		ii = 48;
	}else if(P2==0x82){		// emm82
		ii = 64;
	}	
	
	// fill KEY
	for(i = 0; i < 16; i++){
		IOBuf[i] = ROM[ii+i]; // fill KEY
	}
	
	// fill block 8 bytes
	for(i = 0; i < 6; i++){
		for(ii = 0; ii < 8; ii++){
			IOBuf[ii+16] = RXBuf[i*8+ii];
		}
		// magia 3DES
		DES_Operation(1, 0x01, IOBuf, 24); // decrypt
		DES_Operation(1, 0x10, IOBuf, 24); // read
		for(ii = 0; ii < 8; ii++){
			RXBuf[i*8+ii] = IOBuf[ii+16];
		}
	}
}

//====================================================
//	Transmit one byte by UART
void	send_byte(char c)
{
	UCR = 0x20;						//	UART mode,H/W check parity err,Tx
	UBUF = c;						//	Load the transmitted byte into buffer
	while(!USR_TBE)
	{;}	
	UCR = 0x00;						//	Keep Rx mode
}

//====================================================
//  send SW
void send_sw(char sw){
	if(sw==0){
		send_byte(0x90);
		send_byte(0x00);
	}else{
		send_byte(0x98);
		send_byte(sw);
	}
}

//====================================================
//	ISO/IEC 7816 interface ISO Tx NULL byte automatically
void	ISO_AutoTxNULL(void){
		HALFW ETUcount = 7999;
		short	i = 0;
		ISOTDAT2 = ISOTRLD2 = 0x00;
		ISOTDAT0 = ISOTRLD0 = ETUcount;
		ISOTDAT1 = ISOTRLD1 = (ETUcount>>8);	//	Every ETUcount to send 1 NULL byte
		ISOTMSK |= Bit0_En;						//	ISO timing to send NULL byte interrupt disabled
		ISOTCON &= (Bit1_Dis&Bit2_Dis);			//	Counting mode
		ISOTCON |= Bit0_En;						//	Timer start
		// __nop__
		for(i=0; i<20; i++){
			_nop_();
		}
		ISOTCON &= Bit0_Dis;
		// __nop__
		for(i=0; i<10; i++){
			_nop_();
		}
}





//====================================================
//	DES operation
//	mode=P1:	0x00-DES,0x01-T-DES
//	oper=P2:	0x00-Encrypt,0x01-Decrypt,0x10-Read Result
//	len=P3
BYTE	DES_Operation(BYTE mode,BYTE oper,BYTE * desdata,USHORT len)
{

	if((mode > 0x01) || (oper > 0x10))
		return	0;								//	P1,P2 wrong
	CLKCON = 0x01;								//	Open DES clock
	if(oper != 0x10)
	{	
		DES_DATEN = 1;							//	Enable data input
		DESD0 = desdata[len-1];
		DESD1 = desdata[len-2];
		DESD2 = desdata[len-3];
		DESD3 = desdata[len-4];	
		DESD4 = desdata[len-5];
		DESD5 = desdata[len-6];
		DESD6 = desdata[len-7];
		DESD7 = desdata[len-8];
		DES_DATEN = 0;							//	Disable data input

		if(mode == 0x01)						//	Tripple DES
		{	
			DES_KEY1EN = 1;
			DESD0 = desdata[len-17];
			DESD1 = desdata[len-18];
			DESD2 = desdata[len-19];	
			DESD3 = desdata[len-20];
			DESD4 = desdata[len-21];
			DESD5 = desdata[len-22];
			DESD6 = desdata[len-23];	
			DESD7 = desdata[len-24];
			DES_KEY1EN = 0;

			DES_KEY2EN = 1;
			DESD0 = desdata[len-9];
			DESD1 = desdata[len-10];
			DESD2 = desdata[len-11];	
			DESD3 = desdata[len-12];
			DESD4 = desdata[len-13];
			DESD5 = desdata[len-14];
			DESD6 = desdata[len-15];	
			DESD7 = desdata[len-16];
			DES_KEY2EN = 0;

			if(oper == 0x00)
			DES_MODE = 0;						//	Encryption
			if(oper == 0x01)
			DES_MODE = 1;						//	Decryption
			DES_TDES = 1;						//	Tripple DES
		}
		else									//	Single DES
		{	
			DESCTL |= 0xC0;						//	Key1,Key2 enable
			DESD0 = desdata[len-9];
			DESD1 = desdata[len-10];
			DESD2 = desdata[len-11];	
			DESD3 = desdata[len-12];
			DESD4 = desdata[len-13];
			DESD5 = desdata[len-14];
			DESD6 = desdata[len-15];	
			DESD7 = desdata[len-16];
			DESCTL &= 0x3F;						//	Key1,Key2 disable

			if(oper == 0x00)
			DES_MODE = 0;						//	Encryption
			if(oper == 0x01)
			DES_MODE = 1;						//	Decryption
			DES_TDES = 0;						//	Tripple DES
		}
		RNGCTL &= 0xFE;
		DESDPACON = 0x01;						//	Open anti-DPA
		DES_START = 1;							//	Start DES
		while(!DES_END)							//	DES caculation is pending
		{;}		
		
		DESDPACON = 0x00;						//  Close anti-DPA		
	}
	else										//	Read result
	{	
		desdata[len-1] = DESD0;
		desdata[len-2] = DESD1;		
		desdata[len-3] = DESD2;
		desdata[len-4] = DESD3;
		desdata[len-5] = DESD4;
		desdata[len-6] = DESD5;
		desdata[len-7] = DESD6;
		desdata[len-8] = DESD7;
		DES_END = 0;							//	Clear DES_END
	}
	CLKCON = 0x00;								//	Close DES clock
	return 1;
}




//====================================================
//	Read flash memory
void	ReadFlash(BYTE *ramAddr, HALFW FlashAddr, HALFW  length){
	if(length == 0){
		length = 0x100;
	}
	mem_cpy(ramAddr,(BYTEX *)FlashAddr,length);
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
//	Update flash memory
void	UpdateFlash(HALFW foffset,BYTE * RAMbuf,BYTE length)
{
	HALFW	FlashAddr,FlashAddrEnd;
	HALFW	dataleft = 0,toUpdate = 0,i = 0;

	FlashAddr =  foffset;
	FlashAddrEnd = FlashAddr + length - 1;		//	End of the target address

	//***************************************************//	
//	Update RAM
	if((foffset >= RAMBase) && (foffset < RAMLimit))
	{
		mem_cpy((BYTEX *)foffset,RAMbuf,length);
		return;
	}

	//***************************************************//	
//	Update flash
	else if((FlashAddrEnd < FlashLimit))		//	Flash and OTP memory area
	{   
		mem_cpy(FlashBuffer,RAMbuf,length);
		
	    FlashAddr &= (0xFE00 | PageSize); 
	    foffset = (HALFW)foffset & (PageSize - 1);
	    
		for(dataleft = length; dataleft; dataleft  -= toUpdate)
		{
			toUpdate = (foffset + dataleft) < PageSize ? dataleft : (PageSize - foffset);
						
			mem_cpy(RAMbuf,(BYTEX *)FlashAddr,PageSize);

			mem_cpy(RAMbuf + foffset,FlashBuffer+i,toUpdate);

			i += toUpdate;
				
			if((SWptr = Erase_Page((BYTEX *)FlashAddr)) != SUCCESS)
			{
				return;			
			}
			if((SWptr = Write_Bytes_Page((BYTEX *)FlashAddr,RAMbuf)) != SUCCESS)
			{
				return;			
			}
					
			foffset = (BYTE)(toUpdate + foffset) & (PageSize - 1);
			FlashAddr += PageSize;
		}
	}
	else
	{
		SWptr = P1P2ERR;						//	6A00
	}
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

//====================================================
//	Change bank
/*void	SetBase(BYTE BankNum)
{
	if(P3 != 0x00)
	{
		SWptr = P3ERROR;
		return;
	}
	if(BankNum < 8)
	{
		MMU_SEL = 0x01;
		rP3	= BankNum; 				//	Set P3
	}
	else
	{
		MMU_SEL = 0x00;
		SWptr = P1P2ERR;			//	P1,P2 error
	}	
}
*/

