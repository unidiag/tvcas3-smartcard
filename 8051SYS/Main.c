#include	"_17BD_UserCode.h"
#include	"..\ISO7816\ISO7816.h"
#include	<intrins.h>


BYTEX	SN[4]={0x7D,0x2B,0x75,0x00};	// serial number
BYTEX	UA[4]={0x00,0x1B,0x49,0x3B};	// shared address. first byte 0B XX
BYTEX	OP[15]={0x54,0x56,0x43,0x41,0x53,0x2E,0x43,0x4F,0x4D,0x20,0x20,0x20,0x20,0x20,0x20};	// TVCAS.COM______
BYTEX	CAM[3]={0x00,0x00,0x00};



//====================================================
//	Main process
//====================================================
void	main(void)
{	
	short	i = 0;
	unsigned char ii = 0;
	unsigned char last = 0;
	
  union {
		unsigned char c[4];
		unsigned long l;
  }now;  
    
  union {
   unsigned char c[4];
   unsigned long l;
  }start;  
    
  union {
   unsigned char c[4];
   unsigned long l;
  }finish;  	
	
  union {
   unsigned char c[4];
   unsigned long l;
  }antishara;  
 
	
	// HW init
	IE_EA = 1;						//	Interrupt enabed by corresponding control bit
	IP = 0x00;						//	No priviledge
	UCR2 = 0x03;					//	Retry 3 times
	RNGCTL = 0x01;					//	Close RNG

  // ATR
  UBRC = 0x11;		//	9600 Bps
  send_byte(0x3B);
  send_byte(0x24);
  send_byte(0x00);
  send_byte(0x30);
  send_byte(0x42);
  send_byte(0x30);
  send_byte(0x30);
	
	
	first_start(); // write memory datas
	
	while(1){  
		CLA = receive_byte();
		INS = receive_byte();
		if(CLA!=0x66) P1 = receive_byte();
		
		
		switch(CLA){

/*
PPPPPP  IIIII NN   NN     CCCCC   OOOOO  DDDDD   EEEEEEE 
PP   PP  III  NNN  NN    CC    C OO   OO DD  DD  EE      
PPPPPP   III  NN N NN    CC      OO   OO DD   DD EEEEE   
PP       III  NN  NNN    CC    C OO   OO DD   DD EE      
PP      IIIII NN   NN     CCCCC   OOOO0  DDDDDD  EEEEEEE 
                                                         
*/



			case 0x1F: // maturity rating PIN: 1F 05 31 31 31 31 01   = 1111
				ISO_AutoTxNULL();
				if(P1==ROM[81]
						&& receive_byte()==ROM[82]
						&& receive_byte()==ROM[83]
						&& receive_byte()==ROM[84]){
							ROM[80] = receive_byte(); // 08
							UpdateFlash(0x7000,ROM,255);
							send_sw(0x00);		//  9000 - OK
						}else{
							receive_byte(); // 08
							send_byte(0x90);  //  9017 - FAIL
							send_byte(0x17);
						}
				break;
			case 0x1E: // change PIN: 1E 08 31 32 33 34 30 30 30 30
				ISO_AutoTxNULL();
				if(P1==ROM[81]
						&& receive_byte()==ROM[82]
						&& receive_byte()==ROM[83]
						&& receive_byte()==ROM[84]){
							ROM[81] = receive_byte(); // 0
							ROM[82] = receive_byte(); // 0
							ROM[83] = receive_byte(); // 0
							ROM[84] = receive_byte(); // 0
							UpdateFlash(0x7000,ROM,255);
							send_sw(0);		//  9000 - OK
						}else{
							receive_byte(); // 0
							receive_byte(); // 0
							receive_byte(); // 0
							receive_byte(); // 0
							send_byte(0x90);  //  9017 - FAIL
							send_byte(0x17);
						}
				break;

				
			
			
			
			
			
/*
DDDDD   DDDDD   
DD  DD  DD  DD  
DD   DD DD   DD 
DD x  DD DD   DD 
DDDDDD  DDDDDD  
                
*/			
			case 0xDD:
				P2 = receive_byte();
				P3 = receive_byte(); // len
				ISO_AutoTxNULL();
				send_byte(INS);
				
				switch(INS){		
					case 0x99:
						send_sw(0);
						break;
					case 0xCA:
						for(i=0; i<P3; i++){
							send_byte(IOBuf[i]);
            }
						send_sw(0);		// 0x9000
						break;
					case 0x11:
						// 3DES ENCRYPT
						// KEY1
						IOBuf[0] = 0x7E; // ECM80  7E 33 A2 78 B3 61 7A 30 9B 18 22 2F 37 2A 23 19
						IOBuf[1] = 0x33;
						IOBuf[2] = 0xA2;
						IOBuf[3] = 0x78;
						IOBuf[4] = 0xB3;
						IOBuf[5] = 0x61;
						IOBuf[6] = 0x7A;
						IOBuf[7] = 0x30;
						// KEY2
						IOBuf[8] =  0x9B;
						IOBuf[9] =  0x18;
						IOBuf[10] = 0x22;
						IOBuf[11] = 0x2F;
						IOBuf[12] = 0x37;
						IOBuf[13] = 0x2A;
						IOBuf[14] = 0x23;
						IOBuf[15] = 0x19;
						// DATA
						IOBuf[16] = 0x45; // 45 90 4C 4A D8 46 54 99
						IOBuf[17] = 0x90; // 5f 22 34 02 ad f6 14 b7	
						IOBuf[18] = 0x4C;
						IOBuf[19] = 0x4A;
						IOBuf[20] = 0xD8;
						IOBuf[21] = 0x46;
						IOBuf[22] = 0x54;
						IOBuf[23] = 0x99;
						
						DES_Operation(1, 0, IOBuf, 24);
						DES_Operation(1, 0x10, IOBuf, 24);
						send_sw(0);		//  9000
						break;
						
				}

				
				break;
				
				
			case 0x10:
				ISO_AutoTxNULL();
        IOBuf[0] = 0x20;
        IOBuf[1] = 0x01;
        IOBuf[2] = 0x40;
        IOBuf[3] = 0x28;
        IOBuf[4] = 0x02;
        IOBuf[5] = 0x0B; // 0B XX
        IOBuf[6] = UA[0];
        IOBuf[7] = 0x2F;
        IOBuf[8] = 0x02;  // TVCAS  |  DOMRU
        IOBuf[9] = 0x00;
        IOBuf[10] = 0x2C; //  2C        07
        IOBuf[11] = 0x30;
        IOBuf[12] = 0x01;
        IOBuf[13] = ROM[80]; //  01 - with parents, 02 - normal, 04 - 18+, 08 - erotic
        IOBuf[14] = 0x23;
        IOBuf[15] = 0x01;
        IOBuf[16] = 0x02; //  02        0A
				send_sw(0x11); // 0x9811
				break;
			
			
			case 0x11:
				for(i=0; i<11;i++){ // 0x03...0x0E
					receive_byte();
				}
				// FTA channel in STB FORBIS (aldo aldo)
				if(P3==0x0E){
					send_sw(0); // 0x9000
			  	break;
				}
				CAM[0] = receive_byte();
				CAM[1] = receive_byte();
				CAM[2] = receive_byte();
				for(i=17; i<P3; i++){
					receive_byte();
				}

				ISO_AutoTxNULL();
				IOBuf[0] = 0x22;
				IOBuf[1] = 0x18;
				IOBuf[2] = 0x09;
				IOBuf[3] = 0x04;
				IOBuf[4] = 0x0B; // 0B XX
				IOBuf[5] = UA[0];
				IOBuf[6] = CAM[0];  // oscam = E0, cam = 0F
				IOBuf[7] = CAM[1];  // oscam = 30, cam = DD
				IOBuf[8] = 0x23;
				IOBuf[9] = 0x07;
				IOBuf[10] = 0x00;
				IOBuf[11] = 0x00;
				IOBuf[12] = 0x00;
				IOBuf[13] = SN[0]; // 		  7D -- # 2 100 000 000
				IOBuf[14] = SN[1]; //       2B
				IOBuf[15] = SN[2]; //       75
				IOBuf[16] = SN[3]; //       00
				IOBuf[17] = 0x23;
				IOBuf[18] = 0x07;
				IOBuf[19] = 0x00;
				IOBuf[20] = 0x00;
				IOBuf[21] = 0x00;
				IOBuf[22] = 0x00; // shared address
				IOBuf[23] = UA[1]; // 1E
				IOBuf[24] = UA[2]; // C9
				IOBuf[25] = UA[3]; // 2A
				send_sw(0x1A); // 0x981A
				break;
				
				
				
			case 0x1C:
				ISO_AutoTxNULL();
				if(P1==0x01){
					send_sw(0); // 9000
					break;
				}
				ReadFlash(ROM,0x7000,0); // read 0x7000 + 256 bytes to IOBuf[x]
				IOBuf[0] = 0x32;
				IOBuf[1] = 0x2F;
				IOBuf[2] = 0x10;
				IOBuf[3] = 0x10;
				IOBuf[4] = 0x01;
				IOBuf[5] = 0x0F;
				if(ROM[27]==1){ // blocked
					IOBuf[6] = 67; //        C
					IOBuf[7] = 65; //        A  
					IOBuf[8] = 82; //        R 
					IOBuf[9] = 68; //        D 
					IOBuf[10] = 32; //         
					IOBuf[11] = 66; //       B 
					IOBuf[12] = 76; //       L
					IOBuf[13] = 79; //       O
					IOBuf[14] = 67; //       C
					IOBuf[15] = 75; //       K
					IOBuf[16] = 69; //       E
					IOBuf[17] = 68; //       D
					IOBuf[18] = 32; //       _
					IOBuf[19] = 32; //       _
					IOBuf[20] = 32; //       _
				}else{
					IOBuf[6] = OP[0]; //        T 
					IOBuf[7] = OP[1]; //        V  
					IOBuf[8] = OP[2]; //        C 
					IOBuf[9] = OP[3]; //        A 
					IOBuf[10] = OP[4]; //       S  
					IOBuf[11] = OP[5]; //       . 
					IOBuf[12] = OP[6]; //       C
					IOBuf[13] = OP[7]; //       O
					IOBuf[14] = OP[8]; //       M
					IOBuf[15] = OP[9]; //       _
					IOBuf[16] = OP[10]; //       _
					IOBuf[17] = OP[11]; //       _
					IOBuf[18] = OP[12]; //       _
					IOBuf[19] = OP[13]; //       _
					if(ROM[16]==1){
						IOBuf[20] = 0x2B; // [+] PARING
					}else{
						IOBuf[20] = OP[14]; //       _
					}
				}
				IOBuf[21] = 0x30;
				IOBuf[22] = 0x02;
				IOBuf[23] = ROM[12]; // 05
				IOBuf[24] = ROM[13]; // 03.2019
				IOBuf[25] = 0x30;
				IOBuf[26] = 0x02;
				IOBuf[27] = ROM[14]; // 23
				IOBuf[28] = ROM[15]; // 03.2022
				IOBuf[29] = 0x20;
				IOBuf[30] = 0x04;
				IOBuf[31] = ROM[8];    // AC
				IOBuf[32] = ROM[9];    // AC
				IOBuf[33] = ROM[10];    // AC
				IOBuf[34] = ROM[11];    // AC
				IOBuf[35] = 0x30;
				IOBuf[36] = 0x02;
				IOBuf[37] = ROM[12]; // 05
				IOBuf[38] = ROM[13]; // 03.2019
				IOBuf[39] = 0x30;
				IOBuf[40] = 0x02;
				IOBuf[41] = ROM[14]; // 17
				IOBuf[42] = ROM[15]; // 03.2020
				IOBuf[43] = 0x20;
				IOBuf[44] = 0x04;
				IOBuf[45] = ROM[19];      // emm counter
				IOBuf[46] = ROM[23];      // correctly decrypt
				//IOBuf[46] = ROM[27];      // LOCK / UNLOCK / SUPERUNLOCK
				IOBuf[47] = ROM[24];      // correctly decrypt
				IOBuf[48] = ROM[20];      // ecm counter
				send_sw(0x31); // 9831
				break;
				
				
/*

EEEEEEE  CCCCC  MM    MM 
EE      CC    C MMM  MMM 
EEEEE   CC      MM MM MM 
EE      CC    C MM    MM 
EEEEEEE  CCCCC  MM    MM 
                         

*/ 			
			case 0x14:
				P2 = receive_byte(); // 80 / 81 ecm
				receive_byte(); // 70
				receive_byte(); // 34
				receive_byte(); // 70
				receive_byte(); // 32
				receive_byte(); // 64
				receive_byte(); // 21
				ReadFlash(ROM,0x7000,0);
				RX_data(); // read 48 bytes
				ISO_AutoTxNULL();
				
				ROM[20]++; // ecm counter +1
			
        now.c[0] = RXBuf[0];
        now.c[1] = RXBuf[1];
        now.c[2] = RXBuf[2];
        now.c[3] = RXBuf[3];
			
			
			
				// counter
				if(ROM[27] == 0xCA){ // disabled counter
					_nop_();
					_nop_();
					_nop_();
				}else if(ROM[27] == 0x00){
					ii = now.c[3];
					if(ii>30 && ii <= ROM[28]){
						ROM[26]++;
					}else if(ROM[26] > 0){
						ROM[26]--;
					}
					ROM[28] = ii+5;			
					
					if(ROM[26]>0xF0){ // need 0xFE
						ROM[27] = 1; // ENABLE ANTISHARA
						ROM[28] = now.c[0];
						ROM[29] = now.c[1];
						ROM[30] = now.c[2];
						ROM[31] = now.c[3];
					}
				}else{
					antishara.c[0] = ROM[28];
					antishara.c[1] = ROM[29];
					antishara.c[2] = ROM[30];
					antishara.c[3] = ROM[31];
					if((now.l - antishara.l) > 86400*7){ // need 86400 (1 day)
						ROM[26] = 0;
						ROM[27] = 0;
					}
				}
			
				UpdateFlash(0x7000,ROM,255);
			

        start.c[0] = ROM[0];		// 5F 1E 5A C6 (27.07.20)
        start.c[1] = ROM[1];
        start.c[2] = ROM[2];
        start.c[3] = ROM[3];

        finish.c[0] = ROM[4];			// 5F 22 4F 46
        finish.c[1] = ROM[5];
        finish.c[2] = ROM[6];
        finish.c[3] = ROM[7];			
				
				IOBuf[501] = 0; // control summ
				for(IOBuf[500]=0; IOBuf[500]<47; IOBuf[500]++){
					IOBuf[501] = IOBuf[501]+RXBuf[IOBuf[500]];
				}
				


				if((ROM[27]==0 || ROM[27]==0xCA) && // antishara
						IOBuf[501]==RXBuf[47] &&
						(start.l < now.l) && (finish.l > now.l) &&
						((ROM[8] & RXBuf[20])>0 ||
						 (ROM[9] & RXBuf[21])>0 ||
						 (ROM[10] & RXBuf[22])>0 ||
						 (ROM[11] & RXBuf[23])>0) &&
						(ROM[16]==0x00 || (ROM[17]==CAM[0] && ROM[18]==CAM[1] && ROM[25]==CAM[2]))){

					IOBuf[0] = 0x25;     
					IOBuf[1] = 0x0D;     
					IOBuf[2] = 0x00;     
					IOBuf[3] = 0x00;     
					IOBuf[4] = 0x01;     
					IOBuf[5] = 0x00;     
					IOBuf[6] = 0x00;     
					IOBuf[7] = RXBuf[4]; // cw1
					IOBuf[8] = RXBuf[5];  
					IOBuf[9] = RXBuf[6];    
					IOBuf[10] = RXBuf[7];
					IOBuf[11] = RXBuf[8];   
					IOBuf[12] = RXBuf[9];   
					IOBuf[13] = RXBuf[10];    
					IOBuf[14] = RXBuf[11];    
					IOBuf[15] = 0x25;    
					IOBuf[16] = 0x0D;     
					IOBuf[17] = 0x00;     
					IOBuf[18] = 0x00;     
					IOBuf[19] = 0x00;     
					IOBuf[20] = 0x00;     
					IOBuf[21] = 0x00;
					IOBuf[22] = RXBuf[12];  // cw2
					IOBuf[23] = RXBuf[13];  
					IOBuf[24] = RXBuf[14];  
					IOBuf[25] = RXBuf[15];
					IOBuf[26] = RXBuf[16];   
					IOBuf[27] = RXBuf[17];   
					IOBuf[28] = RXBuf[18];   
					IOBuf[29] = RXBuf[19]; 
					IOBuf[30] = 0x31;     
					IOBuf[31] = 0x02;     
					IOBuf[32] = 0x40;     
					IOBuf[33] = 0x00;  
					send_sw(0x22); // 9822
					
				}else{
					IOBuf[0] = 0x31;
					IOBuf[1] = 0x02;
					IOBuf[2] = 0x00;
					IOBuf[3] = 0x00;
					send_sw(0x04); // 9804
				}
				break;
			
			
/*

EEEEEEE MM    MM MM    MM 
EE      MMM  MMM MMM  MMM 
EEEEE   MM MM MM MM MM MM 
EE      MM    MM MM    MM 
EEEEEEE MM    MM MM    MM 
       
EMM				

*/				
			case 0x12:
				P2 = P1;
				if(P2==0x82){
					receive_byte(); // 0x70
					receive_byte(); // 0x3B
					receive_byte(); // 0x00
					receive_byte(); // 0x00
					receive_byte(); // 0x00
					
					UA[0] = receive_byte(); // serial number
					UA[1] = receive_byte();
					UA[2] = receive_byte();
					UA[3] = receive_byte();
					receive_byte();receive_byte();receive_byte();receive_byte(); // 4 bytes
					ReadFlash(ROM, 0x7000,255);
					RX_data(); // read 48 bytes
					
					// if message destination to this card
					if(UA[0]==SN[0] && UA[1]==SN[1] && UA[2]==SN[2] && UA[3]==SN[3]){
						
						ReadFlash(ROM, 0x7000,255);
						ROM[19]++; // emm country
						IOBuf[501] = 0; // control summ
						for(IOBuf[500]=0; IOBuf[500]<47; IOBuf[500]++){
							IOBuf[501] = IOBuf[501]+RXBuf[IOBuf[500]];
						}
						if(SN[0]==RXBuf[0] && SN[1]==RXBuf[1] && SN[2]==RXBuf[2] && SN[3]==RXBuf[3]){

								ROM[8] = RXBuf[4];		// access criteria
								ROM[9] = RXBuf[5];
								ROM[10] = RXBuf[6];
								ROM[11] = RXBuf[7];
								ROM[0] = RXBuf[8];    // start
								ROM[1] = RXBuf[9];    // start
								ROM[2] = RXBuf[10];   // start
								ROM[3] = RXBuf[11];   // start
								ROM[4] = RXBuf[12];   // stop
								ROM[5] = RXBuf[13];   // stop
								ROM[6] = RXBuf[14];   // stop
								ROM[7] = RXBuf[15];   // stop
								ROM[12] = RXBuf[16];		// slydate start
								ROM[13] = RXBuf[17];
								ROM[14] = RXBuf[18];		// slydate stop
								ROM[15] = RXBuf[19];
								ROM[16] = RXBuf[20];		// pair on/off
								if(RXBuf[21]==1 && ROM[27]==0xCA) ROM[27]=0x00;
								ROM[21] = RXBuf[21];		// protection
								ROM[22] = RXBuf[22];		// reserve		
							
								if(ROM[16]==0 ||
										(ROM[17]==0x00 && ROM[18]==0x00 && ROM[25]==0x00)){
											ROM[17] = CAM[0];
											ROM[18] = CAM[1];
											ROM[25] = CAM[2];
								}								
								
					 }		

					 ROM[23] = IOBuf[501];		// chk csum
					 ROM[24] = RXBuf[47];			// chk csum	
					 UpdateFlash(0x7000,ROM,255);
				 }
				}
				ISO_AutoTxNULL();
				
				send_sw(0);
				break;
			
			
				
			case 0x66:
				ISO_AutoTxNULL();
        IOBuf[0] = 0x74;     
        IOBuf[1] = 0x04;     
        IOBuf[2] = SN[0];     
        IOBuf[3] = SN[1];      
        IOBuf[4] = SN[2];       
        IOBuf[5] = SN[3]; 
				send_sw(0x06);
				break;
				
				
				
				
				
			default:
				ISO_AutoTxNULL();
				send_sw(0); // 9000
		}

	}
	
	
	
	
}







#define	MEMXDATA	0x01
#define	MEMCODE		0xFF
//====================================================
//	Copy memory
//	DPTR0	=	dst
//	DPTR1	=	src
//	R0		=	backupDPS
//	R1 R2	=	length	
void	mem_cpy(BYTE * dst,BYTE * src,USHORT len)
{
	if(*(BYTEX *)(&dst) == MEMXDATA)
	{
		switch(*(BYTEX *)(&src))
		{
			case	MEMXDATA:								//	XDATA to XDATA
			{	
				#pragma	asm
						MOV		R0,		DPS	
						MOV		DPS,	0	
						MOV		DPTR,	#?_MEM_CPY?BYTE+6	//	Get length
						MOVX	A,		@DPTR
						MOV		R1,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		R2,		A
						JZ		$+1+1+1
						INC		R1
						MOV		DPTR,	#?_MEM_CPY?BYTE+1	//	DPTR 0 for dst
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL0,	A
						MOV		DPH0,	R3
						INC		DPS
						MOV		DPTR,	#?_MEM_CPY?BYTE+4	//	DPTR 1 for src
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL1,	A
						MOV		DPH1,	R3
						MOV		DPS,	#01H
					LOOP_CPY_X:
						MOVX	A,		@DPTR				//	DPTR 0 for dst
						INC		DPTR
						INC		DPS
						MOVX	@DPTR,	A					//	DPTR 1 for src
						INC		DPTR
						INC		DPS
						DJNZ	R2,		LOOP_CPY_X
						DJNZ	R1,		LOOP_CPY_X
						MOV		DPS,	R0			
				#pragma	endasm	  
				break;
			}	
			case	MEMCODE:								//	CODE to XDATA
			{
			 	#pragma	asm
						MOV		R0,		DPS	
						MOV		DPS,	0	
						MOV		DPTR,	#?_MEM_CPY?BYTE+6	//	Get length
						MOVX	A,		@DPTR
						MOV		R1,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		R2,		A
						JZ		$+1+1+1
						INC		R1
						MOV		DPTR,	#?_MEM_CPY?BYTE+1	//	DPTR 0 for dst
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL0,	A
						MOV		DPH0,	R3
						INC		DPS
						MOV		DPTR,	#?_MEM_CPY?BYTE+4	//	DPTR 1 for src
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL1,	A
						MOV		DPH1,	R3
						MOV		DPS,	#01H
					LOOP_CPY_C:
						CLR		A
						MOVC	A,		@A+DPTR				//	DPTR 0 for dst
						INC		DPTR
						INC		DPS
						MOVX	@DPTR,	A					//	DPTR 1 for src
						INC		DPTR
						INC		DPS
						DJNZ	R2,		LOOP_CPY_C
						DJNZ	R1,		LOOP_CPY_C	
						MOV		DPS,	R0		
				#pragma	endasm	 
				break;
			}	
		}
	}
	else
		while(1);											//	ERROR
}

//====================================================
//	Compare memory
//	DPTR0	=	dst
//	DPTR1	=	src
//	R0		=	backupDPS
//	R1 R2	=	length
//	R3		=	compare data
BYTE	mem_cmp(BYTE * dst,BYTE * src,USHORT len)
{
	if(*(BYTEX *)(&dst) == MEMXDATA)
	{
		switch(*(BYTEX *)(&src))
		{
			case	MEMXDATA:								//	XDATA vs XDATA		
			{	
				#pragma	asm	
						MOV		R0,		DPS	
						MOV		DPS,	0	
						MOV		DPTR,	#?_MEM_CMP?BYTE+6	//	Get length
						MOVX	A,		@DPTR
						MOV		R1,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		R2,		A
						JZ		$+1+1+1
						INC		R1
						MOV		DPTR,	#?_MEM_CMP?BYTE+1	//	DPTR 0 for dst
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL0,	A
						MOV		DPH0,	R3
						INC		DPS
						MOV		DPTR,	#?_MEM_CMP?BYTE+4	//	DPTR 1 for src
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL1,	A
						MOV		DPH1,	R3
						INC		DPS
					LOOP_CMP_XX:
						MOVX	A,		@DPTR				//	DPTR 0
						MOV		R3,		A				 
						INC		DPTR
						INC		DPS
						MOVX	A,		@DPTR				//	DPTR 1
						INC		DPTR
						INC		DPS
						CLR		C
						SUBB	A,		R3
						JZ		CONTINUE_XX
						MOV		R7,		0
						MOV		DPS,	R0	
						RET
					CONTINUE_XX:
						DJNZ	R2,		LOOP_CMP_XX
						DJNZ	R1,		LOOP_CMP_XX
						MOV		DPS,	R0			
				#pragma	endasm					
				break;
			}
			case	MEMCODE:								//	XDATA vs CODE		
			{	
				#pragma	asm	
						MOV		R0,		DPS	
						MOV		DPS,	0	
						MOV		DPTR,	#?_MEM_CMP?BYTE+6	//	Get length
						MOVX	A,		@DPTR
						MOV		R1,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		R2,		A
						JZ		$+1+1+1
						INC		R1
						MOV		DPTR,	#?_MEM_CMP?BYTE+1	//	DPTR 0 for dst
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL0,	A
						MOV		DPH0,	R3
						INC		DPS
						MOV		DPTR,	#?_MEM_CMP?BYTE+4	//	DPTR 1 for src
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL1,	A
						MOV		DPH1,	R3
						INC		DPS
					LOOP_CMP_CX:
						MOVX	A,		@DPTR				//	DPTR 0
						MOV		R3,		A				 
						INC		DPTR
						INC		DPS
						CLR		A
						MOVC	A,		@A+DPTR				//	DPTR 1
						INC		DPTR
						INC		DPS
						CLR		C
						SUBB	A,		R3
						JZ		CONTINUE_CX
						MOV		R7,		0
						MOV		DPS,	R0	
						RET
					CONTINUE_CX:
						DJNZ	R2,		LOOP_CMP_CX
						DJNZ	R1,		LOOP_CMP_CX
						MOV		DPS,	R0			
				#pragma	endasm					
				break;
			}
		}
	}
	else if(*(BYTEX *)(&dst) == MEMCODE)
	{
		switch(*(BYTEX *)(&src))
		{
			case	MEMXDATA:								//	CODE vs XDATA				
			{	
				#pragma	asm	
						MOV		R0,		DPS	
						MOV		DPS,	0	
						MOV		DPTR,	#?_MEM_CMP?BYTE+6	//	Get length
						MOVX	A,		@DPTR
						MOV		R1,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		R2,		A
						JZ		$+1+1+1
						INC		R1
						MOV		DPTR,	#?_MEM_CMP?BYTE+1	//	DPTR 0 for dst
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL0,	A
						MOV		DPH0,	R3
						INC		DPS
						MOV		DPTR,	#?_MEM_CMP?BYTE+4	//	DPTR 1 for src
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL1,	A
						MOV		DPH1,	R3
						INC		DPS
					LOOP_CMP_XC:
						CLR		A
						MOVC	A,		@A+DPTR				//	DPTR 0
						MOV		R3,		A				 
						INC		DPTR
						INC		DPS
						MOVX	A,		@DPTR				//	DPTR 1
						INC		DPTR
						INC		DPS
						CLR		C
						SUBB	A,		R3
						JZ		CONTINUE_XC
						MOV		R7,		0
						MOV		DPS,	R0	
						RET
					CONTINUE_XC:
						DJNZ	R2,		LOOP_CMP_XC
						DJNZ	R1,		LOOP_CMP_XC
						MOV		DPS,	R0			
				#pragma	endasm					
				break;
			}
			case	MEMCODE:								//	CODE vs	CODE		
			{	
				#pragma	asm	
						MOV		R0,		DPS	
						MOV		DPS,	0	
						MOV		DPTR,	#?_MEM_CMP?BYTE+6	//	Get length
						MOVX	A,		@DPTR
						MOV		R1,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		R2,		A
						JZ		$+1+1+1
						INC		R1
						MOV		DPTR,	#?_MEM_CMP?BYTE+1	//	DPTR 0 for dst
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL0,	A
						MOV		DPH0,	R3
						INC		DPS
						MOV		DPTR,	#?_MEM_CMP?BYTE+4	//	DPTR 1 for src
						MOVX	A,		@DPTR
						MOV		R3,		A
						INC		DPTR
						MOVX	A,		@DPTR
						MOV		DPL1,	A
						MOV		DPH1,	R3
						INC		DPS
					LOOP_CMP_CC:
						CLR		A
						MOVC	A,		@A+DPTR				//	DPTR 0
						MOV		R3,		A				 
						INC		DPTR
						INC		DPS
						CLR		A
						MOVC	A,		@A+DPTR				//	DPTR 1
						INC		DPTR
						INC		DPS
						CLR		C
						SUBB	A,		R3
						JZ		CONTINUE_CC
						MOV		R7,		0
						MOV		DPS,	R0	
						RET
					CONTINUE_CC:
						DJNZ	R2,		LOOP_CMP_CC
						DJNZ	R1,		LOOP_CMP_CC
						MOV		DPS,	R0			
				#pragma	endasm					
				break;
			}
		}
	}
	else
		while(1);

	return 1;
}

