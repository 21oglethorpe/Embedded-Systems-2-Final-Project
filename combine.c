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
   return 0;
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
   for(int i =0; i < 64; i++){
	   OLEDrgb_DrawLine(&oledrgb, 0, i, 95, i, 0xFFFF);
   }
  OLEDrgb_DrawLine(&oledrgb, 0, 16, 80, 16, 0x07e0);
  OLEDrgb_DrawLine(&oledrgb, 16, 32, 95, 32, 0x07e0);
  OLEDrgb_DrawLine(&oledrgb, 0, 48, 80, 48, 0x07e0);
   int16_t xAxis = 0;
   int16_t yAxis = 0;
   int16_t zAxis = 0;
   int8_t temp = 0;
   int trig = 0;

   print("Starting...\n\r");
   while (1) {
      usleep(500000);

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
      }
   }
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
