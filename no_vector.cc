#include <cstdio>
#include <cmath>

struct vec3{
	vec3(double x=0,double y=0,double z=0) : x(x),y(y),z(z){}
	vec3 operator+(vec3 a){return vec3(x+a.x, y+a.y, z+a.z);}
	vec3 operator*(double d){return vec3(d*x, d*y, d*z);}
	double x,y,z,operator*(vec3 a){return x*a.x + y*a.y + z*a.z;}
	vec3 operator-(vec3 a){return (*this)+a*(-1);}
	vec3 UnitVector(){return (*this)*(1/std::sqrt((*this)*(*this)));}
};

// Other scene variables.
vec3 light_dir(-0.32,0.81,-0.49);
vec3* spheres = new vec3[22];

// Keep track of the most recent scatter sphere
vec3 scattering_sphere;
double next_intersection(vec3 pos, vec3 dir){
	double min_dist = 0;
	for(vec3* s = spheres; ++s<spheres+23;){
		double B = dir*(*s-pos);
		double C = (pos-*s)*(pos-*s)-1;
		if(B>0 && C<B*B){
			double dist = B-sqrt(B*B-C);
			if(min_dist==0 || dist<min_dist){
				min_dist = dist;
				scattering_sphere = *s;
			}
		}
	}
	return min_dist;
}

vec3 color_of(vec3 dir){
	vec3 pos;
	double col = 0;
	double color_remaining = 1;
	//Run through all intersections.
	while(double dist = next_intersection(pos,dir)){
		pos = pos + dir*dist;
		vec3 normal = (pos-scattering_sphere).UnitVector();
		vec3 delta_dir = normal*(-2*(dir*normal));
		dir = (dir+delta_dir).UnitVector();

		double diffuse_col = normal*light_dir*0.6;
		col += color_remaining*0.8*(0.2 + !next_intersection(pos,light_dir)*diffuse_col);
		color_remaining *= 0.2;
	}

	//Color of ground or sky
	vec3 end_color;
	if(dir.y<0){ //Ground, checkered red and white
		double x = pos.x - (pos.y+5)*dir.x/dir.y;
		double z = pos.z - (pos.y+5)*dir.z/dir.y;
		double other_color = !!((int(x)+int(z))%2);
		end_color = vec3(other_color,other_color,1)*(next_intersection(vec3(x,-5,z),light_dir)?0.2:1);
	} else //Sky, blue, growing dark at the horizon.
		end_color = vec3(dir.y);

	return vec3(1,1,1)*col + end_color*color_remaining;
}

int a=1;
int rand(){
	return a = a * 1235 % 7919;
}

int main(){
	printf("Hello\n");

	char* p = "0*06+,2-.4:BCDJTUW`fabh";
	while(*++p)
		*spheres++ = vec3(*p/6*2-24,*p%6*2,15);
	spheres -= 23;

	char col_arr[3*640*480];
	char* c = col_arr;
	for(int j=0; j<480; j++){
		for(int i=0; i<640; i++){
			vec3 color;
			for(int k=0;k++<25;)
				color = color + color_of(vec3(i-320+rand()%50*2e-2,
																			-j+320+rand()%50*2e-2,
																			333).UnitVector())*.04;
			*c++ = color.z*255;
			*c++ = color.y*255;
			*c++ = color.x*255;
		}
	}
	FILE* f = fopen("output.ppm","wb");
	fputs("P6 640 480 255\n",f);
	fwrite(col_arr, 1, 3*640*480, f);

	printf("Goodbye\n");
}
