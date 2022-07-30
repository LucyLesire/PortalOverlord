#include "stdafx.h"
#include "PortalGun.h"

#include "Materials/DiffuseMaterial.h"
#include "Materials/UberMaterial.h"

void PortalGun::Initialize(const SceneContext&)
{
	auto pPortalModel = AddComponent(new ModelComponent(L"Meshes/PortalGun.ovm"));


	const auto pPortalGunMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	const auto pPortalGunGlassMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();

	pPortalGunMat->SetDiffuseTexture(L"Textures/v_portalgun.png");
	pPortalGunGlassMat->SetDiffuseTexture(L"Textures/v_portalgun_glass.png");

	pPortalModel->SetMaterial(pPortalGunMat, 0);
	pPortalModel->SetMaterial(pPortalGunGlassMat, 1);


	GetTransform()->Scale(0.5f);


}
