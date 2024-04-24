#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer() = default;

	void Render();
	void OnResize(uint32_t width, uint32_t height);
	std::shared_ptr<Walnut::Image> GetFinalImage() const { return image; }

	uint32_t* GetImageData() const { return imageData; }
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

private:
	glm::vec4 PerPixel(glm::vec2 coord);

	std::shared_ptr<Walnut::Image> image;
	uint32_t* imageData = nullptr;
	uint32_t m_Width, m_Height;

};