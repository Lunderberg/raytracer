#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

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
double vec3::Mag2(){
	return (*this)*(*this);
}
vec3 vec3::UnitVector(){
	return (1/std::sqrt(Mag2()))*(*this);
}

//All the different properties of the spheres.
static const vec3 sphere_color(1,1,1);
static const double radius = 1;
static const double reflectivity = 0.2;
static const double ambient = 0.2;
static const double diffuse = 0.6;
static const double specular = 0.2;
static const double specular_power = 5;
//Other scene variables.
vec3 light_dir(0.2,-0.5,0.3);
vector<vec3> spheres;

// Normal will contain the normal to the last sphere.
vec3 normal;
double next_intersection(vec3& pos, vec3& dir){
	double min_dist = sqrt(-1);
	vec3 scattering_sphere;
	for(auto s : spheres){
		double B = dir*(pos-s);
		double C = (pos-s).Mag2()-radius*radius;
		if(B<0 && C<B*B){
			double dist = -B-sqrt(B*B-C);
			if(std::isnan(min_dist) || dist<min_dist){
				min_dist = dist;
				scattering_sphere = s;
			}
		}
	}

	// No interactions, return false
	if(std::isnan(min_dist)){
		return false;
	}

	//Update the position and direction, return true.
	pos = pos + (min_dist-1e-5) * dir;
	normal = (pos - scattering_sphere).UnitVector();
	vec3 delta_dir = -2*(dir*normal)*normal;
	dir = (dir + delta_dir).UnitVector();
	return true;
}

bool shadow_test(vec3 pos){
	vec3 temp = (-1)*light_dir.UnitVector();
	return !next_intersection(pos,temp);
}

vec3 color_of(vec3 pos, vec3 dir){
	dir = dir.UnitVector();
	vec3 col(0,0,0);
	double color_remaining = 1;
	//Run through all intersections.
	while(next_intersection(pos,dir)){
		double color_used = color_remaining*(1-reflectivity);

		vec3 light_reflect = -2*(normal*light_dir)*normal + light_dir;

		vec3 ambient_col = ambient*sphere_color;
		vec3 diffuse_col = -diffuse*(normal*light_dir)*sphere_color;
		vec3 specular_col = specular*pow(-(normal*light_reflect),specular_power)*sphere_color;
		col = col + color_used*(ambient_col + shadow_test(pos)*(diffuse_col + specular_col));
		color_remaining -= color_used;
	}

	//Color of ground or sky
	vec3 end_color;
	if(dir.y<0){ //Ground, checkered red and white
		double ground_height = -5;
		double x = pos.x - ((pos.y-ground_height)*dir.x/dir.y);
		double z = pos.z - ((pos.y-ground_height)*dir.z/dir.y);
		int other_color = (int(x)+int(z))%2;
		other_color = abs(other_color);
		end_color = (shadow_test({x,ground_height,z})?1.0:ambient)*vec3(1,other_color,other_color);
	} else { //Sky, blue, growing dark at the horizon.
		end_color = (dir*vec3(0,1,0))*vec3(0,0,1);
	}
	col = col + color_remaining*end_color;

	return col;
}

vec3 color_of_pixel(int i, int width, int j, int height){
	const int n_repeat = 5;
	const double pixel_width = 4e-3;
	vec3 color;
	for(int k=0; k<n_repeat; k++){
		vec3 pos(10,0,-10);
		double x = pixel_width*(i-width/2+1e-2*(rand()%100-50));
		double y = -pixel_width*(j-width/2+1e-2*(rand()%100-50));
		vec3 dir(x,y,1);
		color = color + (1.0/n_repeat)*color_of(pos,dir);
	}
	return color;
}

void write_ppm(const char* filename, unsigned int width, unsigned int height, unsigned char* colors){
	FILE* f = fopen(filename,"wb");
	fprintf(f, "P6 %d %d 255\n", width, height);
	fwrite(colors, 1, 3*width*height, f);
	fclose(f);
}

int main(){
	printf("Hello\n");

	light_dir = light_dir.UnitVector();

	// E
	spheres.push_back({0,0,5});
	spheres.push_back({2,0,5});
	spheres.push_back({4,0,5});
	spheres.push_back({0,2,5});
	spheres.push_back({0,4,5});
	spheres.push_back({2,4,5});
	spheres.push_back({0,6,5});
	spheres.push_back({0,8,5});
	spheres.push_back({2,8,5});
	spheres.push_back({4,8,5});

	// r
	spheres.push_back({8,0,5});
	spheres.push_back({8,2,5});
	spheres.push_back({8,4,5});
	spheres.push_back({10,4,5});

	// i
	spheres.push_back({14,0,5});
	spheres.push_back({14,2,5});
	spheres.push_back({14,6,5});

	// c
	spheres.push_back({18,0,5});
	spheres.push_back({20,0,5});
	spheres.push_back({18,2,5});
	spheres.push_back({18,4,5});
	spheres.push_back({20,4,5});

	const int width = 640;
	const int height = 480;
	const int n_repeat = 20;
	unsigned char colors[3*width*height];
	for(int i=0; i<width; i++){
		for(int j=0; j<height; j++){
			vec3 color = color_of_pixel(i,width,j,height);
			colors[3*(i+j*width)+0] = color.x*255;
			colors[3*(i+j*width)+1] = color.y*255;
			colors[3*(i+j*width)+2] = color.z*255;
		}
	}
	write_ppm("output.ppm",width,height,colors);

	printf("Goodbye\n");
}
