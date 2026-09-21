#include "PixieToolbox.h"

#include <iostream>
#include <memory>

#include <PixieRenderer/PixieRenderer.h>
#include <PixieRenderer/RenderGraph/RenderGraph.h>
#include <PixieRenderer/Renderer/IRenderer.h>
#include <PixieRenderer/Window/IWindow.h>

#include <PixieToolboxCore/Config.h>
#include <PixieToolboxCore/Scene/Scene.h>
#include <PixieToolboxCore/Scene/SceneLoader.h>
#include <PixieToolboxCore/Time/ApplicationTime.h>

#include "UI/UI.h"
#include "UI/Windows/ApplicationStatsWindow.h"
#include "UI/Windows/DemoWindow.h"
#include "UI/Windows/TextureDisplayWindow.h"

#include "RenderStages/BlurStage.h"
#include "RenderStages/PresentStage.h"
#include "RenderStages/SceneStage.h"

namespace PixieToolbox {

PixieToolboxApp::PixieToolboxApp(const std::string& name, glm::uvec2 resolution, RenderAPI api) {
	Config::Load();

	m_window = IWindow::Create(name, resolution, api);
	m_renderer = m_window->GetRenderer();
	m_ui = UI::Create(m_window.get(), true, api);

	m_ui->AddWindow(new DemoWindow(m_ui.get(), m_renderer));
	m_ui->AddWindow(new ApplicationStatsWindow(m_ui.get(), m_renderer));
	m_ui->AddWindow(new TextureDisplayWindow(m_ui.get(), m_renderer, TextureHandle()));

	m_window->SetDropCallback([this](const std::vector<std::string>& files) {
		if (files.empty())
			return;
		m_pendingDropFile = files.front();
		std::cout << "[SceneDrop] Queued: " << m_pendingDropFile << "\n";
	});

	m_scene = std::make_shared<Scene>("new Scene");

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
		m_window->PollEvents();

		if (!m_pendingDropFile.empty()) {
			const std::filesystem::path dropPath = m_pendingDropFile;
			m_pendingDropFile.clear();
			LoadScene(dropPath);
		}

		if (!m_renderer->BeginFrame()) {
			continue;
		}

		Time::Update();

		m_ui->OnBeforeDrawFrame();

		m_scene->Update();

		m_renderGraph->Execute();

		m_ui->Draw();

		m_renderer->EndFrame();
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

	UpdateRenderGraph();

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

	m_renderGraph->AddStage(std::make_unique<SceneStage>(m_renderer, sceneColor, renderResolution));
	m_renderGraph->AddStage(std::make_unique<BlurStage>(m_renderer,  sceneColor, blurColor, renderResolution));
	m_renderGraph->AddStage(std::make_unique<PresentStage>(m_renderer, blurColor));

	m_renderGraph->Compile();

	// m_ui->AddWindow(new TextureDisplayWindow(m_ui.get(), m_renderer, m_renderGraph->GetResource(blurColor).texture));
}

} // namespace PixieToolbox
