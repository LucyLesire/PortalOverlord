#include "stdafx.h"
#include "MainMenuScene.h"

#include "PortalScene.h"
#include "Prefabs/CubePrefab.h"

void MainMenuScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.enableOnGUI = false;
	m_SceneContext.settings.showInfoOverlay = false;

	//Camera
	auto pCameraObject = new FixedCamera();
	AddChild(pCameraObject);
	auto pCameraComp = new CameraComponent();
	pCameraObject->AddComponent(pCameraComp);
	SetActiveCamera(pCameraComp);

	m_pSprite = new GameObject();
	m_pSprite->AddComponent(new SpriteComponent(L"Textures/MainMenuPortal.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,0.1f }));
	AddChild(m_pSprite);

	m_pSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .9f);
	m_pSprite->GetTransform()->Scale(1.f, 1.f, 1.f);

	const auto pMaterial = PxGetPhysics().createMaterial(.5f, .5f, .5f);

	auto pStartButton = AddChild(new CubePrefab(XMFLOAT3{ 1.f, 0.125f, 0.5f }, XMFLOAT4{ Colors::Purple }));


	auto pCol = pStartButton->AddComponent(new RigidBodyComponent(true));
	pCol->AddCollider(PxBoxGeometry{ 1.f, 0.25f, 0.5f }, *pMaterial, true);
	pStartButton->GetTransform()->Translate(GetActiveCamera()->GetTransform()->GetPosition().x - 1.25f, GetActiveCamera()->GetTransform()->GetPosition().y, GetActiveCamera()->GetTransform()->GetPosition().z + 5.f);
	/*pStartButton->GetTransform()->Scale(0.28125f, 0.1388888888f, 1.f);*/
	pStartButton->SetTag(L"StartButton");

	auto pControlsButton = AddChild(new CubePrefab(XMFLOAT3{ 1.f, 0.125f, 0.5f }, XMFLOAT4{ Colors::Purple }));

	auto pColControls = pControlsButton->AddComponent(new RigidBodyComponent(true));
	pColControls->AddCollider(PxBoxGeometry{ 1.f, 0.25f, 0.5f }, *pMaterial, true);
	pControlsButton->GetTransform()->Translate(GetActiveCamera()->GetTransform()->GetPosition().x + 1.25f, GetActiveCamera()->GetTransform()->GetPosition().y, GetActiveCamera()->GetTransform()->GetPosition().z + 5.f);
	pControlsButton->SetTag(L"ControlsButton");

	auto pQuitButton = AddChild(new CubePrefab(XMFLOAT3{ 1.f, 0.125f, 0.5f }, XMFLOAT4{ Colors::Purple }));
	auto pColQuit = pQuitButton->AddComponent(new RigidBodyComponent(true));
	pColQuit->AddCollider(PxBoxGeometry{ 1.f, 0.25f, 0.5f }, *pMaterial, true);
	pQuitButton->GetTransform()->Translate(GetActiveCamera()->GetTransform()->GetPosition().x, GetActiveCamera()->GetTransform()->GetPosition().y - 1.f, GetActiveCamera()->GetTransform()->GetPosition().z + 5.f);
	pQuitButton->SetTag(L"QuitButton");

		
}

void MainMenuScene::Update()
{
	if (InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON))
	{
		if (const auto pPickedObject = m_SceneContext.pCamera->Pick())
		{
			if(pPickedObject->GetTag() == L"QuitButton")
			{
				PostQuitMessage(0);
			}

			if(pPickedObject->GetTag() == L"StartButton")
			{
				SceneManager::Get()->AddGameScene(new PortalScene());
				SceneManager::Get()->SetActiveGameScene(L"Portal Scene");
			}

			if (pPickedObject->GetTag() == L"ControlsButton")
			{
				SceneManager::Get()->SetActiveGameScene(L"Controls Scene");
			}
		}
	}
}

void MainMenuScene::OnSceneActivated()
{
	auto portalScene = SceneManager::Get()->GetSceneByName(L"Portal Scene");
	SceneManager::Get()->RemoveGameScene(portalScene, true);
}
