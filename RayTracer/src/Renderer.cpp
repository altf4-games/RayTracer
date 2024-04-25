#include "Renderer.h"
#include "Walnut/Random.h"


static uint32_t ConvertToRGBA(const glm::vec4& color)
{
	uint8_t r = (uint8_t)(color.r * 255.0f);
	uint8_t g = (uint8_t)(color.g * 255.0f);
	uint8_t b = (uint8_t)(color.b * 255.0f);
	uint8_t a = (uint8_t)(color.a * 255.0f);

	return (a << 24) | (b << 16) | (g << 8) | r;
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	Ray ray;
	ray.origin = camera.GetPosition();

	for (uint32_t y = 0; y < image->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < image->GetWidth(); x++)
		{
			ray.direction = camera.GetRayDirections() [ x + y * image->GetWidth() ];

			uint32_t index = x + y * image->GetWidth();
			glm::vec4 color = TraceRay(scene, ray);
			imageData[index] = ConvertToRGBA(color);
		}
	}

	image->SetData(imageData);
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (image)
	{
		if (image->GetWidth() == width && image->GetHeight() == height)
			return;
		image->Resize(width, height);
	}
	else
	{
		image = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] imageData;
	imageData = new uint32_t[width * height];
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	if(scene.Spheres.size() == 0)
		return glm::vec4(0, 0, 0, 1);

	const Sphere* closestSphere = nullptr;
	float hitDistance = std::numeric_limits<float>::max();

	for (const Sphere& sphere : scene.Spheres)
	{
		glm::vec3 origin = ray.origin - sphere.position;

		float a = glm::dot(ray.direction, ray.direction);
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;


		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
			continue;

		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (closestT < hitDistance) 
		{
			closestSphere = &sphere;
			hitDistance = closestT;
		}
	}

	if (!closestSphere)
		return glm::vec4(0, 0, 0, 1);

	glm::vec3 origin = ray.origin - closestSphere->position;
	glm::vec3 hitPoint = origin + ray.direction * hitDistance;
	glm::vec3 normal = glm::normalize(hitPoint);
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float d = glm::max(glm::dot(normal, -lightDir),0.0f);

	glm::vec3 sphereColor = closestSphere->albedo;
	sphereColor *= d;
	return glm::vec4(sphereColor, 1.0f);
}

