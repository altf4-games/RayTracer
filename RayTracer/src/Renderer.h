#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
public:
	Renderer() = default;

	void Render(const Scene& scene, const Camera& camera);
	void OnResize(uint32_t width, uint32_t height);
	std::shared_ptr<Walnut::Image> GetFinalImage() const { return image; }

	uint32_t* GetImageData() const { return imageData; }
	uint32_t GetWidth() const { return width; }
	uint32_t GetHeight() const { return height; }

private:
	struct Hit {
		float hitDistance;
		glm::vec3 worldNormal;
		glm::vec3 worldPosition;
		int objectIndex;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y);
	Hit TraceRay(const Ray& ray);
	Hit ClosestHit(const Ray& ray, float hitDistance ,int objectIndex);
	Hit Miss(const Ray& ray);

	std::shared_ptr<Walnut::Image> image;
	uint32_t* imageData = nullptr;
	uint32_t width, height;
	const Scene* activeScene = nullptr;
	const Camera* activeCamera = nullptr;

};