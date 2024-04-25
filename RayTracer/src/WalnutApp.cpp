#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Camera.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: camera(45.0f, 0.1f, 100.0f) 
	{
		scene.Materials.push_back(Material{ {0.5f,0.5f,0.5f},1.0f });
		scene.Materials.push_back(Material{ {0.0f,0.0f,0.0f},0.2f});
		{ 
			Sphere sphere;
			sphere.position = { 0.0f,-0.100f,3.500f };
			sphere.radius = 0.5f;
			sphere.materialIndex = 0;
			scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.position = { 0.0f,-100.480f,0.0f };
			sphere.radius = 100.0f;
			sphere.materialIndex = 1;
			scene.Spheres.push_back(sphere);
		}
	}

	virtual void OnUpdate(float ts) override
	{
		camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render Time: %.2f ms", lastRenderTime);
		ImGui::End();

		ImGui::Begin("Scene");
		if (ImGui::Button("Add Sphere")) {
			scene.Spheres.push_back(Sphere{ {0.0f,0.0f,0.0f},0.5f,0});
		}
		for (int i = 0; i < scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Text("Sphere %d", i);
			ImGui::DragFloat3("Position", &scene.Spheres[i].position.x, 0.1f);
			ImGui::DragFloat("Radius", &scene.Spheres[i].radius, 0.1f);
			ImGui::DragInt("Material Index", &scene.Spheres[i].materialIndex, 1, 0, scene.Materials.size() - 1);
			ImGui::Separator();
			ImGui::PopID();
		}	
		if (ImGui::Button("Add Material")) {
			scene.Materials.push_back(Material{ {0.5f,0.5f,0.5f},0.0f });
		}

		for (int i = 0; i < scene.Materials.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Text("Material %d", i);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(scene.Materials[i].albedo));
			ImGui::DragFloat("Roughness", &scene.Materials[i].Roughness, 0.05f, 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		viewPortWidth = ImGui::GetContentRegionAvail().x;
		viewPortHeight = ImGui::GetContentRegionAvail().y;

		auto image = renderer.GetFinalImage();

		if(image)
			ImGui::Image(image->GetDescriptorSet(), {(float)image->GetWidth(),(float)image->GetHeight()}, ImVec2(0,1),ImVec2(1,0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		renderer.OnResize(viewPortWidth, viewPortHeight);
		camera.OnResize(viewPortWidth, viewPortHeight);
		renderer.Render(scene,camera);

		lastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer renderer;
	Camera camera;
	Scene scene;
	uint32_t viewPortWidth = 0, viewPortHeight = 0;
	float lastRenderTime = 0.0f;
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