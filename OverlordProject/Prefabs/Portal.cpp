#include "stdafx.h"
#include "Portal.h"

#include "PortalTraveller.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/ScreenPortalMaterial.h"
#include "Misc/PostPortalMaterial.h"

Portal::Portal(Portal* pLinkedPortal, XMFLOAT4 color)
	:m_pLinkedPortal(pLinkedPortal)
	,m_Color(color)
{
}

Portal::~Portal()
{
}

void Portal::Initialize(const SceneContext& sceneContext)
{
	//Render target
	m_pRenderTarget = new RenderTarget(sceneContext.d3dContext);
	RENDERTARGET_DESC rDesc{};
	rDesc.enableColorSRV = true;
	rDesc.enableColorBuffer = true;
	rDesc.enableDepthSRV = true;
	rDesc.width = static_cast<UINT>(sceneContext.windowWidth);
	rDesc.height = static_cast<UINT>(sceneContext.windowHeight);

	m_pRenderTarget->Create(rDesc);

	//Cameras
	m_pMainCamera = sceneContext.pCamera;
	m_pPortalCamera = new CameraComponent();
	m_pPortalCamera->SetActive(false);

	m_pCamera = new FreeCamera();
	m_pCamera->AddComponent(m_pPortalCamera);
	m_pCamera->SetTag(L"PortalCam");

	auto pCamMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	pCamMat->SetColor(m_Color);

	//auto pModel = m_pCamera->AddComponent(new ModelComponent(L"Meshes/Sphere.ovm", false));
	//pModel->SetMaterial(pCamMat);

	AddChild(m_pCamera);

	m_pPortalMat = MaterialManager::Get()->CreateMaterial<PostPortalMaterial>();

	//Screen
	auto pScreenObject = AddChild(new GameObject());
	m_pScreenMat = MaterialManager::Get()->CreateMaterial<ScreenPortalMaterial>();
	m_pPortalModel = new ModelComponent(L"Meshes/UnitPlane.ovm");
	m_pPortalModel->SetMaterial(m_pScreenMat);
	pScreenObject->AddComponent(m_pPortalModel);

	//Frame
	auto pFrameObject = AddChild(new GameObject());
	auto pFrame = new ModelComponent(L"Meshes/PortalFrame.ovm");
	pFrame->SetMaterial(pCamMat);
	pFrameObject->AddComponent(pFrame);


	m_pPortalModel->GetTransform()->Scale(1.f, 1.f, 1.3f);
	pFrame->GetTransform()->Scale(3.f, 4.f, 2.f);
	m_pPortalModel->GetTransform()->Rotate(90.f, 180.f, 0.f);
	pFrame->GetTransform()->Rotate(0.f, 0.f, 90.f);
	m_pPortalModel->GetTransform()->Translate(0.f, 2.4f, 0.2f);
	pFrame->GetTransform()->Translate(0.f, 2.f, 0.f);


	//Portal collision
	const auto pStaticMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.0f);
	auto pRigidBody = AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxBoxGeometry{ 5.f, 6.f, 2.f }, *pStaticMaterial, true, PxTransform{0.f, 3.f, 0.f});

	DebugRenderer::ToggleDebugRenderer();

	SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
		{
			if (action == PxTriggerAction::ENTER)
			{
				if (dynamic_cast<PortalTraveller*>(pOther))
				{
					OnTravellerEnterPortal(dynamic_cast<PortalTraveller*>(pOther));
				}


			}
			if(action == PxTriggerAction::LEAVE)
			{
				if (dynamic_cast<PortalTraveller*>(pOther))
				{
					auto it = std::find(m_pTravellers.begin(), m_pTravellers.end(), pOther);
					if (it != m_pTravellers.end())
					{
						m_pTravellers.erase(it);
					}
				}
			}
		});
}

void Portal::OnTravellerEnterPortal(PortalTraveller* pTraveller)
{
	if (std::find(m_pTravellers.begin(), m_pTravellers.end(), pTraveller) == m_pTravellers.end())
	{
		m_pTravellers.emplace_back(pTraveller);
	}
}


void Portal::LateUpdate()
{
	Travel();
}

void Portal::Travel()
{
	for (PortalTraveller* pTraveller : m_pTravellers)
	{
		//Calculate traveller offset torwards this portal 
 		const DirectX::XMFLOAT3& travellerPos = pTraveller->GetTransform()->GetWorldPosition();
		DirectX::XMVECTOR xmTravellerPos = DirectX::XMLoadFloat3(&travellerPos);
		const DirectX::XMFLOAT3 travellerForward = pTraveller->GetTransform()->GetForward();
		DirectX::XMVECTOR xmTravellerForward = DirectX::XMLoadFloat3(&travellerForward);

		const DirectX::XMFLOAT3& thisPortalPos = GetTransform()->GetPosition();
		DirectX::XMVECTOR xmThisPortalPos = DirectX::XMLoadFloat3(&thisPortalPos);

		xmThisPortalPos -= xmTravellerForward * 2.f;

		//Current offset from portal
		DirectX::XMVECTOR xmTravellerOffsetFromPortal = DirectX::XMVectorSubtract(xmTravellerPos, xmThisPortalPos);
		
		//Ignore on first entry, if not ignored this can cause problems with the checking of previous offset
		if (pTraveller->IsFirstEntry())
		{
			//Set up previous distance for next frame
			DirectX::XMFLOAT3 prevDist{};
			DirectX::XMStoreFloat3(&prevDist, xmTravellerOffsetFromPortal);
			pTraveller->SetPrevDistanceFromPortal(prevDist);
			pTraveller->SetFirstEntry(false);
		}

		//Previous offset from portal
		const DirectX::XMFLOAT3& prevTravellerOffsetFromPortal = pTraveller->GetPrevDistanceFromPortal();
		DirectX::XMVECTOR xmPrevTravellerOffsetFromPortal = DirectX::XMLoadFloat3(&prevTravellerOffsetFromPortal);

		//Current portal side
		DirectX::XMVECTOR xmPortalFwd = DirectX::XMLoadFloat3(&GetTransform()->GetForward());

		DirectX::XMVECTOR xmDot = DirectX::XMVector3Dot(xmTravellerOffsetFromPortal, xmPortalFwd);
		float dot{ 0.f };
		DirectX::XMStoreFloat(&dot, xmDot);
		int portalSide = (dot < 0.f) ? -1 : 1;

		//Old portal side
		DirectX::XMVECTOR xmOldDot = DirectX::XMVector3Dot(xmPrevTravellerOffsetFromPortal, xmPortalFwd);
		float oldDot{ 0.f };
		DirectX::XMStoreFloat(&oldDot, xmOldDot);
		int portalSideOld = (oldDot < 0.f) ? -1 : 1;

		//Teleport the traveller if it has crossed from one side of the portal to the other
		if (portalSide != portalSideOld)
		{
			//Get portal position, offset the Y with quarter of the height (to get position slightly above the bottom y-value)
			float quarterHeight = GetLinkedPortal()->GetHeight() / 4.f;
			DirectX::XMFLOAT3 otherPortalPos = GetLinkedPortal()->GetTransform()->GetWorldPosition();
			otherPortalPos.y -= quarterHeight;
			DirectX::XMVECTOR xmOtherPortalPos = DirectX::XMLoadFloat3(&otherPortalPos);

			//Get portal's direction to offset the traveller in front of portal
			const DirectX::XMFLOAT3& otherPortalDir = GetLinkedPortal()->GetTransform()->GetForward();
			DirectX::XMVECTOR xmOtherPortalDir = DirectX::XMLoadFloat3(&otherPortalDir);
			xmOtherPortalDir = DirectX::XMVector3Normalize(xmOtherPortalDir);
			xmOtherPortalDir = DirectX::XMVectorScale(xmOtherPortalDir, 0.1f);

			//Teleport traveller with offsetted position
			DirectX::XMVECTOR xmTeleportPos = DirectX::XMVectorAdd(xmOtherPortalPos, xmOtherPortalDir);
			pTraveller->GetTransform()->Translate(xmTeleportPos);

			//Calculate the needed offset to make the character face the right way when going out the portal
			DirectX::XMFLOAT3 travellerFwd = pTraveller->GetTransform()->GetForward();
			travellerFwd.y = 0.f;
			DirectX::XMVECTOR xmTravellerFwd = DirectX::XMLoadFloat3(&travellerFwd);
			xmTravellerFwd = DirectX::XMVector3Normalize(xmTravellerFwd);

			DirectX::XMFLOAT3 camFwd = GetLinkedPortal()->GetCamera()->GetTransform()->GetForward();
			camFwd.y = 0.f;
			DirectX::XMVECTOR xmCamFwd = DirectX::XMLoadFloat3(&camFwd);
			xmCamFwd = DirectX::XMVector3Normalize(xmCamFwd);

			DirectX::XMVECTOR xmAngle = DirectX::XMVector3AngleBetweenVectors(xmCamFwd, xmTravellerFwd);
			float angle{};
			DirectX::XMStoreFloat(&angle, xmAngle);
			angle = DirectX::XMConvertToDegrees(angle);

			//Determine sign of angle
			DirectX::XMVECTOR xmCross = DirectX::XMVector3Cross(xmCamFwd, xmTravellerFwd);
			DirectX::XMVECTOR xmUp{ 0.f, 1.f, 0.f };
			DirectX::XMVECTOR xmSign = DirectX::XMVector3Dot(xmUp, xmCross);


			std::cout << GetPortalDir() << std::endl;

			pTraveller->SetYOffsetAngle(angle);
			pTraveller->SetFirstEntry(true);

			m_pTravellers.erase(std::remove(m_pTravellers.begin(), m_pTravellers.end(), pTraveller));

		}
		else
		{
			//Set up previous distance for next frame
			DirectX::XMFLOAT3 prevDist{};
			DirectX::XMStoreFloat3(&prevDist, xmTravellerOffsetFromPortal);
			pTraveller->SetPrevDistanceFromPortal(prevDist);
		}
	}
}

void Portal::SetNearClipPlane()
{
	const DirectX::XMFLOAT3& portalPos = GetTransform()->GetWorldPosition();
	DirectX::XMVECTOR xmPortalPos = DirectX::XMLoadFloat3(&portalPos);

	const DirectX::XMFLOAT3& portalNormal = GetTransform()->GetForward();
	DirectX::XMVECTOR xmPortalNormal = DirectX::XMLoadFloat3(&portalNormal);
	DirectX::XMVECTOR xmInvPortalNormal = DirectX::XMVectorScale(xmPortalNormal, -1); //rotate 180 degrees

	//Camera distance
	auto xmDot = DirectX::XMVector3Dot(xmPortalPos, xmInvPortalNormal);
	float camDist{};
	DirectX::XMStoreFloat(&camDist, xmDot);

	//Don't use oblique clip plane if very close to portal as it seems this can cause some visual artifacts
	float nearClipLimit = 0.3f;
	if (abs(camDist) > nearClipLimit)
	{
		//Create and set clipping plane vector
		DirectX::XMFLOAT4 clipPlane{ portalNormal.x, portalNormal.y, portalNormal.z, camDist };
		m_pPortalCamera->SetOblique(true);
		m_pPortalCamera->SetClipPlane(clipPlane);
	}
	else
	{
		m_pPortalCamera->SetOblique(false);
	}
}

void Portal::Update(const SceneContext& sceneContext)
{
	m_pMainCamera = sceneContext.pCamera;

	auto portalT = GetTransform();
	auto linkedPortalT = m_pLinkedPortal->GetTransform();
	auto camT = m_pMainCamera->GetTransform();

	XMVECTOR relativeRotCam = XMLoadFloat4(&camT->GetWorldRotation());
	XMVECTOR relativeRotIn = XMLoadFloat4(&portalT->GetWorldRotation());
	XMVECTOR relativeRotOut = XMLoadFloat4(&linkedPortalT->GetWorldRotation());

	XMVECTOR relativePosCam = XMLoadFloat3(&camT->GetWorldPosition());
	XMVECTOR relativePosIn = XMLoadFloat3(&portalT->GetWorldPosition());
	XMVECTOR relativePosOut = XMLoadFloat3(&linkedPortalT->GetWorldPosition());



	//Rotate Camera according to main camera in portal space
	auto relativeRot = XMQuaternionInverse(relativeRotOut) * relativeRotCam;

	//Calculate rotation offset
	XMMATRIX yRotDiff{};
	if (static_cast<int>(std::roundf(GetLinkedPortal()->GetPortalDir())) % 180 == 0)
	{
		yRotDiff = XMMatrixRotationAxis(XMLoadFloat3(&portalT->GetUp()), XMConvertToRadians(180.f + GetLinkedPortal()->GetPortalDir()));
	}
	else
	{
		yRotDiff = XMMatrixRotationAxis(XMLoadFloat3(&portalT->GetUp()), XMConvertToRadians(GetLinkedPortal()->GetPortalDir()));
	}
	relativeRot = XMVector4Transform(relativeRot, yRotDiff);
	m_pPortalCamera->GetTransform()->Rotate(relativeRotIn * relativeRot);

	//Translate Camera according to main camera in portal space
	XMVECTOR relativePos = (relativePosCam - relativePosOut);
	//Calculate rotation offset

	XMMATRIX yRotDiff2{};
	if(static_cast<int>(std::roundf(GetLinkedPortal()->GetPortalDir())) % 180 == 0)
	{
		yRotDiff2 = XMMatrixRotationAxis(XMLoadFloat3(&portalT->GetUp()), XMConvertToRadians(180.f + GetLinkedPortal()->GetPortalDir()));
	}
	else
	{
		yRotDiff2 = XMMatrixRotationAxis(XMLoadFloat3(&portalT->GetUp()), XMConvertToRadians(GetLinkedPortal()->GetPortalDir()));
	}

	relativePos = XMVector4Transform(relativePos, yRotDiff2);

	XMFLOAT3 pos{};
	XMStoreFloat3(&pos, relativePos);
	m_pPortalCamera->GetTransform()->Translate(relativePos);

}

