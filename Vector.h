#pragma once
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>



// Classe d�finissant un vecteur

class Vector
{

public:

	double x; // coordonn�e x (en m�tres)
	double y; // coordonn�e y (en m�tres)
	double z; // coordonn�e z (en m�tres)

	Vector();
	Vector(const Vector& vector);
	Vector(const double& x, const double& y, const double& z);

	void operator=(const Vector& vector);
	void operator+=(const Vector& vector);
	void operator-=(const Vector& vector);
	void operator*=(const double& number);
	void operator/=(const double& number);

	double get_radius();
	double get_phi();
	double get_theta();
};

Vector create_spherical(const double& radius, const double& phi, const double& theta);
Vector operator+(const Vector& vector_1, const Vector& vector_2);
Vector operator-(const Vector& vector_1, const Vector& vector_2);
Vector operator*(const Vector& vector, const double& number);
Vector operator*(const double& number, const Vector& vector);
double operator*(const Vector& vector_1, const Vector& vector_2);
Vector operator^(const Vector& vector_1, const Vector& vector_2);
Vector operator/(const Vector& vector, const double& number);
bool operator==(const Vector& vector_1, const Vector& vector_2);
bool operator!=(const Vector& vector_1, const Vector& vector_2);
double get_x(const double& radius, const double& phi, const double& theta);
double get_y(const double& radius, const double& phi, const double& theta);
double get_z(const double& radius, const double& phi, const double& theta);
double get_distance(const Vector& point_1, const Vector& point_2);
double get_phi(const Vector& point_1, const Vector& point_2);
double get_theta(const Vector& point_1, const Vector& point_2);
