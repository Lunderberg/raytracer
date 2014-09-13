#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

using namespace std;


struct vec3{
	vec3():x(0),y(0),z(0){}
	vec3(double x,double y,double z) : x(x),y(y),z(z){}
	double x,y,z;
	vec3 operator+(vec3 a){return vec3(x+a.x, y+a.y, z+a.z);}
	vec3 operator*(double d){return vec3(d*x, d*y, d*z);}
	double operator*(vec3 a){return x*a.x + y*a.y + z*a.z;}
	vec3 operator-(vec3 a){return (*this)+a*(-1);}
	vec3 UnitVector(){return (*this)*(1/std::sqrt(Mag2()));}
	double Mag2(){return (*this)*(*this);}
};

//All the different properties of the spheres.
static vec3 sphere_color(1,1,1);
static double radius = 1;
static double reflectivity = 0.2;
static double ambient = 0.2;
static double diffuse = 0.6;
//Other scene variables.
vec3 light_dir(0.32,-0.81,0.49);
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
	pos = pos + dir*min_dist;
	normal = (pos - scattering_sphere).UnitVector();
	vec3 delta_dir = normal*(-2*(dir*normal));
	dir = (dir + delta_dir).UnitVector();
	return true;
}

bool shadow_test(vec3 pos){
	vec3 temp = light_dir.UnitVector()*(-1);
	return !next_intersection(pos,temp);
}

vec3 color_of(vec3 pos, vec3 dir){
	dir = dir.UnitVector();
	vec3 col(0,0,0);
	double color_remaining = 1;
	//Run through all intersections.
	while(next_intersection(pos,dir)){
		vec3 ambient_col = sphere_color*ambient;
		vec3 diffuse_col = sphere_color*diffuse*(-1)*(normal*light_dir);
		double color_used = color_remaining*(1-reflectivity);
		col = col + (ambient_col + diffuse_col*shadow_test(pos))*color_used;
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
		end_color = vec3(1,other_color,other_color)*(shadow_test({x,ground_height,z})?1.0:ambient);
	} else { //Sky, blue, growing dark at the horizon.
		end_color = vec3(0,0,1)*(dir*vec3(0,1,0));
	}
	col = col + end_color*color_remaining;

	return col;
}

vec3 color_of_pixel(int i, int width, int j, int height){
	const int n_repeat = 20;
	const double pixel_width = 3e-3;
	vec3 color;
	for(int k=0; k<n_repeat; k++){
		vec3 pos(10,0,-10);
		double x = pixel_width*(i-width/2+1e-2*(rand()%100-50));
		double y = -pixel_width*(j-width/2+1e-2*(rand()%100-50));
		vec3 dir(x,y,1);
		color = color + color_of(pos,dir)*(1.0/n_repeat);
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
