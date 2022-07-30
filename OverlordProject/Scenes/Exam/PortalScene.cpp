#include "stdafx.h"
#include "PortalScene.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/Portal.h"


#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/ScreenPortalMaterial.h"

#include "Materials/Post/PostGrayscale.h"
#include "Materials/Post/PostBlur.h"

#include <Materials\ColorMaterial.h>

#include "Graphics/PortalRenderer.h"
#include "Prefabs/Character.h"

PortalScene::~PortalScene()
{
}

void PortalScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.showInfoOverlay = false;
	m_SceneContext.settings.enableOnGUI = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Portals
	m_pPortals.emplace_back(new Portal(nullptr, XMFLOAT4{Colors::Purple}));
	m_pPortals.emplace_back(new Portal(nullptr));

	m_pPortals[0]->SetLinkedPortal(m_pPortals[1]);
	m_pPortals[1]->SetLinkedPortal(m_pPortals[0]);

	m_pPortals[0]->GetTransform()->Scale(1.f);
	m_pPortals[0]->GetTransform()->Rotate(0, 90, 0);
	m_pPortals[0]->GetTransform()->Translate(-10, 5.f, 0);

	m_pPortals[1]->GetTransform()->Scale(1.f);
	m_pPortals[1]->GetTransform()->Rotate(0, 90, 0);
	m_pPortals[1]->GetTransform()->Translate(10, 5.f, 0);

	AddChild(m_pPortals[0]);
	AddChild(m_pPortals[1]);

	m_pMainCamera = GetActiveCamera();
	PortalRenderer::Get()->SetRenderTargets(m_pPortals[0]->GetRenderTarget(), m_pPortals[1]->GetRenderTarget());

	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.controller.height = 5.f;
	characterDesc.controller.radius = 3.f;
	characterDesc.maxFallSpeed = 98.1f;
	characterDesc.maxMoveSpeed = 25.f;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(-182.f, 31.f ,6.5f);

	m_pCharacter->GetComponent<ControllerComponent>()->SetCollisionGroup(CollisionGroup::Group0);

	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/LevelBackFace.ovm"));
	auto pLevelMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pLevelMat->SetDiffuseTexture(L"Textures/GridBox_Default.png");
	pLevelMesh->SetMaterial(pLevelMat);

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/LevelBackFace.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ 0.1f,0.1f,0.1f })), *pDefaultMaterial);
	pLevelObject->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	pLevelActor->SetCollisionIgnoreGroups(CollisionGroup::Group0);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PrimaryFire, InputState::pressed, -1, VK_LBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(SecondaryFire, InputState::pressed, -1, VK_RBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);

	m_ActionId_PrimaryFire = PrimaryFire;
	m_ActionId_SecondaryFire = SecondaryFire;

	//Door exit
	auto pDoorObject = AddChild(new GameObject());
	const auto pDoorModel = pDoorObject->AddComponent(new ModelComponent(L"Meshes/Door.ovm"));
	const auto pDoor = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pDoor->SetDiffuseTexture(L"Textures/portal_door_b.png");

	//Door exit collider
	pDoorModel->SetMaterial(pDoor);
	auto pDoorRigid =	pDoorObject->AddComponent(new RigidBodyComponent(true));
	pDoorRigid->AddCollider(PxBoxGeometry{ 20.f, 8.f, 5.f }, * pDefaultMaterial, true, PxTransform{ 0.f, 8.f, 0.f });
	pDoorObject->SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER)
			{
				if (dynamic_cast<PortalTraveller*>(pOther))
				{
					SceneManager::Get()->SetActiveGameScene(L"Main Menu Scene");
				}
			}
		});

	pDoorObject->GetTransform()->Scale(0.25f);
	pDoorObject->GetTransform()->Translate(-50.f, 40.f, 10.f);

}

void PortalScene::Update()
{
	//Portal shooting
	if (GetSceneContext().pInput->IsActionTriggered(m_ActionId_PrimaryFire))
	{
		CollisionGroup ignoreGroups = CollisionGroup::Group2;
		PxQueryFilterData filterData{};
		filterData.data.word0 = ~UINT(ignoreGroups);

		PxRaycastBuffer hit{};
		auto charPos = m_pCharacter->GetTransform()->GetWorldPosition();
		auto charDir = m_pCharacter->GetTransform()->GetForward();
		PxVec3 rayStart{ charPos.x, charPos.y + 5.f, charPos.z };
		PxVec3 rayDirection{ charDir.x, charDir.y, charDir.z };


		if (GetPhysxProxy()->Raycast(rayStart, rayDirection, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
		{
			//Calculate x, y and z
			XMFLOAT3 x{ hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };
			auto z = XMVector3Cross(XMLoadFloat3(&x), { 0.f, 1.f, 0.f });
			auto y = XMVector3Cross(z, XMLoadFloat3(&x));

			//Make rotation matrix
			XMMATRIX rotMatrix{ XMLoadFloat3(&x), y, z, {0,0,0,1} };
			auto portalRotation = XMQuaternionRotationMatrix(rotMatrix);

			//Apply offset from perpendicular to wall
			auto yOffset = XMQuaternionRotationAxis({ 0.f, 1.f, 0.f }, XMConvertToRadians(90.f));
			portalRotation = XMQuaternionMultiply(yOffset, portalRotation);

			m_pPortals[0]->GetTransform()->Rotate(portalRotation);

			XMVECTOR yAxis{};
			float pAngle{};
			XMQuaternionToAxisAngle(&yAxis, &pAngle, portalRotation);

			m_pPortals[0]->SetDirection(XMConvertToDegrees(pAngle));

			//Translate portal + zoffset
			auto offset = XMVectorScale(XMLoadFloat3(&x), 2.5f);
			auto hitPos = PhysxHelper::ToXMFLOAT3(hit.block.position);
			auto newPos = XMLoadFloat3(&hitPos);
			newPos += offset;

			m_pPortals[0]->GetTransform()->Translate(newPos);
		}
	}

	if (GetSceneContext().pInput->IsActionTriggered(m_ActionId_SecondaryFire))
	{
		CollisionGroup ignoreGroups = CollisionGroup::Group2;
		PxQueryFilterData filterData{};
		filterData.data.word0 = ~UINT(ignoreGroups);

		PxRaycastBuffer hit{};
		auto charPos = m_pCharacter->GetTransform()->GetWorldPosition();
		auto charDir = m_pCharacter->GetTransform()->GetForward();
		PxVec3 rayStart{ charPos.x, charPos.y + 5.f, charPos.z };
		PxVec3 rayDirection{ charDir.x, charDir.y, charDir.z };


		if (GetPhysxProxy()->Raycast(rayStart, rayDirection, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
		{

			//Calculate x, y and z
			XMFLOAT3 x{ hit.block.normal.x, hit.block.normal.y, hit.block.normal.z};
			auto z = XMVector3Cross(XMLoadFloat3(&x), { 0.f, 1.f, 0.f });
			auto y = XMVector3Cross(z, XMLoadFloat3(&x));

			//Make rotation matrix
			XMMATRIX rotMatrix{ XMLoadFloat3(&x), y, z, {0,0,0,1} };
			auto portalRotation = XMQuaternionRotationMatrix(rotMatrix);

			//Apply offset from perpendicular to wall
			auto yOffset = XMQuaternionRotationAxis({ 0.f, 1.f, 0.f }, XMConvertToRadians(90.f));
			portalRotation = XMQuaternionMultiply(yOffset, portalRotation);

			m_pPortals[1]->GetTransform()->Rotate(portalRotation);

			XMVECTOR yAxis{};
			float pAngle{};
			XMQuaternionToAxisAngle(&yAxis, &pAngle, portalRotation);

			m_pPortals[1]->SetDirection(XMConvertToDegrees(pAngle));

			//Translate portal + zoffset
			auto offset = XMVectorScale(XMLoadFloat3(&x), 2.5f);
			auto hitPos = PhysxHelper::ToXMFLOAT3(hit.block.position);
			auto newPos = XMLoadFloat3(&hitPos);
			newPos += offset;

			m_pPortals[1]->GetTransform()->Translate(newPos);
		}
	}
}

void PortalScene::LateUpdate()
{
	for(auto pPortal : m_pPortals)
	{
		pPortal->LateUpdate();
	}
}

void PortalScene::Draw()
{

}

void PortalScene::PostDraw()
{
	
	auto pMainCam = GetActiveCamera();

	//Draw portal 0 to screen 1
	SetActiveCamera(m_pPortals[0]->GetCamera()->GetComponent<CameraComponent>());
	m_pPortals[0]->SetNearClipPlane();
	DrawPortals(m_pPortals[0]->GetRenderTarget());
	m_pPortals[1]->GetScreenMat()->SetVariable_Texture(L"gTexture", m_pPortals[0]->GetRenderTarget()->GetColorShaderResourceView());


	//Draw portal 1 to screen 0
	SetActiveCamera(m_pPortals[1]->GetCamera()->GetComponent<CameraComponent>());
	m_pPortals[1]->SetNearClipPlane();
	DrawPortals(m_pPortals[1]->GetRenderTarget());
	m_pPortals[0]->GetScreenMat()->SetVariable_Texture(L"gTexture", m_pPortals[1]->GetRenderTarget()->GetColorShaderResourceView());


	pMainCam->SetActive(true);

}

void PortalScene::OnGUI()
{
	m_pCharacter->DrawImGui();
}

