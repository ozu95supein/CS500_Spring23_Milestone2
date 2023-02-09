#pragma once
#include "Primitives.h"
#include "Materials.h"

struct SphereObject
{
	Sphere s;
	glm::vec3 color;
	Material m;
	SphereObject()
	{
		s = Sphere(glm::vec3(0,0,0), 1);
		color = glm::vec3(0,0,0);
		m = E_DIFFUSE;
	}
	SphereObject(glm::vec3 inputSphereCenter, glm::vec3 inputColor, float inputRadiusFloat, Material mat)
	{
		s = Sphere(inputSphereCenter, inputRadiusFloat);
		color = inputColor;
		m = mat;
	}
};
struct CameraObject
{
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;
	float focal_length;
	void SetCPosition(glm::vec3 & pos)
	{
		position = pos;
	}
	void SetTPosition(glm::vec3& t)
	{
		target = t;
	}
	void SetUpVector(glm::vec3& u)
	{
		up = u;
	}
	void SetFocalLength(float& f)
	{
		focal_length = f;
	}
};
enum RAY_HIT_TYPE
{
	E_NO_HIT = 0,
	E_SPHERE_HIT = 1,
	E_BOX_HIT = 2,
	E_LIGHT_HIT = 3
};
float Ray_VS_Sphere(Ray & r, Sphere s)
{
	//t values to compare which one is the closest
	float current_t = INFINITY;
	//return black by default
	glm::vec3 nearest_sphere_color = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 CP = (r.P - s.GetCenter()); //Sphere to Cam vector
	float sphere_radius = s.GetRadius();                     //radius of sphere
	//gain discriminant
	auto a = glm::dot(r.v, r.v);
	auto b = 2.0f * (glm::dot(r.v, CP));
	auto c = glm::dot(CP, CP) - (sphere_radius * sphere_radius);
	float discriminant = b * b - (4.0f * a * c);
	if (discriminant < 0)
	{
		//return a negative number for no intersection
		return -1.0f;
	}
	else if (discriminant > 0)
	{
		//2 intercetions, take the smaller one (the one witht he subtraction)
		current_t = ((-2.0f * (glm::dot(r.v, CP))) - glm::sqrt(discriminant)) / (2 * (glm::dot(r.v, r.v)));
		//TODO more cases to account for
		if (current_t < 0.0f)
		{
			current_t = ((-2.0f * (glm::dot(r.v, CP))) + glm::sqrt(discriminant)) / (2 * (glm::dot(r.v, r.v)));
		}
		return current_t;
	}
	else if (discriminant == 0)
	{
		current_t = -(glm::dot(r.v, CP)) / (glm::dot(r.v, r.v));
		return current_t;
	}
	return -1.0f;
	return 1;
}