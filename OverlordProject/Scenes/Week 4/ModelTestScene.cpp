#include "stdafx.h"
#include "ModelTestScene.h"

#include "Materials/DiffuseMaterial.h"

void ModelTestScene::Initialize()
{
	//Ground plane
	const auto pBouncyMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.9f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	//Create Material
	const auto pMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMat->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	//Mesh
	m_pChair = AddChild(new GameObject());
	m_pChair->AddComponent(new ModelComponent(L"Meshes/chair.ovm"));

	//Material
	m_pChair->GetComponent<ModelComponent>()->SetMaterial(pMat);

	//Physics
	const auto chairActor = m_pChair->AddComponent(new RigidBodyComponent());

	//Collider
	auto convexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/chair.ovpc");
	//auto triangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/chair.ovpt");
	chairActor->AddCollider(PxConvexMeshGeometry{convexMesh}, *pBouncyMaterial);

	m_pChair->GetTransform()->Translate(0.f, 10.f, 0.f);
}

void ModelTestScene::Update()
{
}
