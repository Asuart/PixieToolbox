#include "PixieToolbox.h"

#include <iostream>

#include <PixieRenderer/PixieRenderer.h>
#include <PixieRenderer/RenderGraph/RenderGraph.h>
#include <PixieRenderer/Renderer/IRenderer.h>
#include <PixieRenderer/Window/IWindow.h>

#include <PixieToolboxCore/Config.h>
#include <PixieToolboxCore/Scene/Scene.h>
#include <PixieToolboxCore/Scene/SceneLoader.h>
#include <PixieToolboxCore/Time/ApplicationTime.h>
#include <PixieToolboxCore/Time/GlobalTimer.h>

#include "UI/UI.h"
#include "UI/Windows/ApplicationStatsWindow.h"
#include "UI/Windows/DemoWindow.h"
#include "UI/Windows/TextureDisplayWindow.h"
#include "UI/Windows/SceneTreeWindow.h"

#include "RenderStages/BlurStage.h"
#include "RenderStages/PresentStage.h"
#include "RenderStages/SceneStage.h"

namespace PixieToolbox {

PixieToolboxApp::PixieToolboxApp(const std::string& name, glm::uvec2 resolution, RenderAPI api) {
	Config::Load();

	m_window = IWindow::Create(name, resolution, api);
	m_renderer = m_window->GetRenderer();
	m_ui = UI::Create(m_window.get(), true, api);

	m_textureDisplayWindow = new TextureDisplayWindow(m_ui.get(), m_renderer, TextureHandle());
	m_sceneTreeWindow = new SceneTreeWindow(m_ui.get(), m_renderer);

	m_ui->AddWindow(new DemoWindow(m_ui.get(), m_renderer));
	m_ui->AddWindow(new ApplicationStatsWindow(m_ui.get(), m_renderer));
	m_ui->AddWindow(m_textureDisplayWindow);
	m_ui->AddWindow(m_sceneTreeWindow);


	m_window->SetDropCallback([this](const std::vector<std::string>& files) {
		if (files.empty())
			return;
		m_pendingDropFile = files.front();
		std::cout << "[SceneDrop] Queued: " << m_pendingDropFile << "\n";
	});

	if (!Config::GetLastScenePath().empty()) {
		m_pendingDropFile = Config::GetLastScenePath().string();
	} else {
		m_scene = std::make_shared<Scene>("New Scene");
	}

	UpdateRenderGraph();
}

PixieToolboxApp::~PixieToolboxApp() {
	m_renderer->WaitIdle();
	m_ui.reset();
	m_renderGraph.reset();
	m_window.reset();
}

void PixieToolboxApp::Start() {
	while (!m_window->GetShouldClose()) {
		GlobalTimer::StartTimer("Frame");

		GlobalTimer::StartTimer("PollEvents");
		m_window->PollEvents();
		GlobalTimer::StopTimer("PollEvents");

		if (!m_pendingDropFile.empty()) {
			const std::filesystem::path dropPath = m_pendingDropFile;
			m_pendingDropFile.clear();
			LoadScene(dropPath);
		}

		if (!m_renderer->BeginFrame()) {
			GlobalTimer::StopTimer("Frame");
			continue;
		}

		Time::Update();

		GlobalTimer::StartTimer("UI_BeforeFrame");
		m_ui->OnBeforeDrawFrame();
		GlobalTimer::StopTimer("UI_BeforeFrame");

		GlobalTimer::StartTimer("SceneUpdate");
		m_scene->Update();
		GlobalTimer::StopTimer("SceneUpdate");

		GlobalTimer::StartTimer("RenderGraph");
		m_renderGraph->Execute();
		GlobalTimer::StopTimer("RenderGraph");

		GlobalTimer::StartTimer("UI_Draw");
		m_ui->Draw();
		GlobalTimer::StopTimer("UI_Draw");

		m_renderer->EndFrame();

		GlobalTimer::StopTimer("Frame");
	}
}

void PixieToolboxApp::LoadScene(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
		std::cerr << "[SceneDrop] Not a file: " << path << "\n";
		return;
	}

	m_renderer->WaitIdle();

	std::cout << "[SceneDrop] Loading scene: " << path << "\n";
	m_scene = SceneLoader::LoadScene(path.string(), m_renderer);

	if (m_sceneStage) {
		m_sceneStage->SetScene(m_scene);
	}

	if (m_sceneTreeWindow) {
		m_sceneTreeWindow->SetScene(m_scene);
	}

	Config::SetLastScenePath(path);
	Config::Save();
}

void PixieToolboxApp::UpdateRenderGraph() {
	m_renderGraph = std::make_unique<RenderGraph>(m_renderer);

	glm::uvec2 renderResolution = m_window->GetResolution();

	RenderTargetDesc rtDesc;
	rtDesc.format = TextureFormat::RGBA32f;
	rtDesc.size = renderResolution;
	rtDesc.hasDepth = true;
	rtDesc.finalUsage = ResourceUsage::Sampled;

	RGResource sceneColor = m_renderGraph->RegisterRenderTarget("SceneColor", rtDesc);

	TextureDesc blurDesc;
	blurDesc.format = TextureFormat::RGBA32f;
	blurDesc.size = renderResolution;
	blurDesc.mipLevels = 1;
	blurDesc.storageImage = true;

	RGResource blurColor = m_renderGraph->RegisterTexture("BlurColor", blurDesc);

	std::unique_ptr<SceneStage> sceneStage = std::make_unique<SceneStage>(m_renderer, sceneColor, renderResolution);
	m_sceneStage = sceneStage.get();

	m_renderGraph->AddStage(std::move(sceneStage));
	m_renderGraph->AddStage(std::make_unique<BlurStage>(m_renderer, sceneColor, blurColor, renderResolution));
	m_renderGraph->AddStage(std::make_unique<PresentStage>(m_renderer, blurColor));

	m_renderGraph->Compile();

	m_textureDisplayWindow->SetTexture(m_renderGraph->GetResource(blurColor).texture);
}

} // namespace PixieToolbox
