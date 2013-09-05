#ifndef _VECTOR_STRUCT_H
#define _VECTOR_STRUCT_H

#include <cmath>
//using namespace std;

class double2;
class double3;
class int2;
class int3;

//
//	double�Q�����x�N�g��
//
class double2{
	
public:
	double x;
	double y;

	//�R���X�g���N�^
	double2();
	double2(const double dx, const double dy);
	double2(const double d);
	double2(const double2 &ob);
	double2(const int2 &ob);

	//���Z�q
	double2 operator+(const double2 &ob) const;
	double2 operator+(const double f);
	friend double2 operator+(const double f, const double2 &ob);
	double2 operator-(const double2 &ob) const;
	double2 operator-(const double f);
	friend double2 operator-(const double f, const double2 &ob);
	double2 operator*(const double f);
	friend double2 operator*(const double f, const double2 &ob);
	double2 operator/(double f) const;
	double2 & operator=(const double2 &ob);
	double2 & operator=(double f);
	double2 & operator+=(const double2 &ob);
	double2 & operator+=(double f);
	double2 & operator-=(const double2 &ob);
	double2 & operator-=(double f);
	double2 & operator*=(double f);
	double2 & operator/=(double f);
	bool operator==(const double2 &ob) const;
	bool operator!=(const double2 &ob) const;

	//�Z�p�֐�
	double GetLength() const;
	double GetLengthSq() const;
	double2 & Normalize();
	double Dot(const double2 &ob0, const double2 &ob1);
	double Cross(const double2 &ob0, const double2 &ob1);

};

//
//	double3�����x�N�g��
//
class double3{
public:
	double x;
	double y;
	double z;

	//�R���X�g���N�^
	double3();
	double3(const double fx, const double fy, const double fz);
	double3(const double f);
	double3(const double3 &ob);
//	double3(const int3 &ob);

	//���Z�q
	double3 operator+(const double3 &ob) const;
	double3 operator+(const double f);
	friend double3 operator+(const double f, const double3 &ob);
	double3 operator-(const double3 &ob) const;
	double3 operator-(const double f);
	friend double3 operator-(const double f, const double3 &ob);
	double3 operator*(const double f);
	friend double3 operator*(const double f, const double3 &ob);
	double3 operator/(double f) const;
	double3 & operator=(const double3 &ob);
	double3 & operator=(double f);
	double3 & operator+=(const double3 &ob);
	double3 & operator+=(double f);
	double3 & operator-=(const double3 &ob);
	double3 & operator-=(double f);
	double3 & operator*=(double f);
	double3 & operator/=(double f);
	bool operator==(const double3 &ob) const;
	bool operator!=(const double3 &ob) const;

	//�Z�p�֐�
	double GetLength() const;
	double GetLengthSq() const;
	double3 & Normalize();
	double Dot(const double3 &ob0, const double3 &ob1);
	double3 & Cross(const double3 &ob0, const double3 &ob1);

};

//
//	int�Q�����x�N�g��
//
class int2{
public:
	int x;
	int y;

	//�R���X�g���N�^
	int2();
	int2(const int fx, const int fy);
	int2(const int f);
	int2(const int2 &ob);
	int2(const double2 &ob);

	//���Z�q
	int2 operator+(const int2 &ob) const;
	int2 operator+(const int f);
	friend int2 operator+(const int f, const int2 &ob);
	int2 operator-(const int2 &ob) const;
	int2 operator-(const int f);
	friend int2 operator-(const int f, const int2 &ob);
	int2 operator*(const int f);
	friend int2 operator*(const int f, const int2 &ob);
	int2 operator/(int f) const;
	int2 & operator=(const int2 &ob);
	int2 & operator=(int f);
	int2 & operator+=(const int2 &ob);
	int2 & operator+=(int f);
	int2 & operator-=(const int2 &ob);
	int2 & operator-=(int f);
	int2 & operator*=(int f);
	int2 & operator/=(int f);
	bool operator==(const int2 &ob) const;
	bool operator!=(const int2 &ob) const;

};

//
//	int�R�����x�N�g��
//
class int3{
public:
	int x;
	int y;
	int z;

	//�R���X�g���N�^
	int3();
	int3(const int fx, const int fy, const int fz);
	int3(const int f);
	int3(const int3 &ob);
	int3(const double3 &ob);

	//���Z�q
	int3 operator+(const int3 &ob) const;
	int3 operator+(const int f);
	friend int3 operator+(const int f, const int3 &ob);
	int3 operator-(const int3 &ob) const;
	int3 operator-(const int f);
	friend int3 operator-(const int f, const int3 &ob);
	int3 operator*(const int f);
	friend int3 operator*(const int f, const int3 &ob);
	int3 operator/(int f) const;
	int3 & operator=(const int3 &ob);
	int3 & operator=(int f);
	int3 & operator+=(const int3 &ob);
	int3 & operator+=(int f);
	int3 & operator-=(const int3 &ob);
	int3 & operator-=(int f);
	int3 & operator*=(int f);
	int3 & operator/=(int f);
	bool operator==(const int3 &ob) const;
	bool operator!=(const int3 &ob) const;

};

#endif
