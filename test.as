  void main(int mousx, int mousy)
  {
	cls(0,0,0);
	
	circle(mousx,mousy,10.0f);

	float slider = float(mousx)/float(mousy);
	
	//-25.0f + 50.0f * slider
	drawsph( 0.0f , 0.0f, 0.0f, 3.0f );
  }