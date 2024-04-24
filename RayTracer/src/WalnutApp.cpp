#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::Begin("Viewport");
		viewPortWidth = ImGui::GetContentRegionAvail().x;
		viewPortHeight = ImGui::GetContentRegionAvail().y;

		if(image)
			ImGui::Image(image->GetDescriptorSet(), {(float)image->GetWidth(),(float)image->GetHeight()});

		ImGui::End();
	}

	void Render()
	{
		if (!image || viewPortWidth != image->GetWidth() || viewPortHeight != image->GetHeight())
		{
			image = std::make_shared<Image>(viewPortWidth, viewPortHeight,ImageFormat::RGBA);
			delete[] imageData;
			imageData = new uint32_t[viewPortWidth * viewPortHeight];
		}

		for (uint32_t i = 0; i < viewPortWidth * viewPortHeight; i++)
		{
			imageData[i] = 0xFFFFFFFF;
		}

		image->SetData(imageData);
	}

private:
	std::shared_ptr<Image> image;
	uint32_t* imageData = nullptr;
	uint32_t viewPortWidth = 0;
	uint32_t viewPortHeight = 0;
};

Walnut::Application *Walnut::CreateApplication(int argc, char **argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "RayTracer";

	Walnut::Application *app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
							{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		} });
	return app;
}