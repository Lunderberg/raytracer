#include <cstdio>
#include <cmath>
#define D double
struct V{V(D x=0,D y=0,D z=0):x(x),y(y),z(z){}V operator+
(V a){return V(x+a.x,y+a.y,z+a.z);}V operator*(D d){return
V(d*x,d*y,d*z);}D x,y,z,operator*(V a){return x*a.x+y*a.y+
z*a.z;}V operator-(V a){return *this+a*-1;}V U(){return *
this*(1/sqrt(*this**this));}};V L(-0.32,0.81,-0.49),g,*H=
new V[22];D N(V p,V d){D m=0;for(V*s=H;++s<H+23;){D B=d*(*
s-p);D C=(p-*s)*(p-*s)-1;if(B>0&&C<B*B){D t=B-sqrt(B*B-C);
if(m==0||t<m){m=t;g=*s;}}}return m;}V F(V d){V p;D f=0,r=1
;while(D t=N(p,d)){p=p+d*t;V n=(p-g).U();d=(d+n*(d*n*-2)).
U();f+=r*0.8*(0.2+n*L*0.6*!N(p,L));r*=.2;}V e;if(d.y<0){D
x=p.x-(p.y+5)*d.x/d.y;D z=p.z-(p.y+5)*d.z/d.y;D R=!!((int(
x)+int(z))%2);e=V(R,R,1)*(N(V(x,-5,z),L)?.2:1);}else e=V(d
.y);return V(1,1,1)*f+e*r;}int a=1;int R(){return a=a*1235
%7919;}int main(){char*p="0*06+,2-.4:BCDJTUW`fabh";while(*
++p)*H++=V(*p/6*2-24,*p%6*2,15);H-=23;char B[921600],*G=B;
for(int j=0;j<480;j++){for(int i=0;i<640;i++){V L;for(int
k=0;k++<99;)L=L+F(V(i-320+R()%50*.02,-j+320+R()%50*.02,333
).U())*.01;*G++=L.z*255;*G++=L.y*255;*G++=L.x*255;}}FILE*f
=fopen("output.ppm","wb");fputs("P6 640 480 255\n",f);
fwrite(B,1,921600,f);}
