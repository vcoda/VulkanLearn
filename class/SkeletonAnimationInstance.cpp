#include "SkeletonAnimationInstance.h"
#include "UniformData.h"
#include "../Maths/AssimpDataConverter.h"
#include "../common/Macros.h"
#include "Mesh.h"

bool SkeletonAnimationInstance::Init(const std::shared_ptr<SkeletonAnimationInstance>& pSelf, const std::shared_ptr<SkeletonAnimation>& pSkeletonAnimation, const std::shared_ptr<Mesh>& pMesh)
{
	if (!SelfRefBase<SkeletonAnimationInstance>::Init(pSelf))
		return false;

	// No animation of input mesh? quit
	if (!pMesh->ContainBoneData())
		return false;

	m_pSkeletonAnimation = pSkeletonAnimation;
	m_pMesh = pMesh;

	uint32_t boneCount;
	ASSERTION(UniformData::GetInstance()->GetPerBoneIndirectUniforms()->GetBoneCount(pMesh->GetMeshBoneChunkIndexOffset(), boneCount));

	m_animationChunk = UniformData::GetInstance()->GetPerAnimationUniforms()->AllocatePerObjectChunk();
	m_boneChunkIndexOffset = UniformData::GetInstance()->GetPerFrameBoneIndirectUniforms()->AllocateConsecutiveChunks(boneCount);

	UniformData::GetInstance()->GetPerAnimationUniforms()->SetBoneChunkIndexOffset(m_animationChunk, m_boneChunkIndexOffset);
	
	for (uint32_t i = 0; i < m_pMesh->GetBoneCount(); i++)
	{
		const std::wstring& boneName = UniformData::GetInstance()->GetPerBoneIndirectUniforms()->GetBoneName(m_pMesh->GetMeshBoneChunkIndexOffset(), i);
		UniformData::GetInstance()->GetPerFrameBoneIndirectUniforms()->SetBoneTransform(m_boneChunkIndexOffset, boneName, i, {});
	}

	return true;
}

std::shared_ptr<SkeletonAnimationInstance> SkeletonAnimationInstance::Create(const std::shared_ptr<SkeletonAnimation>& pSkeletonAnimation, const std::shared_ptr<Mesh>& pMesh)
{
	std::shared_ptr<SkeletonAnimationInstance> pSkeletonAnimationInstance = std::make_shared<SkeletonAnimationInstance>();
	if (pSkeletonAnimationInstance != nullptr && pSkeletonAnimationInstance->Init(pSkeletonAnimationInstance, pSkeletonAnimation, pMesh))
		return pSkeletonAnimationInstance;

	return nullptr;
}

void SkeletonAnimationInstance::SetBoneTransform(const std::wstring& boneName, const DualQuaternionf& dq)
{
	uint32_t boneIndex;

	// Animation bone and mesh bone doesn't match? quit
	if (!UniformData::GetInstance()->GetPerBoneIndirectUniforms()->GetBoneIndex(m_pMesh->GetMeshBoneChunkIndexOffset(), boneName, boneIndex))
		return;

	UniformData::GetInstance()->GetPerFrameBoneIndirectUniforms()->SetBoneTransform(m_boneChunkIndexOffset, boneName, boneIndex, dq);
}