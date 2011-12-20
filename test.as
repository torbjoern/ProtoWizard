
int numframes = 0;

bool obleft = false;
bool obright = false;

float TWO_PI = 3.14159 * 2;


class vec3{
	vec3(float _x, float _y, float _z){ x=_x; y=_y; z=_z;}

	void opAddAssign( vec3 a ) {
		x += a.x;
		y += a.y;
		z += a.z;
	}
	
	vec3 opMul( float s ){
		return vec3( x*s, y*s, z*s );
	}
	
	vec3 opMul( mat3 m )
	{
		return vec3(x * m.xd.x + y * m.yd.y + z * m.zd.z,
	                x * m.xd.x + y * m.yd.y + z * m.zd.z,
                    x * m.xd.x + y * m.yd.y + z * m.zd.z);
					
	}
					
	
	float x;
	float y;
	float z;
}

float length( vec3 v )
{
	return sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
}

vec3 cross( vec3 a, vec3 b ){
	vec3 o;
	o.x = a.y*b.z - a.z*b.y;
	o.y = a.z*b.x - a.x*b.z;
	o.z = a.x*b.y - a.y*b.x;
	return o;
}

class mat3{
	mat3(vec3 a, vec3 b, vec3 c){
		xd = a;
		yd = b;
		zd = c;
	}
	

	void setRot(){
		setRotation( xd.x, xd.y, xd.z,
					 yd.x, yd.y, yd.z,
					 zd.x, zd.y, zd.z );
	}

	vec3 xd;
	vec3 yd;
	vec3 zd;
}


	/* converts horizontal and vertical angles to a 3x3 rotation matrix */
mat3 ang2mat(float hang, float vang )
{
	//vec3 side( cos(hang), 0.f, -sin(hang) );
	//vec3 fwd( cos(vang)*sin(hang), sin(vang), cos(vang)*cos(hang) );
	//vec3 up = cross( fwd, side );
	float ch = cos(hang);
	float sh = sin(hang);
	float cv = cos(vang);
	float sv = sin(vang);
	
	vec3 side( cv, 0, sv );
	vec3 up( -sh*-sv, ch, -sh*cv );
	vec3 fwd( ch*-sv, sh, ch*cv );
	
	return mat3( side, up, fwd );
}

// side = front,back,left,right,top,bottom
vec3 cube(int side, float s, float t){
	float x,y,z;
   if(side==0){
      x = 1-2*s;
      y = 1-2*t;
      z = 1;
   }else if(side==1){
      x = 1-2*s;
      y = 1-2*t;
      z = -1;
   }else if(side==2){
      x = -1;
      y = 1-2*s;
      z = 1-2*t;
   }else if(side==3){
      x = 1;
      y = 1-2*s;
      z = 1-2*t;
   }else if(side==4){
      x = 1-2*s;
      y = 1;
      z = 1-2*t;
   }else if(side==5){
      x = 1-2*s;
      y = -1;
      z = 1-2*t;
   }
   return vec3(x,y,z);
}

vec3 cam_pos = vec3(0,0,0);
float hang = 0;
float vang = 0;

int old_mouse_x = 0;
int old_mouse_y = 0;

void update_cam(){
	bool k_left = keystatus( 256+29 );
	bool k_right = keystatus( 256+30 );
	bool k_fwd = keystatus( 256+27 );
	bool k_back = keystatus( 256+28 );
	
	bool mousedown = mouseLeft();
	int mousex = getMouseX();
	int mousey = getMouseY();
	
	float delta = 0.01f;
		
	wasdcam( k_left, k_right, k_fwd , k_back ,
			mousex, mousey, mousedown, delta );
}

void onFrame()
{
	if ( numframes == 0 ){
		print("matrices \n");
		//setcam( 0,0,0, 0,0 );
	}
	cls(0.2, 0.2, 0.2);
	
	

	double xres = 800.0;
	double yres = 600.0;

	setcol(255,255,255);
	
	update_cam();
			
	//enableLightblend();
	disableLightblend();
	setalpha(0.8);
	
	setcol(.7,.7,.7);
	mat3 rotmat = ang2mat( numframes * 0.01 , 0 ); //mousex/xres*TWO_PI, 0 );
	rotmat.setRot();
	drawcube(2.5,0,0, 2.0);
	
	mat3 identity;
	identity.xd = vec3( 1, 0, 0 );
	identity.yd = vec3( 0, 1, 0 );
	identity.zd = vec3( 0, 0, 1 );
	identity.setRot();
	
	
	int segs = 32;
	int max_steps = 2;
	for(int step=0; step < max_steps; step++){
	
		float r = 10 + step*1.2;
		for(int i=0; i<segs; i++){
			float a = i * TWO_PI / (segs-1);
			
			mat3 rotmat = ang2mat( 0.0, a );
			rotmat.setRot();
			
			drawcube( cos(a)*r, -step*1.5, sin(a)*r , 1.5 );
		}
		
	}

	// int dim1 = 10;
	
   // for(int side=0; side<6; side++)
   // for(int i=0; i<dim1; i++){
	   // for(int j=0; j<dim1; j++)
	   // {
		// float s = i/float(dim1-1);
		// float t = j/float(dim1-1);
		// vec3 p = cube(side, s, t );
		// setcol(p.x+0.5, p.y+0.5, p.z+0.5);
		// drawcube(p.x * 20, p.y * 20, p.z * 20, 4.0);
	   // }
   // }
   
   setcol(1.0, 0.0, 0.0 );
   drawcone( -10, 0,0,
			  10, 0, 0, 1.0 );
			  
   setcol(0.0, 1.0, 0.0 );
   drawcone(  0, 10,0,
			  0, -10, 0, 1.0 );
   
   setcol(0.0, 0.0, 1.0 );
   drawcone(  0, 0, 10,
			  0, 0, -10, 1.0 );

	old_mouse_x = getMouseX();
	old_mouse_y = getMouseY();

   
	numframes++;
}

