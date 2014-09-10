#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

struct vec3{
	vec3():x(0),y(0),z(0){}
	vec3(double x,double y,double z) : x(x),y(y),z(z){}
	double x,y,z;
	double Mag2();
	vec3 UnitVector();
};
vec3 operator+(vec3 lhs, vec3 rhs){
	return vec3(lhs.x+rhs.x,lhs.y+rhs.y,lhs.z+rhs.z);
}
vec3 operator*(double d, vec3 v){
	return vec3(d*v.x,d*v.y,d*v.z);
}
double operator*(vec3 lhs, vec3 rhs){
	return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}
vec3 operator-(vec3 lhs,vec3 rhs){
	return lhs + (-1)*rhs;
}
vec3 vec3::UnitVector(){
	return (1/std::sqrt((*this)*(*this)))*(*this);
}

//Other scene variables.
vec3 light_dir(-0.32,0.81,-0.49);
vector<vec3> spheres;

// Normal will contain the normal to the last sphere.
vec3 scattering_sphere;
double next_intersection(vec3 pos, vec3 dir){
	double min_dist = 0;
	for(auto s : spheres){
		double B = dir*(pos-s);
		double C = (pos-s)*(pos-s)-1;
		if(B<0 && C<B*B){
			double dist = -B-sqrt(B*B-C);
			if(min_dist==0 || dist<min_dist){
				min_dist = dist;
				scattering_sphere = s;
			}
		}
	}
	return min_dist;
}

bool shadow_test(vec3 pos){
	return !next_intersection(pos,light_dir);
}

vec3 color_of(vec3 pos, vec3 dir){
	dir = dir.UnitVector();
	vec3 col(0,0,0);
	double color_remaining = 1;
	//Run through all intersections.
	while(double dist = next_intersection(pos,dir)){
		pos = pos + (dist-1e-5)*dir;
		vec3 normal = (pos-scattering_sphere).UnitVector();
		vec3 delta_dir = -2*(dir*normal)*normal;
		dir = (dir+delta_dir).UnitVector();

		double diffuse_col = 0.6*(normal*light_dir);
		col = col + color_remaining*0.8*(0.2 + (!next_intersection(pos,light_dir))*diffuse_col)*vec3(1,1,1);
		color_remaining *= 0.2;
	}

	//Color of ground or sky
	vec3 end_color;
	if(dir.y<0){ //Ground, checkered red and white
		double x = pos.x - ((pos.y+5)*dir.x/dir.y);
		double z = pos.z - ((pos.y+5)*dir.z/dir.y);
		int other_color = ((int(x)+int(z))%2+2)%2;
		end_color = (next_intersection({x,-5,z},light_dir)?0.2:1.0)*vec3(1,other_color,other_color);
	} else { //Sky, blue, growing dark at the horizon.
		end_color = vec3(0,0,dir.y);
	}
	col = col + color_remaining*end_color;

	return col;
}

vec3 color_of_pixel(int i, int j){
	const int n_repeat = 1;
	vec3 color;
	for(int k=0; k<n_repeat; k++){
		vec3 pos(10,0,-10);
		vec3 dir(3e-3*(i-320+1e-2*(rand()%100-50)),
						 -3e-3*(j-320+1e-2*(rand()%100-50)),
						 1);
		color = color + (1.0/n_repeat)*color_of(pos,dir);
	}
	return color;
}

int main(){
	printf("Hello\n");

	char sphere_list[] = "001020010212030414244041425270717390:09192:2";
	char* p = sphere_list;
	while(*p){
		int i = *p++ - 48;
		int j = *p++ - 48;
		spheres.push_back({2*i,2*j,5});
	}

	unsigned char colors[3*640*480];
	unsigned char* c = colors;
	for(int j=0; j<480; j++){
		for(int i=0; i<640; i++){
			vec3 color = color_of_pixel(i,j);
			*c++ = color.x*255;
			*c++ = color.y*255;
			*c++ = color.z*255;
		}
	}
	FILE* f = fopen("output.ppm","wb");
	fputs("P6 640 480 255\n",f);
	fwrite(colors, 1, 3*640*480, f);
	fclose(f);

	printf("Goodbye\n");
}
