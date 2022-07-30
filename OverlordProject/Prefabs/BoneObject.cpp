#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	:m_pMaterial(pMaterial)
	,m_Length(length)	
{
	
}


void BoneObject::AddBone(BoneObject* pBone)
{
	auto xPos = GetTransform()->GetWorldPosition().x + m_Length;
	pBone->GetTransform()->Translate(xPos, 0.f, 0.f);

	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	FXMMATRIX world = XMLoadFloat4x4(&GetTransform()->GetWorld());
	XMVECTOR determinant = XMMatrixDeterminant(world);
	XMStoreFloat4x4(&m_BindPose, XMMatrixInverse(&determinant, world));

	for(BoneObject* boneObject : GetChildren<BoneObject>())
	{
		boneObject->CalculateBindPose();
	}
}


void BoneObject::Initialize(const SceneContext&)
{
	auto pEmpty = AddChild(new GameObject());

	auto pModel = pEmpty->AddComponent(new ModelComponent(L"Meshes/Bone.ovm"));
	pModel->SetMaterial(m_pMaterial);

	pEmpty->GetTransform()->Rotate(0.f, -90.f, 0.f);
	pEmpty->GetTransform()->Scale(m_Length);
}
