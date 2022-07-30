#include "stdafx.h"
#include "ControlsScene.h"

#include "Prefabs/CubePrefab.h"

void ControlsScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = false;
	m_SceneContext.settings.showInfoOverlay = false;
	//Camera
	auto pCameraObject = new FixedCamera();
	AddChild(pCameraObject);
	auto pCameraComp = new CameraComponent();
	pCameraObject->AddComponent(pCameraComp);
	SetActiveCamera(pCameraComp);

	m_pSprite = new GameObject();
	m_pSprite->AddComponent(new SpriteComponent(L"Textures/Controls.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,0.1f }));
	AddChild(m_pSprite);

	m_pSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .9f);
	m_pSprite->GetTransform()->Scale(1.f, 1.f, 1.f);

	const auto pMaterial = PxGetPhysics().createMaterial(.5f, .5f, .5f);

	auto pReturnButton = AddChild(new CubePrefab(XMFLOAT3{ 1.f, 0.125f, 0.5f }, XMFLOAT4{ Colors::Purple }));
	auto pColQuit = pReturnButton->AddComponent(new RigidBodyComponent(true));
	pColQuit->AddCollider(PxBoxGeometry{ 1.f, 0.25f, 0.5f }, *pMaterial, true);
	pReturnButton->GetTransform()->Translate(GetActiveCamera()->GetTransform()->GetPosition().x, GetActiveCamera()->GetTransform()->GetPosition().y - 1.f, GetActiveCamera()->GetTransform()->GetPosition().z + 5.f);
	pReturnButton->SetTag(L"ReturnButton");
}

void ControlsScene::Update()
{
	if (InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON))
	{
		if (const auto pPickedObject = m_SceneContext.pCamera->Pick())
		{
			if (pPickedObject->GetTag() == L"ReturnButton")
			{
				SceneManager::Get()->SetActiveGameScene(L"Main Menu Scene");
			}
		}
	}
}
