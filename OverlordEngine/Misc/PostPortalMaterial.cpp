//Resharper Disable All
	#include "stdafx.h"
	#include "PostPortalMaterial.h"

	PostPortalMaterial::PostPortalMaterial(float width, float height)
		:PostProcessingMaterial(L"Effects/PostPortal.fx")
		, m_Width(width)
		, m_Heigth(height)
	{
		
	}


	void PostPortalMaterial::Initialize(const GameContext&)
	{
		XMMATRIX viewProj{};
		m_pBaseEffect->GetVariableByName("gWorldViewProj")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&viewProj));
	}

	void PostPortalMaterial::UpdateBaseEffectVariables(const SceneContext& sceneContext, RenderTarget* pSource)
	{
		auto world = XMLoadFloat4x4(&sceneContext.pCamera->GetTransform()->GetWorld());
		auto view = XMLoadFloat4x4(&sceneContext.pCamera->GetView());
		auto proj = XMLoadFloat4x4(&sceneContext.pCamera->GetProjection());
		world = world * view * proj;

		m_pBaseEffect->GetVariableByName("gWorldViewProj")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&world));

		const auto pSourceSRV = pSource->GetColorShaderResourceView();
		m_pBaseEffect->GetVariableByName("gTexture")->AsShaderResource()->SetResource(pSourceSRV);
	}

	void PostPortalMaterial::SetWorldViewProj(XMMATRIX worldViewProj)
	{
		m_pBaseEffect->GetVariableByName("gWorldViewProj")->AsMatrix()->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	}
