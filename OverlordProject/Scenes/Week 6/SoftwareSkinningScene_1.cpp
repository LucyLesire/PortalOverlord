#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_1::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	const auto pMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();

	GameObject* pRoot = AddChild(new GameObject());

	m_pBone0 = pRoot->AddChild(new BoneObject(pMat));
	m_pBone1 = new BoneObject(pMat);
	m_pBone0->AddBone(m_pBone1);
}

void SoftwareSkinningScene_1::Update()
{
	if(m_AutoRotate)
	{
		const float speed = 45.f;
		m_BoneRotation += m_RotationSign * GetSceneContext().pGameTime->GetElapsed() * speed;
		if (m_BoneRotation < -45.f || m_BoneRotation > 45.f)
			m_RotationSign *= -1;

		m_RotBone0 = XMFLOAT3{ 0.f, 0.f, m_BoneRotation };
		m_RotBone1 = XMFLOAT3{ 0.f, 0.f, -m_BoneRotation * 2.f };
	}
	m_pBone0->GetTransform()->Rotate(m_RotBone0);
	m_pBone1->GetTransform()->Rotate(m_RotBone1);
}

void SoftwareSkinningScene_1::OnGUI()
{
	if(!m_AutoRotate)
	{
		ImGui::SliderFloat3("Bone 0 - ROT", &m_RotBone0.x, -180.f, 180.f);
		ImGui::SliderFloat3("Bone 1 - ROT", &m_RotBone1.x, -180.f, 180.f);
	}
	ImGui::Checkbox("Auto rotate", &m_AutoRotate);
}

