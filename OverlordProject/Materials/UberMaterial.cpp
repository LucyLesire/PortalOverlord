#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial()
	: Material<UberMaterial>(L"Effects/UberShader.fx")
{
}

void UberMaterial::InitializeEffectVariables()
{
	//Camera variables
	SetVariable_Matrix(L"gMatrixWVP", XMFLOAT4X4{});
	SetVariable_Matrix(L"gMatrixViewInverse", XMFLOAT4X4{});
	SetVariable_Matrix(L"gMatrixWorld", XMFLOAT4X4{});

	//Light
	SetVariable_Vector(L"gLightDirection", XMFLOAT3(0.577f, 0.577f, 0.577f));

	//Diffuse
	SetVariable_Scalar(L"gUseTextureDiffuse", false);
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT4(1.f, 1.f, 1.f, 1.f));
}

void UberMaterial::SetCameraVariables(CameraComponent* pCamera)
{
	SetVariable_Matrix(L"gMatrixWVP", pCamera->GetViewProjection());
	SetVariable_Matrix(L"gMatrixViewInverse", pCamera->GetViewInverse());
	SetVariable_Matrix(L"gMatrixWorld", pCamera->GetTransform()->GetWorld());
}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}

void UberMaterial::SetSpecularTexture(const std::wstring& assetFile)
{
	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularTexture);
}

void UberMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
}

void UberMaterial::SetOpacityMap(const std::wstring& assetFile)
{
	m_pOpacityTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureOpacity", m_pOpacityTexture);
}

void UberMaterial::SetEnvironmentTexture(const std::wstring& assetFile)
{
	m_pCubeTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gCubeEnvironment", m_pCubeTexture);
}
