#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

void PongScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	const auto pBouncyMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.0f);
	const auto pStaticMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.0f);

	
	//Ball
	m_pSphere = AddChild(new SpherePrefab(1.f, 10, XMFLOAT4{ Colors::DarkRed }));
	auto pSphereActor = m_pSphere->AddComponent(new RigidBodyComponent(false));
	pSphereActor->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pSphereActor->SetCollisionGroup(CollisionGroup::Group1);

	pSphereActor->SetConstraint(RigidBodyConstraint::AllRot, false);
	pSphereActor->SetConstraint(RigidBodyConstraint::TransY, false);
	pSphereActor->SetDensity(1.0f);
	pSphereActor->GetPxRigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	auto pCamera = AddChild(new FixedCamera());
	auto pStaticCameraComponent = pCamera->GetComponent<CameraComponent>();
	SetActiveCamera(pStaticCameraComponent);
	pStaticCameraComponent->GetTransform()->Translate(0.f, 25.f, 0.f);
	pStaticCameraComponent->GetTransform()->Rotate(90.f, 0.f, 0.f, true);

	//Peddle 1
	m_pPeddle1 = AddChild(new CubePrefab(1.f, 5.f, 1.f, XMFLOAT4{ Colors::White }));
	auto pPeddle1Actor = m_pPeddle1->AddComponent(new RigidBodyComponent(false));
	pPeddle1Actor->AddCollider(PxBoxGeometry{0.5f, 2.5f, 0.5f}, *pStaticMaterial);

	pPeddle1Actor->SetDensity(10.f);

	m_pPeddle1->GetTransform()->Translate(15.f, 0.f, 0.f);
	m_pPeddle1->GetTransform()->Rotate(90.f, 0.f, 0.f);

	pPeddle1Actor->SetConstraint(RigidBodyConstraint::AllRot, false);
	pPeddle1Actor->SetConstraint(RigidBodyConstraint::TransX, false);
	pPeddle1Actor->SetConstraint(RigidBodyConstraint::TransY, false);
	pPeddle1Actor->SetCollisionGroup(CollisionGroup::Group1);

	//Peddle2
	m_pPeddle2 = AddChild(new CubePrefab(1.f, 5.f, 1.f, XMFLOAT4{ Colors::White }));
	auto pPeddle2Actor = m_pPeddle2->AddComponent(new RigidBodyComponent(false));
	pPeddle2Actor->AddCollider(PxBoxGeometry{ 0.5f, 2.5f, 0.5f }, *pStaticMaterial);

	pPeddle2Actor->SetDensity(10000.f);

	m_pPeddle2->GetTransform()->Translate(-15.f, 0.f, 0.f);
	m_pPeddle2->GetTransform()->Rotate(90.f, 0.f, 0.f);

	pPeddle2Actor->SetConstraint(RigidBodyConstraint::AllRot, false);
	pPeddle2Actor->SetConstraint(RigidBodyConstraint::TransY, false);
	pPeddle2Actor->SetConstraint(RigidBodyConstraint::TransX, false);

	pPeddle2Actor->SetCollisionGroup(CollisionGroup::Group1);

	//Screen Bounds collision
	float aspectRatio = GetSceneContext().aspectRatio;
	float halfHeight = (tan(XM_PIDIV4/2) * GetActiveCamera()->GetTransform()->GetPosition().y);
	float halfWidth = halfHeight * aspectRatio;

	auto pScreenObjects = AddChild(new GameObject());
	auto pScreenBoundsActor = pScreenObjects->AddComponent(new RigidBodyComponent(true));
	pScreenBoundsActor->AddCollider(PxBoxGeometry{ halfWidth, 1.f, 1.f }, *pStaticMaterial, false, PxTransform{0.f, 0.f , halfHeight});
	pScreenBoundsActor->AddCollider(PxBoxGeometry{ halfWidth, 1.f, 1.f }, *pStaticMaterial, false, PxTransform{0.f, 0.f , 0.f - halfHeight });
	pScreenBoundsActor->AddCollider(PxBoxGeometry{ 1.f, 1.f, halfHeight }, *pStaticMaterial, false, PxTransform{halfWidth, 0.f , 0.f });
	pScreenBoundsActor->AddCollider(PxBoxGeometry{ 1.f, 1.f, halfHeight }, *pStaticMaterial, false, PxTransform{0.f - halfWidth, 0.f , 0.f });
	pScreenBoundsActor->SetCollisionGroup(CollisionGroup::Group1);

	//Triggers
	pScreenBoundsActor->AddCollider(PxBoxGeometry{ 1.f, 1.f, halfHeight }, *pStaticMaterial, true, PxTransform{ halfWidth - 1.f, 0.f , 0.f });
	pScreenBoundsActor->AddCollider(PxBoxGeometry{ 1.f, 1.f, halfHeight }, *pStaticMaterial, true, PxTransform{ 1.f - halfWidth, 0.f , 0.f });


	pScreenObjects->SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
		{
			if(action == PxTriggerAction::ENTER)
			{
				pOther->GetTransform()->Translate(0, 0, 0);
				m_Start = true;
			}
		});
}

void PongScene::Update()
{
	if (m_SceneContext.pInput->IsKeyboardKey(InputState{ InputState::down }, VK_UP))
	{
		const auto oldPos = m_pPeddle1->GetTransform()->GetPosition();
		m_pPeddle1->GetTransform()->Translate(oldPos.x, oldPos.y, oldPos.z + (m_SceneContext.pGameTime->GetElapsed() * m_MoveSpeed));
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState{ InputState::down }, VK_DOWN))
	{
		const auto oldPos = m_pPeddle1->GetTransform()->GetPosition();
		m_pPeddle1->GetTransform()->Translate(oldPos.x, oldPos.y, oldPos.z - (m_SceneContext.pGameTime->GetElapsed() * m_MoveSpeed));
	}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState{ InputState::down }, 'W'))
	{
		const auto oldPos = m_pPeddle2->GetTransform()->GetPosition();
		m_pPeddle2->GetTransform()->Translate(oldPos.x, oldPos.y, oldPos.z + (m_SceneContext.pGameTime->GetElapsed() * m_MoveSpeed));
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState{ InputState::down }, 'S'))
	{
		const auto oldPos = m_pPeddle2->GetTransform()->GetPosition();
		m_pPeddle2->GetTransform()->Translate(oldPos.x, oldPos.y, oldPos.z - (m_SceneContext.pGameTime->GetElapsed() * m_MoveSpeed));
	}

	if(m_Start)
	{
		if (m_SceneContext.pInput->IsKeyboardKey(InputState{ InputState::pressed }, 'R'))
		{
			m_pSphere->GetComponent<RigidBodyComponent>()->AddForce(XMFLOAT3{ 50.f, 0.f, 10.f }, PxForceMode::eVELOCITY_CHANGE);
			m_Start = !m_Start;
		}
	}


	if (m_SceneContext.pInput->IsKeyboardKey(InputState{ InputState::pressed }, 'P'))
	{
		GetActiveCamera()->GetTransform()->Translate(0.f, 35.f, 0.f);
	}


}

void PongScene::OnGUI()
{
	ImGui::Text("Controls:\n");
	ImGui::Text("Move peddles: W/S & Up/Down\n");
	ImGui::Text("Start/Restart: R\n");
}
