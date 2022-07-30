#include "stdafx.h"
#include "Character.h"

#include "PortalGun.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include <Materials\Shadow\DiffuseMaterial_Shadow_Skinned.h>

Character::Character(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

Character::~Character()
{
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		delete[] m_ClipNames[i];
	}

	delete[] m_ClipNames;
}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	pCamera->GetTransform()->Translate(0.f, m_CharacterDesc.controller.height , 0.f);

	const auto pPortalGun = AddChild(new PortalGun());
	pPortalGun->GetTransform()->Translate(2.5f, 3.5f, 5.f);
	pPortalGun->GetTransform()->Rotate(0.f, 180.f, 0.f);



	m_pCrossHairTexture = ContentManager::Load<TextureData>(L"Textures/Crosshair.png");

	m_pChellObject = AddChild(new GameObject());
	const auto pChell = m_pChellObject->AddComponent(new ModelComponent(L"Meshes/Chelly.ovm"));
	const auto pSkinnedMaterialTorso = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pSkinnedMaterialLegs = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pSkinnedMaterialHead = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pSkinnedMaterialHair = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	const auto pSkinnedMaterialEye = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterialTorso->SetDiffuseTexture(L"Textures/chell_torso_diffuse.png");
	pSkinnedMaterialLegs->SetDiffuseTexture(L"Textures/chell_legs_diffuse.png");
	pSkinnedMaterialHair->SetDiffuseTexture(L"Textures/chell_hair.png");
	pSkinnedMaterialHead->SetDiffuseTexture(L"Textures/chell_head_diffuse.png");
	pSkinnedMaterialEye->SetDiffuseTexture(L"Textures/eyeball_l.png");

	pChell->SetMaterial(pSkinnedMaterialHead, 0);
	pChell->SetMaterial(pSkinnedMaterialHair, 5);

	pChell->SetMaterial(pSkinnedMaterialTorso, 3);

	pChell->SetMaterial(pSkinnedMaterialLegs, 4);

	pChell->SetMaterial(pSkinnedMaterialEye, 1);

	pChell->SetMaterial(pSkinnedMaterialEye, 2);

	m_pAnimator = pChell->GetAnimator();
	if (m_pAnimator)
	{
		m_pAnimator->SetAnimation(0);
		//m_pAnimator->SetAnimationSpeed();

		//Gather Clip Names
		m_ClipCount = m_pAnimator->GetClipCount();
		m_ClipNames = new char* [m_ClipCount];
		for (UINT i{ 0 }; i < m_ClipCount; ++i)
		{
			auto clipName = StringUtil::utf8_encode(m_pAnimator->GetClip(static_cast<int>(i)).name);
			const auto clipSize = clipName.size();
			m_ClipNames[i] = new char[clipSize + 1];
			strncpy_s(m_ClipNames[i], clipSize + 1, clipName.c_str(), clipSize);
		}
		m_pAnimator->Play();
	}
	m_pChellObject->GetTransform()->Translate(m_pControllerComponent->GetFootPosition());
	m_pChellObject->GetTransform()->Scale(0.00125f);
}

void Character::Update(const SceneContext& sceneContext)
{
	if (m_pCameraComponent->IsActive())
	{
		//constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero

		//***************
		//HANDLE INPUT

		//## Input Gathering (move)
		XMFLOAT2 move{}; //Uncomment
		//move.y should contain a 1 (Forward) or -1 (Backward) based on the active input (check corresponding actionId in m_CharacterDesc)
		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		m_pAnimator->Pause();
		if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward))
		{
			move.y = 1;
			m_pAnimator->Play();
		}
		else if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
		{
			move.y = -1;
			m_pAnimator->Play();
		}

		//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)
		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
		{
			move.x = -1;
			m_pAnimator->Play();
		}
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
		{
			move.x = 1;
			m_pAnimator->Play();
		}

		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f }; //Uncomment
		//Only if the Left Mouse Button is Down >
			// Store the MouseMovement in the local 'look' variable (cast is required)
		//Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look

		auto mouseMove = sceneContext.pInput->GetMouseMovement();
		look = XMFLOAT2{ float(mouseMove.x), float(mouseMove.y) };


		//************************
		//GATHERING TRANSFORM INFO
		//Retrieve the TransformComponent
		auto tranform = GetTransform();
		//Retrieve the forward & right vector (as XMVECTOR) from the TransformComponent
		XMVECTOR forward = XMLoadFloat3(&tranform->GetForward());
		XMVECTOR right = XMLoadFloat3(&tranform->GetRight());

		//***************
		//CAMERA ROTATION
		//Adjust the TotalYaw (m_TotalYaw) & TotalPitch (m_TotalPitch) based on the local 'look' variable
		m_TotalYaw += look.x * sceneContext.pGameTime->GetElapsed() * m_CharacterDesc.rotationSpeed;
		m_TotalPitch += look.y * sceneContext.pGameTime->GetElapsed() * m_CharacterDesc.rotationSpeed;

		m_TotalPitch = PxClamp(m_TotalPitch, m_MinPitch, m_MaxPitch);

		m_TotalYaw -= m_YOffsetAngle;
		m_YOffsetAngle = 0.f;

		//Make sure this calculated on a framerate independent way and uses CharacterDesc::rotationSpeed.
		//Rotate this character based on the TotalPitch (X) and TotalYaw (Y)
		GetTransform()->Rotate(m_TotalPitch , m_TotalYaw, 0.f);
		m_pChellObject->GetTransform()->Rotate(0.f, 180.f, 0.f);

		//********
		//MOVEMENT
		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
		auto moveAcceleration = m_MoveAcceleration * sceneContext.pGameTime->GetElapsed();

		//If the character is moving (= input is pressed)
		if (move.x != 0 || move.y != 0)
		{
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
			XMStoreFloat3(&m_CurrentDirection, (forward * move.y) + (right * move.x));
			//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed += moveAcceleration;
			//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
			m_MoveSpeed = PxClamp(m_MoveSpeed, 0.f, m_CharacterDesc.maxMoveSpeed);
		}
		//Else (character is not moving, or stopped moving)
		else
		{
			//Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed -= moveAcceleration;
			//Make sure the current MoveSpeed doesn't get smaller than zero
			m_MoveSpeed = PxClamp(m_MoveSpeed, 0.f, m_CharacterDesc.maxMoveSpeed);
		}

		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		m_TotalVelocity.x = m_CurrentDirection.x * m_MoveSpeed;
		m_TotalVelocity.z = m_CurrentDirection.z * m_MoveSpeed;

		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)

		//## Vertical Movement (Jump/Fall)
		//If the Controller Component is NOT grounded (= freefall)
		auto collisionFlags = m_pControllerComponent->GetCollisionFlags();

		if(!collisionFlags.isSet(PxControllerCollisionFlag::Enum::eCOLLISION_DOWN))
		{
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			m_TotalVelocity.y -= m_FallAcceleration * sceneContext.pGameTime->GetElapsed();
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
			if(m_TotalVelocity.y < -m_CharacterDesc.maxFallSpeed)
			{
				m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
			}
		}
		//Else If the jump action is triggered
		else if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		{
			//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
		}
		//Else (=Character is grounded, no input pressed)
		else
		{
			//m_TotalVelocity.y is zero
			m_TotalVelocity.y = 0;
		}

		//************
		//DISPLACEMENT

		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		//Calculate the displacement (m) for the current frame and move the ControllerComponent
		XMFLOAT3 displacement{};
		XMStoreFloat3(&displacement, XMVectorScale(XMLoadFloat3(&m_TotalVelocity), sceneContext.pGameTime->GetElapsed()));
		m_pControllerComponent->Move(displacement);
		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)
	}
	SpriteRenderer::Get()->AppendSprite(m_pCrossHairTexture, { sceneContext.windowWidth/2.f, sceneContext.windowHeight / 2.f }, XMFLOAT4{ Colors::Black }, { 0.5f,0.5f });
}

void Character::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}
