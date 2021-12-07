int main(){
int playerc, playerr;
int *col = &playerc;
int *row = &playerr;
while( //player is not at coordinates for end) // ADD THIS CODE TO WHILE LOOP FOR GYROSCOPE
  {
     if(
  }
}

include <time.h>;    
 void move(int dir, int &col, int &row) {
    int msec = 0, trigger = 500; /* 10ms */ 
    clock_t before = clock();
    do {
   
      clock_t difference = clock() - before;
        msec = difference * 1000 / CLOCKS_PER_SEC;
} while ( msec < trigger );
  //add collision test here
   OLEDrgb_DrawPixel(&oledrgb, *col , *row, 0);
  switch(dir){
  case 1: 
      (*row)--;
  case 2: 
      (*col)++;
  case 3: 
      (*row)++;
  case 4:
      (*col)--;
  }
  OLEDrgb_DrawPixel(&oledrgb, *col , *row, 0x07e0);
}

int within_neg( int16_t numb)
{
  if((int) numb < 0xEFFF && (int) numb > 0x8000)
    return 1;
    else
    return 0; 
}

int within_pos( int16_t numb)
{
  if((int) numb > 0x1000 && (int) numb > 0x7FFF)
    return 1;
    else
    return 0; 
}

int is_stable( int16_t numb)
{
  if((int) numb < 0x1000 || (int) numb < 0x7FFF)
    return 1;
    else
    return 0; 
}
