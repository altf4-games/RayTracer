#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Material
{
	glm::vec3 albedo{ 1.0f };
	float Roughness = 1.0f;
};

struct Sphere
{
	glm::vec3 position {0.0f,0.0f,0.0f};
	float radius = 0.5f;
	int materialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};