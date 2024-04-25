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
	activeScene = &scene;
	activeCamera = &camera;

	for (uint32_t y = 0; y < image->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < image->GetWidth(); x++)
		{
			uint32_t index = x + y * image->GetWidth();
			glm::vec4 color = PerPixel(x,y);
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

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.origin = activeCamera->GetPosition();
	ray.direction = activeCamera->GetRayDirections()[x + y * image->GetWidth()];

	glm::vec3 color = glm::vec3(0.0f);

	int bounces = 2;
	float mul = 1.0f;

	for (int i = 0; i < bounces; i++) 
	{

		Renderer::Hit hit = TraceRay(ray);
		if (hit.hitDistance < 0) 
		{
			glm::vec3 skyBox = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skyBox * mul;
			break;
		}

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
		float d = glm::max(glm::dot(hit.worldNormal, -lightDir), 0.0f);

		const Material& mat = activeScene->Materials[activeScene->Spheres[hit.objectIndex].materialIndex];

		glm::vec3 sphereColor = mat.albedo;
		sphereColor *= d;
		color += sphereColor * mul;
		mul *= 0.5f;

		ray.origin = hit.worldPosition + hit.worldNormal * 0.0001f;
		ray.direction = glm::reflect(ray.direction, hit.worldNormal + mat.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
	}

	return glm::vec4(color, 1.0f);
}

Renderer::Hit Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::Hit hit;
	hit.hitDistance = hitDistance;
	hit.objectIndex = objectIndex;

	const Sphere& closestSphere = activeScene->Spheres[objectIndex];

	glm::vec3 origin = ray.origin - closestSphere.position;
	hit.worldPosition = origin + ray.direction * hitDistance;
	hit.worldNormal = glm::normalize(hit.worldPosition);

	hit.worldPosition += closestSphere.position;


	return hit;
}

Renderer::Hit Renderer::Miss(const Ray& ray)
{
	Renderer::Hit hit;
	hit.hitDistance = -1;
	return hit;
}

Renderer::Hit Renderer::TraceRay(const Ray& ray)
{
	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();

	for (int i=0; i<activeScene->Spheres.size(); i++)
	{
		const Sphere& sphere = activeScene->Spheres[i];
		glm::vec3 origin = ray.origin - sphere.position;

		float a = glm::dot(ray.direction, ray.direction);
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;


		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
			continue;

		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (closestT > 0 && closestT < hitDistance) 
		{
			closestSphere = i;
			hitDistance = closestT;
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);
}

