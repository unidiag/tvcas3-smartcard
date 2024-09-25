
#include	"_17BD_UserCode.h"

#define CHGBASE		0x50	//	Change bank
#define	WRFLASH		0x58	//	Write flash
#define	ERFLASH		0x2E	//	Erase flash
#define	ERFLASHFAST	0x3E	//	Erase flash
#define	CRCCALL		0x30	//	Read CRC data
#define	RDFLASH		0x5A	//	Read flash
#define	UDFLASH		0x5C	//	Updata flash
#define	DESOPER		0x56	//	DES operation: Encryption/Decrytion/Read result
#define	GETRDMN		0x84	//	Get random number
#define RETURNBL 	0xF0	//	Return to bootloader

//	Define Status Word
#define	SUCCESS	0		//	Success,9000
#define	IVDINS	2		//	Invalid INS,6D00
#define	P3ERROR	4		//	P3 error,6C00
#define	WRERROR	6		//	Flash write error,6501
#define	P1P2ERR	8		//	P1 and/or P2 error,6A00
#define	RDERROR	10		//	Verify error,6504
#define IVDCLA	12		//	Invalid instruction class,6E00
#define	FlashStart	0x00000
#define	FlashLimit	0x10000
#define	RAMBase		0x0000
#define	RAMLimit	0x0800
#define	PageSize	0x200
#define	BlockSize	0x800
#define	RowSize		0x100
#define	CLA gCommand[0]
#define	INS gCommand[1]
#define	P1  gCommand[2]
#define	P2  gCommand[3]
#define	P3  gCommand[4]
extern BYTEX  gCommand[];
extern BYTEX	RXBuf[48];
extern BYTEX	IOBuf[512];
extern BYTEX	ROM[512];
extern BYTEX	ERASER[4];
extern BYTE		SWptr;
extern HALFWX	Foffset;
extern BYTEX	PPSFlag;
extern BYTEX	FlashBuffer[512];
extern BYTEX	ValidFIDI[];
BYTE	receive_byte(void);
void	send_byte(char c);
void	send_sw(char sw);
void  first_start(void);
void	ReadFlash(BYTE *ramAddr, HALFW FlashAddr, HALFW  length);
void	UpdateFlash(HALFW foffset,BYTE * RAMbuf,BYTE length);
BYTE	DES_Operation(BYTE mode,BYTE oper,BYTE * desdata,USHORT len);
BYTE	Write_Bytes_Page(BYTEX * pDest,BYTEX * pSrc);
BYTE	Erase_Page(BYTEX * pDest);
void	ISO_AutoTxNULL(void);
void  RX_data(void);
