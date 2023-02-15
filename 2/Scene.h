#pragma once
#include <vector>
#include "RaycastingObjects.h"
float myRand_0_to_1()
{
    float r = rand() / RAND_MAX;
    return r;
}
float myRand_neg1_to1()
{
    float r = myRand_0_to_1();
    r = 2.0f * r;
    r -= -1.0f;
    return r;
}
glm::vec3 myRand_vec3()
{
    float theta = myRand_0_to_1() * 2.0f * PI;
    float phi = acos(myRand_neg1_to1());
    float x = sin(phi) * cos(theta);
    float y = cos(phi) * sin(theta);
    float z = cos(phi);

    glm::vec3 v = glm::vec3(x, y, z);
    return v;
}
struct SceneStruct
{
	std::vector<SphereObject> mSphereObjs;
    std::vector<BoxObject> mSceneBoxes;
    std::vector<SphereObject> mSceneLights;

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
    //make floats for t values for current t, the value we calculate, and values for smallest 
    //  t sofar, which are the smallest, non-negative, t values we keep track of
    float smallest_t = INFINITY;
    float current_sphere_t = INFINITY;
    float current_box_t = INFINITY;
    float current_light_t = INFINITY;

    std::vector<SphereObject>::iterator nearest_sphere_obj_it;
    std::vector<BoxObject>::iterator nearest_box_obj_it;
    std::vector<SphereObject>::iterator nearest_light_obj_it;

    RAY_HIT_TYPE e_hit_type = RAY_HIT_TYPE::E_NO_HIT;
    Material e_material_type = Material::E_DIFFUSE;
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
        Sphere s = it->s;
        current_sphere_t = Ray_VS_Sphere(r, s);
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

    //iterate of the box in the scene, get current t, compare with smallest t, keep trak of nearest object and there t
    for (std::vector<BoxObject>::iterator it = scene.mSceneBoxes.begin(); it != scene.mSceneBoxes.end(); ++it)
    {
        current_box_t = it->IntersectWithRay(r);
        if (current_box_t <= 0)
        {
            continue;
        }
        //check if t is smaller
        if (current_box_t < smallest_t)
        {
            smallest_t = current_box_t;
            e_hit_type = RAY_HIT_TYPE::E_BOX_HIT;
            nearest_box_obj_it = it;
        }
    }

    //iterate of the light in the scene, get current t, compare with smallest t, keep trak of nearest object and there t
    for (std::vector<SphereObject>::iterator it = scene.mSceneLights.begin(); it != scene.mSceneLights.end(); ++it)
    {
        Sphere l = it->s;
        current_light_t = Ray_VS_Sphere(r, l);
        if (current_light_t < 0)
        {
            continue;
        }
        //check if t is smaller
        if (current_light_t < smallest_t)
        {
            smallest_t = current_light_t;
            e_hit_type = RAY_HIT_TYPE::E_LIGHT_HIT;
            nearest_light_obj_it = it;
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
        e_material_type = nearest_sphere_obj_it->m;
    }
    else if (e_hit_type == RAY_HIT_TYPE::E_BOX_HIT)
    {
        e_material_type = nearest_box_obj_it->mMaterial;
    }

    //if remaining bounces are == 0
    if (maxBounces == 0)
    {
        //  if intersecting object is a light we return lightcolor matdiffuse
        if (e_hit_type == RAY_HIT_TYPE::E_LIGHT_HIT)
        {
            return nearest_light_obj_it->color;
        }
        //  else if intersecting object is not a light we return ambient
        else
        {
            if (use_default_amb)
            {
                return glm::vec3(0.2, 0.2, 0.2);
            }
            return scene.SceneAmbient;
        }
    }
    else //if we still have bounces
    {
        //  if closest object is a light
        if (e_hit_type == RAY_HIT_TYPE::E_LIGHT_HIT)
        {
            return nearest_light_obj_it->color;
        }
        else
        {
            if (e_hit_type == RAY_HIT_TYPE::E_SPHERE_HIT)
            {
                if (e_material_type == Material::E_DIFFUSE)
                {
                    //generate a new random ray, nr, with the point of intersection, the normal, and call this function again recursively like this:
                    //get point of intersection, NOTE WE NEED TO OFFSET THIS
                    glm::vec3 pi = r.FindPointAt_t(smallest_t);
                    //calculate the normal of the sphere
                    //  get center of sphere and pi, make a vector and normalize

                    glm::vec3 sphere_normal = pi - nearest_sphere_obj_it->s.GetCenter();
                    glm::vec3 dirtolight = glm::vec3(0.0, 2.0, 0.0) - pi;

                    sphere_normal = glm::normalize(sphere_normal);
                    //make offseted point
                    glm::vec3 p_offset = pi + MY_EPSILON * sphere_normal;
                    glm::vec3 rand_dir = myRand_vec3();
                    glm::vec3 dir = rand_dir + sphere_normal;
                    dir = glm::normalize(dir);
                    Ray nr(p_offset, dir);

                    int b = maxBounces - 1;
                    
                    return nearest_sphere_obj_it->color * ThrowRayRecursiveBounce(r, scene, use_default_amb, b);
                }
                else if (e_material_type == Material::E_METALLIC)
                {
                    
                }

            }
            else if (e_hit_type == RAY_HIT_TYPE::E_BOX_HIT)
            {
                if (e_material_type == Material::E_DIFFUSE)
                {
                    // //generate a new random ray, nr, with the point of intersection, the normal, and call this function again recursively like this:
                    // //get point of intersection, NOTE WE NEED TO OFFSET THIS
                    // glm::vec3 pi = r.RayOrigin_p + (r.direction_v * smallest_t);
                    // glm::vec3 box_normal =  nearest_box_obj_it->GetNormalOfIntersection(pi, MY_EPSILON);
                    // //make offseted point
                    // glm::vec3 p_offset = pi + MY_EPSILON * box_normal;
                    // glm::vec3 dir = myRand_vec3();
                    // Ray nr(p_offset, dir);
                    // int b = RemainingBounces - 1;
                    // return nearest_box_obj_it->GetMaterialDiffuse() * CastRayRecursiveBounce(scene, nr, b, use_default_amb);
                    //return nearest_box_obj_it->GetMaterialDiffuse();

                    int b = maxBounces - 1;
                    return nearest_box_obj_it->GetMaterialDiffuse() * ThrowRayRecursiveBounce(r, scene, use_default_amb, b);
                }
                else if (e_material_type == Material::E_METALLIC)
                {
                    if (use_default_amb)
                    {
                        return glm::vec3(0.2, 0.2, 0.2);
                    }
                    return scene.SceneAmbient;
                }

            }
        }

    }
    if (use_default_amb)
    {
        return glm::vec3(0.2, 0.2, 0.2);
    }
    return scene.SceneAmbient;
}
