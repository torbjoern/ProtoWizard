#include "color_utils.h"

#include <math.h>

void protowizard::hsv_to_rgb(float H, float S, float V, float &R, float &G, float &B)
{

	if( V == 0 ) 
	{ 
		R = 0; G = 0; B = 0;
	} 
	else if( S == 0 )                                                   
	{                                                                   
		R = V;  G = V;  B = V;                                                            
	}                                                                   
	else                                                                
	{                                                                   
		float hf = H / 60.0f;                                       
		int    i  = (int)floor( hf );                              
		float f  = hf - i;                                         
		float pv  = V * ( 1 - S );                                 
		float qv  = V * ( 1 - S * f );                             
		float tv  = V * ( 1 - S * ( 1 - f ) );                     
		switch( i )                                                       
		{
		case 0:                                                         
			R = V;  G = tv; B = pv;                                                       
			break; 
		case 1:
			R = qv; G = V; B = pv;
			break;
		case 2:
			R = pv; G = V; B = tv;
			break; 
		case 3:
			R = pv; G = qv; B = V;
			break;
		case 4:
			R = tv; G = pv; B = V;
			break;   
		case 5:
			R = V; G = pv; B = qv;
			break; 
		case 6:
			R = V; G = tv; B = pv;
			break;
		case -1:
			R = V; G = pv; B = qv;
			break;
		} 
	}
}