#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	TODO_W7_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.duration);

		//2. 
		//IF m_Reversed is true
		if(m_Reversed)
		{
			//	Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			if(m_TickCount < 0)
			{
				m_TickCount += m_CurrentClip.duration;
			}
		}
		else
		{
			//ELSE
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;
			//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
			if(m_TickCount > m_CurrentClip.duration)
			{
				m_TickCount -= m_CurrentClip.duration;
			}
		}


		//3.
		//Find the enclosing keys
		AnimationKey keyA{}, keyB{};
		//Iterate all the keys of the clip and find the following keys:
		for (int i = 0; i < m_CurrentClip.keys.size(); ++i)
		{
			auto currentKey = m_CurrentClip.keys[i];
			
			if (currentKey.tick < m_TickCount)
			{
				//keyA > Closest Key with Tick before/smaller than m_TickCount
				float currentDifference = m_TickCount - currentKey.tick;
				float oldDifference = m_TickCount - keyA.tick;
				if(currentDifference <= oldDifference)
				{
					keyA = currentKey;
				}
			}
			else
			{
				//keyB > Closest Key with Tick after/bigger than m_TickCount
				float currentDifference = m_TickCount - currentKey.tick;
				float oldDifference = m_TickCount - keyB.tick;
				if (currentDifference <= oldDifference)
				{
					keyB = currentKey;
				}
			}
		}


		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		float blendFactor = (m_TickCount - keyA.tick) * (100) / (keyB.tick - keyA.tick);
		blendFactor /= 100;

		//Clear the m_Transforms vector
		m_Transforms.clear();
		for(int i =0; i < m_CurrentClip.keys[0].boneTransforms.size(); ++i)
		{
			//FOR every boneTransform in a key (So for every bone)
			//	Retrieve the transform from keyA (transformA)
			auto aTrans = keyA.boneTransforms[i];
			// 	Retrieve the transform from keyB (transformB)
			auto bTrans = keyB.boneTransforms[i];
			//	Decompose both transforms
			XMVECTOR aScale;
			XMVECTOR aPos;
			XMVECTOR aRot;
			XMMatrixDecompose(&aScale, &aRot, &aPos, XMLoadFloat4x4(&aTrans));

			XMVECTOR bScale;
			XMVECTOR bPos;
			XMVECTOR bRot;
			XMMatrixDecompose(&bScale, &bRot, &bPos, XMLoadFloat4x4(&bTrans));
			//	Lerp between all the transformations (Position, Scale, Rotation)
			auto lerpScale = XMVectorLerp(aScale, bScale, blendFactor);
			auto lerpRot = XMQuaternionSlerp(aRot, bRot, blendFactor);
			auto lerpPos = XMVectorLerp(aPos, bPos, blendFactor);

			//	Compose a transformation matrix with the lerp-result
			XMMATRIX xmTrans = XMMatrixIdentity();
			xmTrans = XMMatrixTransformation(XMVECTOR{ 0, 0, 0 }, 
				XMVECTOR{ 0, 0, 0, 0 }, lerpScale, 
				XMVECTOR{ 0, 0, 0 }, lerpRot, lerpPos);
			//	Add the resulting matrix to the m_Transforms vector

			XMFLOAT4X4 finalTrans{};
			XMStoreFloat4x4(&finalTrans, xmTrans);
			m_Transforms.push_back(finalTrans);
		}

	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	m_ClipSet = false;
	//Set m_ClipSet to false

	for(unsigned int i = 0; i < GetClipCount(); ++i)
	{
		if(GetClip(i).name == clipName)
		{
			SetAnimation(GetClip(i));
		}
		else
		{
			Reset(false);
			Logger::LogWarning(L"No clip with clipname found");
		}
	}
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	m_ClipSet = false;

	if(clipNumber < GetClipCount())
	{
		SetAnimation(GetClip(clipNumber));
	}
	else
	{
		Reset(false);
		Logger::LogWarning(L"clipNumber not in clipCount");
		return;
	}
	//Set m_ClipSet to false
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	//If not,
		//	Call Reset
		//	Log a warning with an appropriate message
		//	return
	//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	if (pause)
		m_IsPlaying = false;

	//Set m_TickCount to zero
	m_TickCount = 0;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;

	if(m_ClipSet)
	{
		auto transforms = m_CurrentClip.keys[0].boneTransforms;
		m_Transforms.assign(transforms.begin(), transforms.end());
	}
	else
	{
		XMFLOAT4X4 idMatrix = (XMFLOAT4X4)0;
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, idMatrix);
	}
	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
}
