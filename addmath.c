#include "addmath.h"

/*
*********************************************************************************************************
*                        CONVERT A FLOATING POINT NUMBER TO STRING WITH 1 DECIMAL PLACE
*
* Description :	This function converts a floating point to a null terminated string 
*				with 1 decimal place.
*			
*				Examples:
*					
*				float f = 9.567;
*				ftoa(&s[0], f);		//s[]={'9','.','5', 0}
*				float f = -0.189;
*				ftoa(&s[0], f);  	//s[]={'-', '0', '.', '1', 0}
* Arguments   : 'unsigned char* buf' 	is the pointer to the string holding the conversion result
*				'float f'				is the input floating point
* Returns     : Returns the string with unsigned char* buf pointing to.
* Notes		  : This routine modified from itoa10() in ..\sample\misc folder of ht-picc
*				If more decimal places required, modify the last section of this code
*				Range of f in (-3,276.7, 3,276.7)
*				This function does print result like 0.0.
*********************************************************************************************************
*/
void ftoa(unsigned char *buf, float f) {
	unsigned int rem;
	unsigned char *s,length=0;
	int i;

	i = (int)((float)f*10);

	s = buf;
	if (i == 0){ 		//print 0.0 with null termination here
		*s++ = '0';
		*s++ = '.';
		*s++ = '0'; 
		*s=0; 			//null terminate the string
	} else {	
		if (i < 0) {
			*buf++ = '-';
			s = buf;
			i = -i;
		}
		//while-loop to "decode" the long integer to ASCII by append '0', string in reverse manner
		//If it is an integer of 124 -> string = {'4', '2', '1'}
		while (i) {
			++length;
			rem = i % 10;
			*s++ = rem + '0';
			i /= 10;
		}
		//reverse the string in this for-loop, string became {'1', '2', '4'} after this for-loop
		for(rem=0; ((unsigned char)rem)<length/2; rem++) {
			*(buf+length) = *(buf+((unsigned char)rem));
			*(buf+((unsigned char)rem)) = *(buf+(length-((unsigned char)rem)-1));
			*(buf+(length-((unsigned char)rem)-1)) = *(buf+length);
		}

		/* Take care of the special case of 0.x if length ==1*/	
		if(length==1) {
			*(buf+2) = *buf;
			*buf = '0';
			*(buf+1) = '.';
			*(s+2)=0; 			//null terminate
		} else {
			*(buf+length) = *(buf+length-1);
			*(buf+length-1)='.';
			*(s+1)=0; 			//null terminate
		}
	}
}
