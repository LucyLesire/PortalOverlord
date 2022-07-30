#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO_W9(L"Implement Destructor")

	delete[] m_ParticlesArray;
	m_ParticlesArray = nullptr;

	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
	}

}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Initialize")

	if(!m_pParticleMaterial)
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();

	CreateVertexBuffer(sceneContext);

	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement CreateVertexBuffer")

	if(m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
	}

	D3D11_BUFFER_DESC dynamicVertexBufferDesc{};
	dynamicVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	dynamicVertexBufferDesc.ByteWidth = sizeof(VertexParticle) * m_ParticleCount;
	dynamicVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dynamicVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dynamicVertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA dynamicVertexBufferData{};

	HANDLE_ERROR(sceneContext.d3dContext.pDevice->CreateBuffer(&dynamicVertexBufferDesc, nullptr, &m_pVertexBuffer));
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Update")

	float particleInterval{ ((m_EmitterSettings.minEnergy + m_EmitterSettings.maxEnergy) / 2.f) / static_cast<float>(m_ParticleCount) };

	//for(UINT i{}; i < m_ParticleCount; ++i)
	//{
	//	particleInterval += m_ParticlesArray[i].totalEnergy;
	//}

	//particleInterval /= m_ParticleCount;

	float elapsedTime = sceneContext.pGameTime->GetElapsed();
	m_LastParticleSpawn += elapsedTime;

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};

	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

	VertexParticle* pBuffer = nullptr;
	pBuffer = static_cast<VertexParticle*>(mappedSubresource.pData);

	for (UINT i{}; i < m_ParticleCount; ++i)
	{
		if(m_ParticlesArray[i].isActive)
		{
			UpdateParticle(m_ParticlesArray[i], elapsedTime);
		}
		if(!m_ParticlesArray[i].isActive)
		{
			if(m_LastParticleSpawn >= particleInterval)
			{
				SpawnParticle(m_ParticlesArray[i]);
			}
		}
		if (m_ParticlesArray[i].isActive)
		{
			pBuffer[m_ActiveParticles] = m_ParticlesArray[i].vertexInfo;
			m_ActiveParticles++;
		}
	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	//TODO_W9(L"Implement UpdateParticle")

	//Return if particle is not active
	if(!p.isActive)
	{
		return;
	}

	p.currentEnergy -= elapsedTime;
	if(p.currentEnergy <= 0.f)
	{
		p.isActive = false;
		return;
	}

	//Update Position with Velocity and time
	auto velocity = XMLoadFloat3(&m_EmitterSettings.velocity);
	auto pos = XMLoadFloat3(&p.vertexInfo.Position);
	pos += velocity * elapsedTime;

	XMStoreFloat3(&p.vertexInfo.Position, pos);

	//Get the alpha percentage (lifetime)
	float lifePercent = p.currentEnergy / p.totalEnergy;

	//Update the color and alpha
	XMFLOAT4 color = m_EmitterSettings.color;
	color.w *= lifePercent;
	p.vertexInfo.Color = color;

	p.vertexInfo.Size = std::lerp(p.initialSize, p.initialSize * p.sizeChange, 1.f - lifePercent);

}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	//TODO_W9(L"Implement SpawnParticle")
	p.isActive = true;

	// --- Erergy init ---
	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;

	// --- Position Init ---
	//Random Direction
	XMVECTOR randDir{ 1.f, 0.f, 0.f };
	auto rotationMatrix = XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI));
	randDir = XMVector3TransformNormal(randDir, rotationMatrix);

	//Distance
	float distance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);

	//Store position
	XMVECTOR gameObjPos = XMVectorSet(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y, GetTransform()->GetPosition().z, 1.f);
	XMStoreFloat3(&p.vertexInfo.Position, randDir * distance + gameObjPos);

	// --- Size Init ---
	p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.initialSize = p.vertexInfo.Size;

	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	// --- Rotation Init ---
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	// --- Color Init ---
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement PostDraw")

	//Pass the VP of camera (Particles are in World-Space)
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	//Pass ViewInverse of camera
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	//Pass Texture of the particle
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	auto techniqueContext = m_pParticleMaterial->GetTechniqueContext();

	//	- Set InputLayout (see TechniqueContext)
	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);

	//	- Set PrimitiveTopology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	//Set Vertex Buffer
	const UINT offset = 0;
	const auto& size = UINT(sizeof(VertexParticle));
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer,&size , &offset);

	//DRAW
	auto tech = m_pParticleMaterial->GetTechniqueContext().pTechnique;
	D3DX11_TECHNIQUE_DESC techDesc{};

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}