#include "stdafx.h"
#include "DiffuseMaterial_Shadow_Skinned.h"

DiffuseMaterial_Shadow_Skinned::DiffuseMaterial_Shadow_Skinned():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow_Skinned.fx")
{}

void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	/*
	 * TODO_W8
	 * Update The Shader Variables
	 * 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	 * 	LightWVP = model_world * light_viewprojection
	 * 	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	 * 
	 * 2. Update the ShadowMap texture
	 * 
	 * 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	 * 
	 * 4. Update Bones
	*/

	auto world = pModel->GetTransform()->GetWorld();
	auto lightWVP = XMMatrixMultiply(XMLoadFloat4x4(&world), XMLoadFloat4x4(&ShadowMapRenderer::Get()->GetLightVP()));
	XMFLOAT4X4 lightWorldViewProj{};
	XMStoreFloat4x4(&lightWorldViewProj, lightWVP);

	SetVariable_Matrix(L"gWorldViewProj_Light", lightWorldViewProj);

	SetVariable_Texture(L"gShadowMap", ShadowMapRenderer::Get()->GetShadowMap());

	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	SetVariable_MatrixArray(L"gBones", &pModel->GetAnimator()->GetBoneTransforms()[0]._11, int(pModel->GetAnimator()->GetBoneTransforms().size()));


	//Update Shadow Variables
	//const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	//...
}
