#pragma once
#include <vector>
#include "RaycastingObjects.h"

struct SceneStruct
{
	std::vector<SphereObject> mSphereObjs;
	CameraObject mSceneCamera;
	glm::vec3 SceneAmbient;
};
// NOTE: if the scene has an ambient color, we dont need to use the default ambient
glm::vec3 ThrowRay(Ray& r, SceneStruct& scene, bool use_default_amb)
{
	float smallest_t = INFINITY;
	float current_sphere_t = INFINITY;
	float current_box_t = INFINITY;
	float current_light_t = INFINITY;
	std::vector<SphereObject>::iterator nearest_sphere_obj_it;
	RAY_HIT_TYPE e_hit_type = RAY_HIT_TYPE::E_NO_HIT;
	Material mat = Material::E_DIFFUSE;
       // integer to track nearest object
    // (-1 == default option, not set)
    // (0 == NO INTERSECT) AKA all t values are -1
    // (1 == Sphere)
    // (2 == Box)
    // (3 == Light)
    int nearest_object = -1;
    //iterate of the spheres in the scene, get current t, compare with smallest t, keep trak of nearest object and there t
    for (std::vector<SphereObject>::iterator it = scene.mSphereObjs.begin(); it != scene.mSphereObjs.end(); ++it)
    {
        current_sphere_t = Ray_VS_Sphere(r, (it->s));
        if (current_sphere_t < 0)
        {
            continue;
        }
        //check if t is smaller
        if (current_sphere_t < smallest_t)
        {
            smallest_t = current_sphere_t;
            e_hit_type = RAY_HIT_TYPE::E_SPHERE_HIT;
            nearest_sphere_obj_it = it;
        }
    }
    //Sanity check for checking if absolutely nothing intersected with ray
    if (smallest_t < 0.0f)
    {
        //NO INTERSECTION, return ambient lighting, which is usually set to 0,0,0
        e_hit_type = RAY_HIT_TYPE::E_NO_HIT;
        if (use_default_amb)
        {
            return glm::vec3(0.2, 0.2, 0.2);
        }
        return scene.SceneAmbient;
    }
    //set the material type for the object depending on what hit
    if (e_hit_type == RAY_HIT_TYPE::E_SPHERE_HIT)
    {
        mat = nearest_sphere_obj_it->m;
        if (mat == Material::E_DIFFUSE)
        {
            return nearest_sphere_obj_it->color;
        }
    }
    if (use_default_amb)
    {
        return glm::vec3(0.2, 0.2, 0.2);
    }
    return scene.SceneAmbient;
}

glm::vec3 ThrowRayRecursiveBounce(Ray& r, SceneStruct& scene, bool use_default_amb, unsigned int maxBounces)
{
    float smallest_t = INFINITY;
    float current_sphere_t = INFINITY;
    float current_box_t = INFINITY;
    float current_light_t = INFINITY;
    std::vector<SphereObject>::iterator nearest_sphere_obj_it;
    RAY_HIT_TYPE e_hit_type = RAY_HIT_TYPE::E_NO_HIT;
    Material mat = Material::E_DIFFUSE;
    // integer to track nearest object
    // (-1 == default option, not set)
    // (0 == NO INTERSECT) AKA all t values are -1
    // (1 == Sphere)
    // (2 == Box)
    // (3 == Light)
    int nearest_object = -1;
    //iterate of the spheres in the scene, get current t, compare with smallest t, keep trak of nearest object and there t
    for (std::vector<SphereObject>::iterator it = scene.mSphereObjs.begin(); it != scene.mSphereObjs.end(); ++it)
    {
        current_sphere_t = Ray_VS_Sphere(r, (it->s));
        if (current_sphere_t < 0)
        {
            continue;
        }
        //check if t is smaller
        if (current_sphere_t < smallest_t)
        {
            smallest_t = current_sphere_t;
            e_hit_type = RAY_HIT_TYPE::E_SPHERE_HIT;
            nearest_sphere_obj_it = it;
        }
    }
    //Sanity check for checking if absolutely nothing intersected with ray
    if (smallest_t < 0.0f)
    {
        //NO INTERSECTION, return ambient lighting, which is usually set to 0,0,0
        e_hit_type = RAY_HIT_TYPE::E_NO_HIT;
        if (use_default_amb)
        {
            return glm::vec3(0.2, 0.2, 0.2);
        }
        return scene.SceneAmbient;
    }
    //set the material type for the object depending on what hit
    if (e_hit_type == RAY_HIT_TYPE::E_SPHERE_HIT)
    {
        mat = nearest_sphere_obj_it->m;
        if (mat == Material::E_DIFFUSE)
        {
            return nearest_sphere_obj_it->color;
        }
    }
    if (use_default_amb)
    {
        return glm::vec3(0.2, 0.2, 0.2);
    }
    return scene.SceneAmbient;
}
