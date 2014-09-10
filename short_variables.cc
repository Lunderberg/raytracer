#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#define D double
struct V{	V():x(0),y(0),z(0){}V(D x,D y,D z):x(x),y(y),z(z){}D
x,y,z;V U();};V operator+(V a, V b){return V(a.x+b.x,a.y+b.y,a
.z+b.z);}V  operator*(D d, V v){return V(d*v.x,d*v.y,d*v.z);}D
operator*(V a, V b){return a.x*b.x+a.y*b.y+a.z*b.z;}V operator
-(V a,V b){return a+-1*b;}V V::U(){return 1/std::sqrt((*this)*
(*this))*(*this);}V L(-0.32,0.81,-0.49),S;std::vector<V> H;D N
(V p,V d){D T = 0;for(auto s:H){D B=d*(s-p);D C=(p-s)*(p-s)-1;
if (B>0&&C<B* B) {D t=B-sqrt(B* B-C);if(T==0||t<T){T=t;S=s;}}}
return T;}V F(V p, V d){V C(0,0,0);D r = 1;while(D t = N(p,d))
{p=p+t*d;V n=(p-S).U();d=(d-2*d*n*n).U();C=C+r*0.8*(0.2+(!N(p,
L))*0.6*n*L)*V(1,1,1);r*=0.2;}V e;if(d.y<0){D x=p.x-((p.y+5)*d
.x/d.y);D z=p.z-((p.y+5)*d.z/d.y);int R=!!((int(x)+int(z))%2);
e=(N({x,-5,z},L)?0.2:1)*V(1,R,R);}else{e=V(0,0,d.y);}return C+
r*e;}int main(){	char*p="00102001021203041424404142527071739"
"0:09192:2";while(*p){int i=*p++-48;int j=*p++-48;H.push_back(
{2*i,2*j,5});}char B[3*640*480];char* c=B;for(int j=0;j<480;
j++){for(int i=0;i<640;i++){V C;for(int k=0;k<20;k++){V p(10,0
,-10);V d(3e-3*(i-320+1e-2*(rand()%100-50)),-3e-3*(j-320+1e-2*
(rand()%100-50)),1);C=C+1.0/20*F(p,d.U());}*c++=C.x*255;*c++=C
.y*255;*c++=C.z* 255;}}FILE* f=fopen("output.ppm","wb");fputs(
"P6 640 480 255\n",f);fwrite(B,1,3*640*480,f);fclose(f);}
