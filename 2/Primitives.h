#pragma once
#include "glm/glm.hpp"
//sphere primitive
struct Sphere
{
	glm::vec4 center_rad;	//vec4 of the sphere, x,y,z will be the center pos, w will be the radius
	Sphere()
	{
		center_rad = glm::vec4(0, 0, 0, 1);
	}
	Sphere(glm::vec4 input): center_rad(input)
	{
		center_rad = input;
	}
	Sphere(glm::vec3 c, float r)
	{
		center_rad = glm::vec4(c, r);
	}
	glm::vec3 GetCenter()
	{
		return glm::vec3(center_rad.x, center_rad.y, center_rad.z);
	}
	float GetRadius()
	{
		return center_rad.w;
	}
};
//ray primitive
struct Ray
{
	glm::vec3 P;	//Origin Point
	glm::vec3 v;	//direction vector
	Ray(glm::vec3 origin, glm::vec3 dir)
	{
		P = origin;
		v = dir;
	}
	glm::vec3 FindPointAt_t(float t)
	{
		return (P + (t*v));
	}
};

