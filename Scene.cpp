//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0f, 1.0f);

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    if (depth > 12)
    {
        return Vector3f(0);
    }

    Intersection intersection = Scene::intersect(ray);

    if (!intersection.happened)
    {
        return Vector3f(0);
    }

    if (intersection.m->hasEmission())
    {
        return intersection.m->getEmission();
    }

    // do both light sampling and indirect illumination sampling
    // do light sampling
    Vector3f l_dir = Vector3f(0);
    Intersection sample_light;
    float pdf_light;
    sampleLight(sample_light, pdf_light);
    Vector3f p_to_light = sample_light.coords - intersection.coords;
    Vector3f wi = normalize(p_to_light);

    // ADD LIGHT VISIBILITY CHECK HERE
    if (dotProduct(wi, intersection.normal) > 0) {  // Light is visible from this side of surface
        Vector3f offset_coord = intersection.coords + intersection.normal * 0.001f;

        Intersection block_check = Scene::intersect(Ray(offset_coord, wi, 0.0f));

        Vector3f a = block_check.coords - intersection.coords;
        Vector3f b = sample_light.coords - intersection.coords;
        bool blocked = block_check.happened && dotProduct(a, a) < dotProduct(b, b) + 1e-4f;
        
        if (!blocked)
        {
            l_dir = sample_light.emit * 
                        intersection.m->eval(wi, -ray.direction, intersection.normal) * 
                        dotProduct(wi, intersection.normal) * 
                        dotProduct(-wi, sample_light.normal) / 
                        (dotProduct(p_to_light, p_to_light) * pdf_light);
        }
    }
    // If dotProduct(wi, intersection.normal) <= 0, l_dir remains Vector3f(0)

    // do indirect illumination sampling
    float random_float = dis(gen);
    float russian_pdf = 0.8;
    
    if (random_float > russian_pdf)
    {
        return l_dir;
    }

    Vector3f sample_dir = intersection.m->sample(wi, intersection.normal);
    Vector3f offset_p = intersection.coords + intersection.normal * 0.001f;

    Vector3f l_indir = castRay(Ray(offset_p, sample_dir, 0.0f), depth+1) *
                       intersection.m->eval(sample_dir, -ray.direction, intersection.normal) *
                       dotProduct(sample_dir, intersection.normal) /
                       (intersection.m->pdf(sample_dir, -ray.direction, intersection.normal) * russian_pdf);

    Vector3f sum = l_dir + l_indir;
    Vector3f result (std::min(sum.x, 0.3f), std::min(sum.y, 0.3f), std::min(sum.z, 0.3f));

    return result;
}