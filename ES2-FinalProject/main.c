//include gyro files
#include "PmodGYRO.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xil_printf.h"
//include oled files
#include "PmodOLEDrgb.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "time.h"
#include <stdio.h>
#include <xgpio.h>
#include "xparameters.h"
#include "sleep.h"
#include "platform.h"
void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();

PmodOLEDrgb oledrgb;
PmodGYRO myDevice;

u8 rgbUserFont[] = {
   0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00, 0x00, // 0x00
   0x0E, 0x1F, 0x15, 0x1F, 0x17, 0x10, 0x1F, 0x0E, // 0x01
   0x00, 0x1F, 0x11, 0x00, 0x00, 0x11, 0x1F, 0x00, // 0x02
   0x00, 0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00, // 0x03
   0x07, 0x0C, 0xFA, 0x2F, 0x2F, 0xFA, 0x0C, 0x07  // 0x04
}; // This table defines 5 user characters, although only one is used
int main(void) {
   DemoInitialize();
   DemoRun();
   DemoCleanup();
	XGpio input;
	int button_data = 0;
	XGpio_Initialize(&input, XPAR_AXI_GPIO_0_DEVICE_ID); //initializeinput XGpio variable
	XGpio_SetDataDirection(&input, 1, 0xF); //set firstchannel tristate buffer to input
	init_platform();
	while(button_data ==0 ){
	button_data = XGpio_DiscreteRead(&input, 1); //get button data
	//print message dependent on whether one or more buttons are pressed
	if(button_data != 0b0000){
		cleanup_platform();
		main();
	} //do nothing

	usleep(200000); //delay
	}
	cleanup_platform();
	return 0;
}
void move(int dir, int *col, int *row) {
   usleep(500);
 //add collision test here
 switch(dir){
 case 1: //n
	 OLEDrgb_DrawLine(&oledrgb, (u16) *col , (u16) *row+1, (u16) (*col +1), (u16) (*row +1), 0);

     OLEDrgb_DrawLine(&oledrgb, (u16) *col ,  (u16) *row-1,  (u16) *col +1, (u16) *row -1, 0x07e0);
     (*row)--;
     break;
 case 2: //e
	 OLEDrgb_DrawLine(&oledrgb, *col , *row,*col , *row +1, 0);

     OLEDrgb_DrawLine(&oledrgb, *col +2, *row, *col +2, *row +1, 0x07e0);
     (*col)++;

     break;
 case 3: //s
	 OLEDrgb_DrawLine(&oledrgb, *col , *row,*col +1, *row , 0);

     OLEDrgb_DrawLine(&oledrgb, *col , *row+2, *col +1, *row +2, 0x07e0);
     (*row)++;

     break;
 case 4: //w
	 OLEDrgb_DrawLine(&oledrgb, *col +1, *row,*col +1, *row +1, 0);

	 OLEDrgb_DrawLine(&oledrgb, *col -1, *row,*col -1, *row +1, 0x07e0);
     (*col)--;

     break;
 }

}

int within_neg( int16_t numb)
{
 if((u16) numb <= 0xDFFF && (u16) numb >= 0x8000) //must be below threshhold and negative
   return 1;
   else
   return 0;
}

int within_pos( int16_t numb)
{
 if((int) numb >= 0x2000 && (int) numb < 0x8000) //must not be negative and over threshhold
   return 1;
   else
   return 0;
}

int is_stable( int16_t numb)
{
 if((u16) numb < 0x2000 || (u16) numb > 0xE000) //stable is within 0x1000
   return 1;
   else
   return 0;
}
int collision(int col, int row){
   if(col ==0 || col == 95 ||row ==0 || row == 63 ) //touch outer boundaries
      return 1;

  else if(row == 16 && col >= 0 && col < 81) //top line
    return 1;
  else if(row == 32 && col >= 16 && col < 96) // middle line
    return 1;
  else if(row == 48 && col >= 0 && col < 81) // bottom line
    return 1;
  else return 0;
}
void DemoInitialize() {
   EnableCaches();
   OLEDrgb_begin(&oledrgb, XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR,
         XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR);
    GYRO_begin(&myDevice, XPAR_PMODGYRO_0_AXI_LITE_SPI_BASEADDR,
         XPAR_PMODGYRO_0_AXI_LITE_GPIO_BASEADDR);

   // Set Threshold Registers
   GYRO_setThsXH(&myDevice, 0x0F);
   GYRO_setThsYH(&myDevice, 0x0F);
   GYRO_setThsZH(&myDevice, 0x0F);

   GYRO_enableInt1(&myDevice, GYRO_INT1_XHIE);    // Threshold interrupt
   GYRO_enableInt2(&myDevice, GYRO_REG3_I2_DRDY); // Data Rdy/FIFO interrupt
}

void DemoRun() {
   char ch;

   // Define the user definable characters
   for (ch = 0; ch < 5; ch++) {
      OLEDrgb_DefUserChar(&oledrgb, ch, &rgbUserFont[ch * 8]);
   }

   OLEDrgb_SetCursor(&oledrgb, 2, 1);
   OLEDrgb_PutString(&oledrgb, "Digilent"); // Default color (green)
   OLEDrgb_SetCursor(&oledrgb, 4, 4);
   OLEDrgb_SetFontColor(&oledrgb, OLEDrgb_BuildRGB(0, 0, 255)); // Blue font
   OLEDrgb_PutString(&oledrgb, "OledRGB");

   OLEDrgb_SetFontColor(&oledrgb, OLEDrgb_BuildRGB(200, 200, 44));
   OLEDrgb_SetCursor(&oledrgb, 1, 6);
   OLEDrgb_PutChar(&oledrgb, 4);

   OLEDrgb_SetFontColor(&oledrgb, OLEDrgb_BuildRGB(200, 12, 44));
   OLEDrgb_SetCursor(&oledrgb, 5, 6);
   OLEDrgb_PutString(&oledrgb, "Demo");
   OLEDrgb_PutChar(&oledrgb, 0);

      sleep(2); // Wait 5 seconds
   OLEDrgb_Clear(&oledrgb);
   sleep(2);
   int playerc=1, playerr = 8;
   OLEDrgb_DrawLine(&oledrgb, playerc , playerr, playerc, playerr+1, 0x07e0);
   OLEDrgb_DrawLine(&oledrgb, playerc +1, playerr, playerc+1, playerr+1, 0x07e0);

  OLEDrgb_DrawLine(&oledrgb, 0, 16, 80, 16, 0xFFFF);
  OLEDrgb_DrawLine(&oledrgb, 16, 32, 95, 32,0xFFFF);
  OLEDrgb_DrawLine(&oledrgb, 0, 48, 80, 48, 0xFFFF);
  for(int i =49; i < 64; i++){

  	   OLEDrgb_DrawLine(&oledrgb, 0, i, 15, i, 0x001F);

     }
   int16_t xAxis = 0;
   int16_t yAxis = 0;
   int16_t zAxis = 0;
   int8_t temp = 0;
   int trig = 0;

   print("Starting...\n\r");
   int n, e, s, w, lose =0;
   while (!(playerc >=0 && playerc <16 && playerr>49 && playerr <64)) {
      usleep(25000);

      if (GYRO_Int1Status(&myDevice) != 0) {
         xil_printf("\x1B[2J");
         xil_printf("\x1B[H");
         xil_printf("Threshold reached\n\r");
         trig = 1;
      }
      if (GYRO_Int2Status(&myDevice) != 0) {
         if (trig == 1) {
            trig = 0;
         } else {
            xil_printf("\x1B[2J"); // Clear screen
            xil_printf("\x1B[H");  // Reset cursor to 0,0
         }
         xil_printf("Data is ready\n\r\n\r");

         xAxis = GYRO_getX(&myDevice);
         yAxis = GYRO_getY(&myDevice);
         zAxis = GYRO_getZ(&myDevice);
         temp = GYRO_getTemp(&myDevice);

         xil_printf("X Axis: 0x%04x\n\r", xAxis & 0xFFFF);
         xil_printf("Y Axis: 0x%04x\n\r", yAxis & 0xFFFF);
         xil_printf("Z Axis: 0x%04x\n\r", zAxis & 0xFFFF);

         xil_printf("\n\r");

         xil_printf("Temperature: %d deg F\n\r", temp);
         //moving oepration
         if(!(is_stable(xAxis)&& is_stable(yAxis))){
         if(within_neg(xAxis)) //east
              {
                if(w !=0){n=0; s =0; e =0; w = 0; usleep(500000);}
                else e = 1;
              }
             else if(within_pos(xAxis)) //west
             {
               if(e !=0) {n=0; s =0; e =0; w = 0;usleep(500000);}
                else w = 1;
             }
         if(within_neg(yAxis)) //north
              {
                if(s !=0){n=0; s =0; e =0; w = 0;usleep(500000);}
                else n = 1;
              }
             else if(within_pos(yAxis)) //south
             {
               if(n !=0){n=0; s =0; e =0; w = 0;usleep(500000);}
                else s = 1;
             }
         }

             if(n==1 ) {move(1, &playerc, &playerr);}
             else if(s==1 ){ move(3, &playerc, &playerr);}
             else if(w==1) { move(4, &playerc, &playerr);}
              else if(e==1){ move(2, &playerc, &playerr);}
              xil_printf("North: %d\n", n);
              xil_printf("East: %d\n", e);
              xil_printf("South: %d\n", s);
              xil_printf("West: %d\n", w);
              xil_printf("Col: %d\n", playerc);
              xil_printf("Row: %d\n", playerr);
              xil_printf("\n");
              if(collision(playerc, playerr))
              {
            	  lose =1;
            	  break;
              }
      }
   }
   OLEDrgb_Clear(&oledrgb);
   OLEDrgb_SetCursor(&oledrgb, 3, 1);
   if (!lose)
      OLEDrgb_PutString(&oledrgb, "YOU WIN!"); // Default color (green)
   else
	   OLEDrgb_PutString(&oledrgb, "YOU LOSE!");

}

void DemoCleanup() {
   GYRO_end(&myDevice);
   DisableCaches();
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#endif
}
void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#endif
}

