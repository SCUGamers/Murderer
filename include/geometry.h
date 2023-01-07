#ifndef GEOMETRY_H
#define GEOMETRY_H
#include<bits/stdc++.h>
#define db double
#define vec Point
#define eps 1e-8
const db pi=acos(-1);
int syn(db x) { return x<-eps?-1:(x>eps?1:0); } db Abs(db x) { return x*syn(x); }
struct Point { db x,y; Point (db _x=0.0,db _y=0.0) { x=_x; y=_y; } void print(char c) { std::cout<<x<<' '<<y<<c; } } ;
struct Line { Point a,b; Line(Point _,Point $) { a=_,b=$; } Line(db ax=0,db ay=0,db bx=0,db by=0) { a=Point(ax,ay); b=Point(bx,by); } } ;
bool operator == (Point a,Point b) { return syn(a.x-b.x)==0&&syn(a.x-b.y)==0; }
vec operator+(vec a,vec b){return vec(a.x+b.x,a.y+b.y);}
vec operator-(vec a,vec b){return vec(a.x-b.x,a.y-b.y);}
vec operator*(vec a,db b) {return vec(a.x*b,a.y*b);}
vec operator/(vec a,db b) {return vec(a.x/b,a.y/b);}
db operator *(vec a,vec b){return a.x*b.y-a.y*b.x;}
db operator ^(vec a,vec b){return a.x*b.x+a.y*b.y;}
bool chk_rak(Point p,Point l,Point r) { return syn((l-p)*(r-p))<0; }
bool operator < (const Point &x,const Point &y) { return (x.x==y.x)?x.y<y.y:x.x<y.x; }
bool operator < (const Line &_,const Line &$) { int x=syn((_.b-_.a)*($.b-$.a)); return x==0?(chk_rak($.a,_.b,$.b)):x>0; }
db len(vec a){return sqrt(a^a);}
vec turn(vec a,db b) { return vec(a.x*cos(b)-a.y*sin(b),a.x*sin(b)+a.y*cos(b)); }
vec turn(vec a,Point b,db c) { return b+turn(a-b,c); }
db ang(vec a) { return atan2(a.y,a.x); }
bool P_on_seg(Point a,Line b) { return !syn((a-b.a)*(b.b-b.a))&&syn((a-b.a)^(a-b.b))<=0; }
bool P_on_ray(Point a,Line b) { return !syn((a-b.a)*(b.b-b.a))&&syn((a-b.a)^(b.b-b.a))>=0; }
bool P_on_lin(Point a,Line b) { return !syn((a-b.a)*(b.b-b.a)); }
Point FootPoint(Point a,Line b) { vec x=a-b.a,y=a-b.b,z=b.b-b.a; db l1=(x^z),l2=-1.0*(y^z); return b.a+z*(l1/(l1+l2)); }
Point Sym_PL(Point a,Line b) { return 2*FootPoint(a,b)-a; }
db P_to_seg(Point a,Line b) { Point x=FootPoint(a,b); return (syn((a-b.a)^(a-b.b))<=0)?len(a-x):((syn((a-b.a)^(b.b-b.a))>=0)?len(b.b-a):len(b.a-a)); }
db P_to_ray(Point a,Line b) { Point x=FootPoint(a,b); return (syn((a-b.a)^(b.b-b.a))>=0)?len(a-x):len(a-b.a); }
db P_to_lin(Point a,Line b) { return len(a-FootPoint(a,b)); }
Point Inter_LL(Line a,Line b) { vec x=a.b-a.a,y=b.b-b.a,z=a.a-b.a; return a.a+x*((y*z)/(x*y)); }
bool chk_Inter_LR(Line a,Line b) { return P_on_ray(Inter_LL(a,b),b); }
bool chk_Inter_LS(Line a,Line b) { return P_on_ray(Inter_LL(a,b),b); }
bool chk_Inter_RR(Line a,Line b) { Point x=Inter_LL(a,b); return P_on_ray(x,a)&&P_on_ray(x,b); }
bool chk_Inter_RS(Line a,Line b) { Point x=Inter_LL(a,b); return P_on_ray(x,a)&&P_on_seg(x,b); }
bool chk_Inter_SS(Line a,Line b) {
	if(std::max(a.a.y,a.b.y)<std::min(b.a.y,b.b.y)||std::max(b.a.y,b.b.y)<std::min(a.a.y,a.b.y)) return false;
	if(std::max(a.a.y,a.b.y)<std::min(b.a.y,b.b.y)||std::max(b.a.y,b.b.y)<std::min(a.a.y,a.b.y)) return false;
	return syn((a.b-a.a)*(b.a-a.a))*syn((a.b-a.a)*(b.b-a.a))<0&&syn((b.b-b.a)*(a.a-b.a))*syn((b.b-b.a)*(a.b-b.a))<0;
}
#define Polygon std::vector<Point>
#define Con_Hul std::vector<Point>
db area(const Polygon &p) { int n=p.size(); db res=p[n-1]*p[0]; for(int i=1;i<n;i++) res+=p[i-1]*p[i]; return res/2.0; }
db peri(const Polygon &p) { int n=p.size(); db res=len(p[n-1]-p[0]); for(int i=1;i<n;i++) res+=len(p[i-1]-p[i]); return res; }
int chk_P_in_Po(const Polygon &p,Point a) {
	int res=0,n=p.size();
	for(int i=0;i<n;i++) {
		int j=i+1; if(j==n) j=0;
		if(P_on_seg(a,Line(p[i],p[j]))) return 2;
		if(syn(a.y-std::min(p[i].y,p[j].y))>=0&&syn(a.y-std::max(p[i].y,p[j].y))<0) res+=syn(p[i].x+(a.y-p[i].y)/(p[j].y-p[i].y)*(p[j].x-p[i].x)-a.x)>0;
	} return res&1;
}
int chk_P_in_Co(const Con_Hul &p,Point a) { int n=p.size();
	if(chk_rak(p[0],p[1],a)||chk_rak(p[0],a,p[n-1])) return 0;
	if(P_on_seg(a,Line(p[0],p[1]))||P_on_seg(a,Line(p[0],p[n-1]))) return 2;
	int l=1,r=n-2,ans=1;
	while(l<=r) {
		int mid=l+r>>1;
		if(chk_rak(p[0],a,p[mid])) ans=mid,l=mid+1; else r=mid-1;
	} int x=syn((a-p[ans])*(p[ans+1]-p[ans]));
	if(x>0) return 0; else if(x==0) return 2; return 1;
}
bool chk_L_in_Po(const Polygon &p,Line a) {
	int n=p.size(); for(int i=0;i<n;i++) {
		int j=i+1; if(j==n) j=0; Point x=Inter_LL(a,Line(p[i],p[j]));
		if(!(x==a.a||x==a.b||(!chk_Inter_SS(a,Line(p[i],p[j]))))) return false;
	} return chk_P_in_Po(p,a.a)&&chk_P_in_Po(p,a.b);
}
bool chk_L_in_Co(const Con_Hul &p,Line a) { return chk_P_in_Co(p,a.a)&&chk_P_in_Co(p,a.b); }
Con_Hul Andrew(Polygon p) {
	std::vector<Point>res; sort(p.begin(),p.end());
	int n=p.size();
	for(int i=0;i<n;i++) {
		while(res.size()>1&&syn((res[res.size()-1]-res[res.size()-2])*(p[i]-res[res.size()-2]))<=0) res.pop_back();
		res.push_back(p[i]);
	} int k=res.size();
	for(int i=n-2;~i;i--) {
		while(res.size()>k&&syn((res[res.size()-1]-res[res.size()-2])*(p[i]-res[res.size()-2]))<=0) res.pop_back();
		res.push_back(p[i]);
	} res.pop_back(); return res;
}
bool chk_PP(const Polygon &A,const Polygon &B) {
	for(Point x:A) if(chk_P_in_Po(B,x)) return 0; for(Point x:B) if(chk_P_in_Po(A,x)) return 0;
	int n=A.size(),m=B.size();
	for(int i=0;i<n;i++) { int j=i+1; if(j==n) j=0; for(int k=0;k<m;k++) { int l=k+1; if(l==m) l=0; if(chk_Inter_SS(Line(A[i],A[j]),Line(B[k],B[l]))) return 0; } }
	return 1;
}
bool chk_con_PP(const Polygon &A,const Polygon &B) { for(Point x:B) if(!chk_P_in_Po(A,x)) return 0; return 1; }
#undef db
#undef vec
#undef eps
#undef Polygon
#endif