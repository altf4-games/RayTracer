#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"

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
		ImGui::Text("Last Render Time: %.2f ms", lastRenderTime);
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
	}

	void Render()
	{
		Timer timer;

		renderer.OnResize(viewPortWidth, viewPortHeight);
		renderer.Render();

		lastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer renderer;
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