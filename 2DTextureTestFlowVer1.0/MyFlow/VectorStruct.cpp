#include "VectorStruct.h"

/*
	double2次元ベクトル
*/

double2::double2():
x(0.0),
y(0.0)
{
}

double2::double2(const double fx, const double fy):
x(fx),
y(fy)
{
}

double2::double2(const double f):
x(f),
y(f)
{
}

double2::double2(const double2 &ob):
x(ob.x),
y(ob.y)
{
}

double2::double2(const int2 &ob):
x(ob.x),
y(ob.y)
{
}

double2 double2::operator+(const double2 &ob) const
{
	return double2(x + ob.x, y + ob.y);
}

double2 double2::operator+(const double f)
{ 
	return double2(x + f, y + f);
}

double2 operator+(const double f, const double2 &ob)
{ 
	return double2(f + ob.x, f + ob.y);
}

double2 double2::operator-(const double2 &ob) const
{
	return double2(x - ob.x, y - ob.y);
}

double2 double2::operator-(const double f)
{
	return double2(x - f, y - f);
}

double2 operator-(const double f, const double2 &ob)
{
	return double2(f - ob.x, f - ob.y);
}

double2 double2::operator*(const double f)
{
	return double2(x * f, y * f);
}

double2 operator*(const double f, const double2 &ob)
{
	return double2(f * ob.x, f * ob.y);
}

double2 & double2::operator=(const double2 &ob)
{
	x = ob.x;
	y = ob.y;
	
	return *this;
}

double2 & double2::operator=(double f)
{
	x = f;
	y = f;
	
	return *this;
}

double2 & double2::operator+=(const double2 &ob)
{
	x += ob.x;
	y += ob.y;
	
	return *this;
}

double2 & double2::operator+=(double f)
{
	x += f;
	y += f;
	
	return *this;
}

double2 & double2::operator-=(const double2 &ob)
{
	x -= ob.x;
	y -= ob.y;
	
	return *this;
}

double2 & double2::operator-=(double f)
{
	x -= f;
	y -= f;
	
	return *this;
}

double2 & double2::operator*=(double f)
{
	x *= f;
	y *= f;
	
	return *this;
}

double2 & double2::operator/=(double f)
{
	x /= f;
	y /= f;
	
	return *this;
}

bool double2::operator==(const double2 &ob) const
{
	return ( x == ob.x && y == ob.y)? true : false;
}

bool double2::operator!=(const double2 &ob) const
{
	return ( x == ob.x && y == ob.y)? false : true;
}

double double2::GetLength() const
{
	return (double)sqrt(x * x + y * y);
}

double double2::GetLengthSq() const
{
	return x * x + y * y;
}

double2 & double2::Normalize()
{
	double temp = GetLength();
	
	if (temp != 0.0) {
		x /= temp;
		y /= temp;
	}

	return *this;
}

double double2::Dot(const double2 &ob0, const double2 &ob1)
{
	return ob0.x * ob1.x + ob0.y * ob1.y;
}

double double2::Cross(const double2 &ob0, const double2 &ob1)
{
	return ob0.x * ob1.y - ob0.y * ob1.x;
}


/*
	double3次元ベクトル
*/
double3::double3():
x(0.0),
y(0.0),
z(0.0)
{
}

double3::double3(const double fx, const double fy, const double fz):
x(fx),
y(fy),
z(fz)
{
}

double3::double3(const double f):
x(f),
y(f),
z(f)
{
}

double3::double3(const double3 &ob):
x(ob.x),
y(ob.y),
z(ob.z)
{
}
/*
double3::double3(const int3 &ob):
x(ob.x),
y(ob.y),
z(ob.z)
{
}
*/
double3 double3::operator+(const double3 &ob) const
{
	return double3(x + ob.x, y + ob.y, z + ob.z);
}

double3 double3::operator+(const double f)
{ 
	return double3(x + f, y + f, z + f);
}

double3 operator+(const double f, const double3 &ob)
{ 
	return double3(f + ob.x, f + ob.y, f + ob.z);
}

double3 double3::operator-(const double3 &ob) const
{
	return double3(x - ob.x, y - ob.y, z - ob.z);
}

double3 double3::operator-(const double f)
{
	return double3(x - f, y - f, z - f);
}

double3 operator-(const double f, const double3 &ob)
{
	return double3(f - ob.x, f - ob.y, f - ob.z);
}

double3 double3::operator*(const double f)
{
	return double3(x * f, y * f, z * f);
}

double3 operator*(const double f, const double3 &ob)
{
	return double3(f * ob.x, f * ob.y, f * ob.z);
}

double3 & double3::operator=(const double3 &ob)
{
	x = ob.x;
	y = ob.y;
	z = ob.z;

	return *this;
}

double3 & double3::operator=(double f)
{
	x = f;
	y = f;
	z = f;
	
	return *this;
}

double3 & double3::operator+=(const double3 &ob)
{
	x += ob.x;
	y += ob.y;
	z += ob.z;
	
	return *this;
}

double3 & double3::operator+=(double f)
{
	x += f;
	y += f;
	z += f;
	
	return *this;
}

double3 & double3::operator-=(const double3 &ob)
{
	x -= ob.x;
	y -= ob.y;
	z -= ob.z;
	
	return *this;
}

double3 & double3::operator-=(double f)
{
	x -= f;
	y -= f;
	z -= f;
	
	return *this;
}

double3 & double3::operator*=(double f)
{
	x *= f;
	y *= f;
	z *= f;
	
	return *this;
}

double3 & double3::operator/=(double f)
{
	x /= f;
	y /= f;
	z /= f;

	return *this;
}

bool double3::operator==(const double3 &ob) const
{
	return ( x == ob.x && y == ob.y && z == ob.z)? true : false;
}

bool double3::operator!=(const double3 &ob) const
{
	return ( x == ob.x && y == ob.y && z == ob.z)? false : true;
}

double double3::GetLength() const
{
	return sqrt(x * x + y * y + z * z);
}

double double3::GetLengthSq() const
{
	return x * x + y * y + z * z;
}

double3 & double3::Normalize()
{
	double temp = GetLength();

	if (temp > 0.0) {
		x /= temp;
		y /= temp;
		z /= temp;
	}

	return *this;
}

double double3::Dot(const double3 &ob0, const double3 &ob1)
{
	return ob0.x * ob1.x + ob0.y * ob1.y;
}

double3 & double3:: Cross(const double3 &ob0, const double3 &ob1) 
{ 
	x = ob0.y * ob1.z - ob0.z * ob1.y;
	y = ob0.z * ob1.x - ob0.x * ob1.z;
	z = ob0.x * ob1.y - ob0.y * ob1.x;

	return *this;
}


//
//	int2次元ベクトル
//
int2::int2():
x(0),
y(0)
{
}

int2::int2(const int fx, const int fy):
x(fx),
y(fy)
{
}

int2::int2(const int f):
x(f),
y(f)
{
}

int2::int2(const int2 &ob):
x(ob.x),
y(ob.y)
{
}

int2::int2(const double2 &ob):
x((int)ob.x),
y((int)ob.y)
{
}

int2 int2::operator+(const int2 &ob) const
{
	return int2(x + ob.x, y + ob.y);
}

int2 int2::operator+(const int f)
{ 
	return int2(x + f, y + f);
}

int2 operator+(const int f, const int2 &ob)
{ 
	return int2(f + ob.x, f + ob.y);
}

int2 int2::operator-(const int2 &ob) const
{
	return int2(x - ob.x, y - ob.y);
}

int2 int2::operator-(const int f)
{
	return int2(x - f, y - f);
}

int2 operator-(const int f, const int2 &ob)
{
	return int2(f - ob.x, f - ob.y);
}

int2 int2::operator*(const int f)
{
	return int2(x * f, y * f);
}

int2 operator*(const int f, const int2 &ob)
{
	return int2(f * ob.x, f * ob.y);
}

int2 & int2::operator=(const int2 &ob)
{
	x = ob.x;
	y = ob.y;
	
	return *this;
}

int2 & int2::operator=(int f)
{
	x = f;
	y = f;
	
	return *this;
}

int2 & int2::operator+=(const int2 &ob)
{
	x += ob.x;
	y += ob.y;
	
	return *this;
}

int2 & int2::operator+=(int f)
{
	x += f;
	y += f;
	
	return *this;
}

int2 & int2::operator-=(const int2 &ob)
{
	x -= ob.x;
	y -= ob.y;
	
	return *this;
}

int2 & int2::operator-=(int f)
{
	x -= f;
	y -= f;
	
	return *this;
}

int2 & int2::operator*=(int f)
{
	x *= f;
	y *= f;
	
	return *this;
}

int2 & int2::operator/=(int f)
{
	x /= f;
	y /= f;
	
	return *this;
}

bool int2::operator==(const int2 &ob) const
{
	return ( x == ob.x && y == ob.y)? true : false;
}

bool int2::operator!=(const int2 &ob) const
{
	return ( x == ob.x && y == ob.y)? false : true;
}


//
//	int3次元ベクトル
//
int3::int3():
x(0),
y(0),
z(0)
{
}

int3::int3(const int fx, const int fy, const int fz):
x(fx),
y(fy),
z(fz)
{
}

int3::int3(const int f):
x(f),
y(f),
z(f)
{
}

int3::int3(const int3 &ob):
x(ob.x),
y(ob.y),
z(ob.z)
{
}

int3::int3(const double3 &ob):
x((int)ob.x),
y((int)ob.y),
z((int)ob.z)
{
}

int3 int3::operator+(const int3 &ob) const
{
	return int3(x + ob.x, y + ob.y, z + ob.z);
}

int3 int3::operator+(const int f)
{ 
	return int3(x + f, y + f, z + f);
}

int3 operator+(const int f, const int3 &ob)
{ 
	return int3(f + ob.x, f + ob.y, f + ob.z);
}

int3 int3::operator-(const int3 &ob) const
{
	return int3(x - ob.x, y - ob.y, z - ob.z);
}

int3 int3::operator-(const int f)
{
	return int3(x - f, y - f, z - f);
}

int3 operator-(const int f, const int3 &ob)
{
	return int3(f - ob.x, f - ob.y, f - ob.z);
}

int3 int3::operator*(const int f)
{
	return int3(x * f, y * f, z * f);
}

int3 operator*(const int f, const int3 &ob)
{
	return int3(f * ob.x, f * ob.y, f * ob.z);
}

int3 & int3::operator=(const int3 &ob)
{
	x = ob.x;
	y = ob.y;
	z = ob.z;
	
	return *this;
}

int3 & int3::operator=(int f)
{
	x = f;
	y = f;
	z = f;
	
	return *this;
}

int3 & int3::operator+=(const int3 &ob)
{
	x += ob.x;
	y += ob.y;
	z += ob.z;
	
	return *this;
}

int3 & int3::operator+=(int f)
{
	x += f;
	y += f;
	z += f;
	
	return *this;
}

int3 & int3::operator-=(const int3 &ob)
{
	x -= ob.x;
	y -= ob.y;
	z -= ob.z;
	
	return *this;
}

int3 & int3::operator-=(int f)
{
	x -= f;
	y -= f;
	z -= f;
	
	return *this;
}

int3 & int3::operator*=(int f)
{
	x *= f;
	y *= f;
	z *= f;
	
	return *this;
}

int3 & int3::operator/=(int f)
{
	x /= f;
	y /= f;
	z /= f;
	
	return *this;
}

bool int3::operator==(const int3 &ob) const
{
	return ( x == ob.x && y == ob.y && z == ob.z)? true : false;
}

bool int3::operator!=(const int3 &ob) const
{
	return ( x == ob.x && y == ob.y && z == ob.z)? false : true;
}