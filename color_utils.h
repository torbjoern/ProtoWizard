/*
================
hsv to rgb. Hue Saturation Value to Red Green Blue
rgb values from 0-1f
================
*/
#pragma once



namespace protowizard
{
	void hsv_to_rgb(float hue, float sat, float val, float &red, float &gre, float &blu);
};