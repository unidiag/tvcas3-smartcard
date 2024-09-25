
#include	"_17BD_UserCode.h"
#include	"..\ISO7816\ISO7816.h"

//====================================================
//	DES finish interrutp service routine
void	DES_ISR(void)	interrupt 0
{
	IE_DES = 0;
	return;
}

//====================================================
//	Timer 0 interrutp service routine
void	TMR0_ISR(void)	interrupt 1
{
	IE_TMR0 = 0;
	return;
}

//====================================================
//	UART interrutp service routine
void	UART_ISR(void)	interrupt 2
{
	IE_UART = 0;
	return;
}

//====================================================
//	Interrutp 3 interrutp service routine
void	INT3_ISR(void)	interrupt 3
{
	IE_INT3 = 0;
	return;
}

//====================================================
//	Flash write finish interrutp service routine
void	FLASH_ISR(void)	interrupt 4
{
	IE_FLASH = 0;
	return;
}

//====================================================
//	Interrutp 5 service routine
void	INT5_ISR(void)	interrupt 5
{
	IE_INT5 = 0;
	return;
}

//====================================================
//	Watch dog interrutp service routine
void	INT6_ISR(void)	interrupt 6
{
	IE_INT6 = 0;
	return;
}



