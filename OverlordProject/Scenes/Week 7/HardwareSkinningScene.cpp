#include "stdafx.h"
#include "HardwareSkinningScene.h"

#include "Materials/DiffuseMaterial_Skinned.h"

HardwareSkinningScene::~HardwareSkinningScene()
{
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		delete[] m_ClipNames[i];
	}

	delete[] m_ClipNames;
}

void HardwareSkinningScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	const auto pSkinnedMaterialTorso = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pSkinnedMaterialLegs = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pSkinnedMaterialHead = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pSkinnedMaterialHair = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pSkinnedMaterialEye = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterialTorso->SetDiffuseTexture(L"Textures/chell_torso_diffuse.png");
	pSkinnedMaterialLegs->SetDiffuseTexture(L"Textures/chell_legs_diffuse.png");
	pSkinnedMaterialHair->SetDiffuseTexture(L"Textures/chell_hair.png");
	pSkinnedMaterialHead->SetDiffuseTexture(L"Textures/chell_head_diffuse.png");
	pSkinnedMaterialEye->SetDiffuseTexture(L"Textures/eyeball_l.png");

	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Chell3.ovm"));
	pModel->SetMaterial(pSkinnedMaterialTorso, 3);
	pModel->SetMaterial(pSkinnedMaterialLegs, 4);
	pModel->SetMaterial(pSkinnedMaterialHead, 0);
	pModel->SetMaterial(pSkinnedMaterialEye, 1);
	pModel->SetMaterial(pSkinnedMaterialEye, 2);
	pModel->SetMaterial(pSkinnedMaterialHair, 5);
	
	//pModel->GetTransform()->Scale(0.01f);

	pObject->GetTransform()->Scale(0.005f);

	
	pAnimator = pModel->GetAnimator();
	if(pAnimator)
	{
		pAnimator->SetAnimation(m_AnimationClipId);
		pAnimator->SetAnimationSpeed(m_AnimationSpeed);

		//Gather Clip Names
		m_ClipCount = pAnimator->GetClipCount();
		m_ClipNames = new char* [m_ClipCount];
		for (UINT i{ 0 }; i < m_ClipCount; ++i)
		{
			auto clipName = StringUtil::utf8_encode(pAnimator->GetClip(static_cast<int>(i)).name);
			const auto clipSize = clipName.size();
			m_ClipNames[i] = new char[clipSize + 1];
			strncpy_s(m_ClipNames[i], clipSize + 1, clipName.c_str(), clipSize);
		}
	}

}

void HardwareSkinningScene::OnGUI()
{
	if (ImGui::Button(pAnimator->IsPlaying() ? "PAUSE" : "PLAY"))
	{
		if (pAnimator->IsPlaying())pAnimator->Pause();
		else pAnimator->Play();
	}

	if (ImGui::Button("RESET"))
	{
		pAnimator->Reset();
	}

	ImGui::Dummy({ 0,5 });

	bool reversed = pAnimator->IsReversed();
	if (ImGui::Checkbox("Play Reversed", &reversed))
	{
		pAnimator->SetPlayReversed(reversed);
	}

	if (ImGui::ListBox("Animation Clip", &m_AnimationClipId, m_ClipNames, static_cast<int>(m_ClipCount)))
	{
		pAnimator->SetAnimation(m_AnimationClipId);
	}

	if (ImGui::SliderFloat("Animation Speed", &m_AnimationSpeed, 0.f, 4.f))
	{
		pAnimator->SetAnimationSpeed(m_AnimationSpeed);
	}
}