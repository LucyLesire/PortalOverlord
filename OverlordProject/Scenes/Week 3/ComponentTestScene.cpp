#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/SpherePrefab.h"

void ComponentTestScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	const auto pBouncyMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.9f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	//Sphere 1 (Group 1)
	auto pSphereObject = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4{ Colors::DarkRed }));
	auto pSphereActor = pSphereObject->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pSphereActor->SetCollisionGroup(CollisionGroup::Group1);

	pSphereObject->GetTransform()->Translate(0.f, 40.f, 0.f);

	//Sphere 2
	pSphereObject = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4{ Colors::DarkGreen }));
	pSphereActor = pSphereObject->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pSphereActor->SetCollisionIgnoreGroups(CollisionGroup::Group1);

	pSphereObject->GetTransform()->Translate(0.f, 30.f, 0.f);

	//Sphere 2
	pSphereObject = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4{ Colors::DarkBlue }));
	pSphereActor = pSphereObject->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pSphereActor->SetCollisionIgnoreGroups(CollisionGroup::Group1);

	pSphereObject->GetTransform()->Translate(0.f, 20.f, 0.f);
}
