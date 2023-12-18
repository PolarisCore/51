#include "timer51.h"

void delay(void){
    int i,j;
    for(i=0;i<0xff;i++)
	 for(j=0;j<0xff;j++);
}