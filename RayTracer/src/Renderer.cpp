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

void Renderer::Render()
{
	for (uint32_t y = 0; y < image->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < image->GetWidth(); x++)
		{
			float aspectRatio = (float)image->GetWidth() / (float)image->GetHeight();

			glm::vec2 coord = { (float)x / (float) image->GetWidth(), (float)y / (float) image->GetHeight()};
			coord = coord * 2.0f - 1.0f;
			coord.x *= aspectRatio;
			uint32_t index = x + y * image->GetWidth();
			glm::vec4 color = PerPixel(coord);
			imageData[index] = ConvertToRGBA(color);
		}
	}

	image->SetData(imageData);
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (image)
	{
		if (image->GetWidth() == width || image->GetHeight() == height)
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

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	float radius = 0.5f;
	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);

	float a = glm::dot(rayDirection,rayDirection);
	float b = 2.0f * glm::dot(rayOrigin,rayDirection);
	float c = glm::dot(rayOrigin,rayOrigin) - radius * radius;


	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return glm::vec4(0, 0, 0, 1);
	
	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

	glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
	glm::vec3 normal = glm::normalize(hitPoint);
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

	float d = glm::max(glm::dot(normal, -lightDir),0.0f);

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= d;
	return glm::vec4(sphereColor, 1.0f);
}

