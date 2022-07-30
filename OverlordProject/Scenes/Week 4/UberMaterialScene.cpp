#include "stdafx.h"
#include "UberMaterialScene.h"

#include "Materials/UberMaterial.h"


void UberMaterialScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_pSphere = AddChild(new GameObject());
	m_pSphere->AddComponent(new ModelComponent(L"Meshes/Sphere.ovm"));

	m_pUberMat = MaterialManager::Get()->CreateMaterial<UberMaterial>();

	m_pUberMat->SetCameraVariables(GetActiveCamera());
	m_pUberMat->SetDiffuseTexture(L"Textures/Skulls_Diffusemap.tga");
	m_pUberMat->SetNormalTexture(L"Textures/Skulls_Normalmap.tga");
	m_pUberMat->SetSpecularTexture(L"Textures/Skulls_HeightMap.tga");
	m_pUberMat->SetEnvironmentTexture(L"Textures/Sunol_Cubemap.dds");

	m_pSphere->GetComponent<ModelComponent>()->SetMaterial(m_pUberMat);

	m_pSphere->GetTransform()->Scale(15.f, 15.f, 15.f);
	const auto pRigidBody = m_pSphere->AddComponent(new RigidBodyComponent());
	pRigidBody->SetConstraint(RigidBodyConstraint::AllTrans, false);
}

void UberMaterialScene::Update()
{
	m_pUberMat->UpdateEffectVariables(m_SceneContext, m_pSphere->GetComponent<ModelComponent>());
	m_pSphere->GetTransform()->Rotate(0.f, m_SceneContext.pGameTime->GetTotal() * m_Speed, 0.f, true);
}

void UberMaterialScene::Draw()
{
}

void UberMaterialScene::OnGUI()
{
	m_pUberMat->DrawImGui();
	ImGui::SliderFloat("Rotation speed", &m_Speed, 0.f, 100.f);
}

