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
struct HalfSpace
{
	HalfSpace(glm::vec3 n, glm::vec3 p)
	{
		normal = n;
		point = p;
		float max_f = (float)INFINITY;
		HalfSpaceInterval = glm::vec2(0.0f, (float)max_f);	//by default it is this value
	}
	glm::vec3 normal;
	glm::vec3 point;
	glm::vec2 HalfSpaceInterval;
};
struct BoxObject
{
	BoxObject(glm::vec3 c, glm::vec3 l, glm::vec3 w, glm::vec3 h, glm::vec3 mat_diff)
	{
		mCornerPosition = c;
		mLegth = l;
		mWidth = w;
		mHeight = h;
		mMaterialDiffuse = mat_diff;
		//generate Halfspaces
		//FRONT
		glm::vec3 c1 = c;
		glm::vec3 n1 = glm::normalize(glm::cross(l, h));
		BoxPlanes.push_back(HalfSpace(n1, c1));
		//BACK
		glm::vec3 c2 = c + w;
		glm::vec3 n2 = -n1;
		BoxPlanes.push_back(HalfSpace(n2, c2));

		//LEFT
		glm::vec3 c3 = c;
		glm::vec3 n3 = glm::normalize(glm::cross(h, w));
		BoxPlanes.push_back(HalfSpace(n3, c3));

		//RIGHT
		glm::vec3 c4 = c + l;
		glm::vec3 n4 = -n3;
		BoxPlanes.push_back(HalfSpace(n4, c4));

		//BOTTOM
		glm::vec3 c5 = c;
		glm::vec3 n5 = glm::normalize(glm::cross(w, l));
		BoxPlanes.push_back(HalfSpace(n5, c5));

		//TOP
		glm::vec3 c6 = c + h;
		glm::vec3 n6 = -n5;
		BoxPlanes.push_back(HalfSpace(n6, c6));

	}
	~BoxObject()
	{

	}
	glm::vec3 GetCorner()
	{
		return mCornerPosition;
	}
	glm::vec3 GetLength()
	{
		return mLegth;
	}
	glm::vec3 GetWidth()
	{
		return mWidth;
	}
	glm::vec3 GetHeight()
	{
		return mHeight;
	}
	glm::vec3 GetMaterialDiffuse()
	{
		return mMaterialDiffuse;
	}
	// No intersection interval is [0, -1], which is invalid interval
	void GetRayPlaneIntersectionInterval(Ray r, HalfSpace plane)
	{
		glm::vec2 Interval(0.0f, INFINITY);
		if (glm::dot(r.v, plane.normal) < 0)
		{
			if (glm::dot((r.P - plane.point), plane.normal) > 0)
			{
				float t = -((glm::dot((r.P - plane.point), plane.normal)) / (glm::dot(r.v, plane.normal)));
				Interval = glm::vec2(t, INFINITY);
			}
			else
			{
				Interval = glm::vec2(0.0f, INFINITY);
			}
		}
		else if (glm::dot(r.v, plane.normal) < 0)
		{
			if (glm::dot((r.P - plane.point), plane.normal) < 0)
			{
				float t = -((glm::dot((r.P - plane.point), plane.normal)) / (glm::dot(r.v, plane.normal)));
				Interval = glm::vec2(0.0f, t);
			}
			else
			{
				Interval = glm::vec2(0, -1);
			}
		}
		else
		{
			Interval = glm::vec2(0, -1);
		}
		//set the interval of the plane to that of the interval
		plane.HalfSpaceInterval = Interval;
	}
	float IntersectWithRay(Ray r)
	{
		//loop through halfspaces and set the intervals
		for (int i = 0; i < BoxPlanes.size(); i++)
		{
			GetRayPlaneIntersectionInterval(r, BoxPlanes[i]);
		}
		glm::vec2 CurrentInterval(0.0f, INFINITY);
		//iterate through and modify Current Interval
		for (int i = 0; i < BoxPlanes.size(); i++)
		{
			//skip over invalid intervals
			if (BoxPlanes[i].HalfSpaceInterval == glm::vec2(0, -1))
			{
				continue;
			}
			//max of the min
			float l = std::max(CurrentInterval.x, BoxPlanes[i].HalfSpaceInterval.x);
			//min of the max
			float r = std::min(CurrentInterval.y, BoxPlanes[i].HalfSpaceInterval.y);
			CurrentInterval.x = l;
			CurrentInterval.y = r;
		}
		//check if the interval is valid
		//in interval [x, y], if y =< x, it is invalid
		if (CurrentInterval.x >= CurrentInterval.y)
		{
			//return default value of -1
			return -1.0f;
		}
		//otherise, we return the x value
		return CurrentInterval.x;
	}
	glm::vec3 GetNormalOfIntersection(glm::vec3 pi, float epsilon_range)
	{
		float current_d;
		//iterate through the halspaces
		for (int i = 0; i < BoxPlanes.size(); i++)
		{
			glm::vec3 vector_sub = pi - BoxPlanes[i].point;
			current_d = glm::dot(BoxPlanes[i].normal, vector_sub);
			//check if distance is within the range
			if (abs(current_d) <= epsilon_range)
			{
				return BoxPlanes[i].normal;
			}
		}
		//default should return 0 normal, but this is wrong, we shoulndt get here
		return BoxPlanes[0].normal;
	}
	void SetMaterial(Material m)
	{
		mMaterial = m;
	}
	glm::vec3 mCornerPosition;
	glm::vec3 mLegth;
	glm::vec3 mWidth;
	glm::vec3 mHeight;
	glm::vec3 mMaterialDiffuse;
	std::vector<HalfSpace> BoxPlanes;
	Material mMaterial;
};