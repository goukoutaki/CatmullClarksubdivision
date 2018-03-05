#ifndef _OBJ_MODEL_
#define _OBJ_MODEL_

#include <stdio.h>
#include <vector>
#include <algorithm> 

using namespace std;

#define EPS 0.0000001f

struct point3d_t {
	float x, y, z;
	point3d_t(){x=z=y=0;}
	point3d_t(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
	}
	bool operator == ( point3d_t & ref) const {
		return (fabs(x - ref.x)<EPS && fabs(y - ref.y)<EPS && fabs(z - ref.z)<EPS);
	}
	bool operator < (point3d_t & ref) const {
		return ((x+y*10+z*100) < (ref.x + ref.y*10+ ref.z*100));
	}
	point3d_t operator + (point3d_t & ref){
		return point3d_t(
			x + ref.x,
			y + ref.y,
			z + ref.z
		);
	}
	point3d_t operator * (float a) {
		return point3d_t(
			x *a,
			y *a,
			z *a
		);
	}
};

struct face_t {
	int v[4];
	unsigned char n;
};

typedef std::vector<face_t>   faces_t;

class obj_t {
public:
	vector<point3d_t>  vs;
	faces_t            fs;

	void load_obj(char *filename);
	void write_obj(char *filename);
};



#endif

