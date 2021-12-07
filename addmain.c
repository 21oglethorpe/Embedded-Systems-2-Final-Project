int main(){
int playerc, playerr, n, e, s, w;

while( //player is not at coordinates for end) // ADD THIS CODE TO WHILE LOOP FOR GYROSCOPE
  {
     if(within_neg(xAxis)) //east
     {
       if(w ==1) w = 0;
       else e = 1;
     }
    else if(within_pos(xAxis)) //west
    {
      if(e ==1) e = 0;
       else w = 1;
    }
    if(within_neg(yAxis)) //south
     {
       if(n ==1) n = 0;
       else s = 1;
     }
    else if(within_pos(yAxis)) //north
    {
      if(s ==1) s = 0;
       else n = 1;
    }
    if(n) move(1, &playerc, &playerr);
    if(e) move(2, &playerc, &playerr);
    if(s) move(3, &playerc, &playerr);
    if(w) move(4, &playerc, &playerr);
    
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
  case 1: //n
      (*row)--;
  case 2: //e
      (*col)++;
  case 3: //s
      (*row)++;
  case 4: //w
      (*col)--;
  }
  OLEDrgb_DrawPixel(&oledrgb, *col , *row, 0x07e0);
}

int within_neg( int16_t numb)
{
  if((int) numb < 0xEFFF && (int) numb > 0x8000) //must be below threshhold and negative
    return 1;
    else
    return 0; 
}

int within_pos( int16_t numb)
{
  if((int) numb > 0x1000 && (int) numb < 0x7FFF) //must not be negative and over threshhold 
    return 1;
    else
    return 0; 
}

int is_stable( int16_t numb)
{
  if((int) numb < 0x1000 || (int) numb > 0x7FFF) //stable is within 0x1000
    return 1;
    else
    return 0; 
}
