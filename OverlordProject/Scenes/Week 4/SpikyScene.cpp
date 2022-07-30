#include "stdafx.h"
#include "SpikyScene.h"

#include "Materials/SpikyMaterial.h"


void SpikyScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_pSphere = AddChild(new GameObject());
	m_pSphere->AddComponent(new ModelComponent(L"Meshes/OctaSphere.ovm"));
	m_pSphere->GetTransform()->Scale(15.f, 15.f, 15.f);

	m_pSpikyMat = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();
	m_pSphere->GetComponent<ModelComponent>()->SetMaterial(m_pSpikyMat);
}

void SpikyScene::Update()
{
	m_pSpikyMat->UpdateEffectVariables(m_SceneContext, m_pSphere->GetComponent<ModelComponent>());
	m_pSphere->GetTransform()->Rotate(0.f, m_SceneContext.pGameTime->GetTotal() * m_Speed, 0.f, true);
}

void SpikyScene::OnGUI()
{
	m_pSpikyMat->DrawImGui();
}
