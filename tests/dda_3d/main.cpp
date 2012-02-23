#include <protographics.h>
#include "color_utils.h"

double BSIZ = 32;


void drawblock(double x, double y)
{
   float x0 = x*BSIZ; float x1 = x0+BSIZ;
   float y0 = y*BSIZ; float y1 = y0+BSIZ;
   ProtoGraphics *proto = ProtoGraphics::getInstance();
   for(int y=y0+1;y<y1;y++) { proto->moveTo(x0+1,y); proto->lineTo(x1,y); }
}

void drawblock2(double x, double y)
{
   float x0 = x*BSIZ-2; float x1 = x0+BSIZ+2;
   float y0 = y*BSIZ-2; float y1 = y0+BSIZ+2;
   ProtoGraphics *proto = ProtoGraphics::getInstance();
   proto->moveTo(x0,y0); 
   proto->lineTo(x1,y0);
   proto->lineTo(x1,y1);
   proto->lineTo(x0,y1);
   proto->lineTo(x0,y0);
}

void raycast(double x1,double y1,double z1,double x2,double y2,double z2)
{
	// Determine start grid cell coordinates
	int i = int(floor(x1));
	int j = int(floor(y1));
	int k = int(floor(z1));

	// End grid cell coordinates
	int iend = int(floor(x2));
	int jend = int(floor(y2));
	int kend = int(floor(z2));

	// The real-value difference
	double dx = x2 - x1;
	double dy = y2 - y1;
	double dz = z2 - z1;
      
	// Determine in which primary dir to step (-1 or +1)
	int di = (dx>0)*2-1;
	int dj = (dy>0)*2-1;
	int dk = (dz>0)*2-1;

	// How much of the line-segment (x1,y1 to x2,y2) do we step in each axis?
	double deltatx = 1.0 / fabs(dx);
	double deltaty = 1.0 / fabs(dy);
	double deltatz = 1.0 / fabs(dz);

	// Determine the parameter t where ray hits cell boundry of each axis; tx,ty,tz
	// min(x,y,z) determines how far one can travel in that dir before hitting another cell
	double minx = floor(x1); double maxx = minx + 1.0;
	double miny = floor(y1); double maxy = miny + 1.0;
	double minz = floor(z1); double maxz = minz + 1.0;
	double tx = x1>x2 ? (x1-minx)*deltatx : (maxx-x1)*deltatx;
	double ty = y1>y2 ? (y1-miny)*deltaty : (maxy-y1)*deltaty;
	double tz = z1>z2 ? (z1-minz)*deltatz : (maxz-z1)*deltatz;

	// A loop where we visit a cell[i][j][k]
	// exit the loop under following conditions
	// * when we've met the desired end point
	// * we've found what we wanted in cell[i][j][k]
	// * stepped out of grid
	while(1)
	{
		// Visit cell x,y
		drawblock2(i,j);
		if(tx <= ty && tx <= tz) // tx smallest, step in x
		{
			if(i == iend) break;
			tx += deltatx;
			i += di;
		} else if (ty <= tz)
		{
			if(j == jend) break; // ty smallest, step in y
			ty += deltaty;
			j += dj;
		} else 
		{
			if(k == kend) break; // tz smallest, step in z
			tz += deltatz;
			k += dk;
		}
	}
}

void blockscan (double x0, double y0, double x1, double y1)
{   
   const double PREC=pow(2.,16.0);
   double ix = floor(x0); double ix1 = floor(x1)-ix; double ixi = (ix1>0)*2-1;
   double iy = floor(y0); double iy1 = floor(y1)-iy; double iyi = (iy1>0)*2-1;
   double fx = x0-ix; if (ixi > 0) fx = 1-fx;
   double fy = y0-iy; if (iyi > 0) fy = 1-fy;
   double gx = fabs(x1-x0)*PREC; int idx = int(gx);
   double gy = fabs(y1-y0)*PREC; int idy = int(gy);
   int id = 0;
        if (ix1 == 0) { id = -1; idx = 0; }
   else if (iy1 == 0) { id =  0; idy = 0; }
   else               { id = int(fy*gx - fx*gy); }
   for(int c=fabs(ix1)+fabs(iy1);c>=0;c--)
   {
      drawblock(ix,iy);
      if (id >= 0) { ix += ixi; id -= idy; }
              else { iy += iyi; id += idx; }
   }
}



int main()
{
	ProtoGraphics proto;

	if (!proto.init(640,480) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}


	proto.setFrameRate( 60 );
	
	double sx,sy;
	sx = sy = 0.0;
	
	
	char title_buf[256];
	do
	{
		proto.cls(0.f, 0.f, 0.f);

		float mspf = proto.getMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, %3.0f", 1000.f * mspf, 1.f/mspf);
		proto.setTitle( title_buf );

		float normalized_mx = proto.getMouseX() / (float)proto.getWindowWidth();
		float normalized_my = proto.getMouseY() / (float)proto.getWindowHeight();
		glm::vec2 normalized_mouse(normalized_mx, normalized_my);

		proto.setColor(1.f, 0.f, 0.f);
		blockscan( sx, sy, proto.getMouseX()/BSIZ , proto.getMouseY()/BSIZ );
		
		proto.setColor(0.f, 1.f, 0.f);
		raycast( sx, sy, 0., proto.getMouseX()/BSIZ , proto.getMouseY()/BSIZ, 0. );

		if ( proto.mouseDownLeft() ){
			sx = proto.getMouseX()/BSIZ;
			sy = proto.getMouseY()/BSIZ;
		}

		proto.setColor(.5f, .5f, .5f);
		proto.moveTo(sx, sy);
		proto.lineTo(proto.getMouseX(), proto.getMouseY());

		//float ang = normalized_mouse.x*6.28f;
		//glm::vec3 cam_pos( cos(ang), 0.f, sin(ang) );
		//cam_pos *= 25.f;
		//proto.setCamera( cam_pos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f) );

		proto.frame();

	} while( proto.isWindowOpen() );


}