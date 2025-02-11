#pragma once

#include "UniformDataStorage.h"
#include "ChunkBasedUniforms.h"
#include "../Maths/DualQuaternion.h"
#include "../common/Macros.h"
#include <map>
#include <string>

class DescriptorSetLayout;
class DescriptorSet;
class GlobalTextures;
class Mesh;
class SkeletonAnimationInstance;

const static uint32_t SSAO_SAMPLE_COUNT = 64;

typedef struct _GlobalVariables
{
	// Camera settings
	Matrix4f	projectionMatrix;
	Matrix4f	prevProjectionMatrix;

	// Windows settings
	Vector4f	gameWindowSize;
	Vector4f	envGenWindowSize;
	Vector4f	shadowGenWindowSize;
	Vector4f	SSAOSSRWindowSize;
	Vector4f	bloomWindowSize;
	Vector4f	motionTileWindowSize;	// xy: tile size, zw: window size

	/*******************************************************************
	* DESCRIPTION: Main directional light direction
	*
	* XYZ: main light direction
	* W: Reserved
	*/
	Vector4f	mainLightDir;

	/*******************************************************************
	* DESCRIPTION: Main directional light rgb
	*
	* XYZ: main light color
	* W: Reserved
	*/
	Vector4f	mainLightColor;

	/*******************************************************************
	* DESCRIPTION: Main directional light vpn matrix
	*/
	Matrix4f	mainLightVP;

	/*******************************************************************
	* DESCRIPTION: Camera parameters
	*
	* X: Aspect
	* Y: Film width in meters
	* Z: Film height in meters
	* W: Focal length in meters
	*/
	Vector4f mainCameraSettings0;

	/*******************************************************************
	* DESCRIPTION: Camera parameters
	*
	* X: Focus distance in meters
	* Y: FStop
	* Z: Shutter speed in seconds
	* W: IOS
	*/
	Vector4f mainCameraSettings1;

	/*******************************************************************
	* DESCRIPTION: Camera parameters
	*
	* X: Far plane in meters
	* Y: Horizontal FOV in radius
	* Z: Vertical FOV in radius
	* W: Aperture diameter in meters
	*/
	Vector4f mainCameraSettings2;

	/*******************************************************************
	* DESCRIPTION: Parameters for tone mapping
	*
	* X: Gamma
	* Y: Exposure
	* Z: White scale
	* W: Reserved
	*/
	Vector4f	GEW;

	/*******************************************************************
	* DESCRIPTION: Parameters for stochastic screen space reflection
	*
	* X: BRDFBias
	* Y: SSR mip toggle
	* Z: Sample normal max regen count
	* W: How near reflected vector to surface tangent that normal has to be resampled
	*/
	Vector4f	SSRSettings0;

	/*******************************************************************
	* DESCRIPTION: Parameters for stochastic screen space reflection
	*
	* X: Pixel stride for screen space ray trace
	* Y: Init offset at the beginning of ray trace
	* Z: Max count of ray trace steps
	* W: Thickness of a surface that you can consider it a hit
	*/
	Vector4f	SSRSettings1;

	/*******************************************************************
	* DESCRIPTION: Parameters for stochastic screen space reflection
	*
	* X: How far a hit to the edge of screen that it needs to be fade, to prevent from hard boundary
	* Y: How many steps that a hit starts to fade, to prevent from hard boundary
	* Z: Screen sized mipmap level count
	* W: Reserved
	*/
	Vector4f	SSRSettings2;

	/*******************************************************************
	* DESCRIPTION: Parameters for temporal filter
	*
	* X: Motion impact lower bound
	* Y: Motion impact upper bound
	* Z: The portion of high response SSR in a moving region, to slightly reduce ssr noise
	* W: Reserved
	*/
	Vector4f    TemporalSettings0;

	/*******************************************************************
	* DESCRIPTION: Parameters for bloom
	*
	* X: Bloom intensity clamping lower bound
	* Y: Bloom intensity clamping upper bound
	* Z: Upsample scale
	* W: Reserved
	*/
	Vector4f    BloomSettings0;

	/*******************************************************************
	* DESCRIPTION: Parameters for bloom
	*
	* X: Bloom amplify when combine back
	* Y: Bloom slope(pow(bloom, slope)), e.g. 1 stands for linear
	* Z: Reserved
	* W: Reserved
	*/
	Vector4f    BloomSettings1;

	/*******************************************************************
	* DESCRIPTION: Parameters for bloom
	*
	* X: Max size of circle of confusion(COC), measured by texture coordinate(0, 1)
	* Y: Recipe of X
	* Z: COC coefficient: f^2 / (N * (S1 - f) * film_width * 2), divide by 2 means from diameter to radius, divide by film width obtains coc size in texture coordinate(0, 1)
	* W: Reserved
	*/
	Vector4f	DOFSettings0;

	/*******************************************************************
	* DESCRIPTION: Parameters for motion blur
	*
	* X: Motion blur amplify factor, to enhance or reduce motion blur effect
	* Y: Motion blur sample count
	* Z: Reserved
	* W: Reserved
	*/
	Vector4f	MotionBlurSettings;

	/*******************************************************************
	* DESCRIPTION: Parameters for vignette
	*
	* X: Vignette min distance
	* Y: Vignette max distance
	* Z: Vignette amplify factor
	* W: Reserved
	*/
	Vector4f	VignetteSettings;

	// SSAO settings
	Vector4f	SSAOSamples[SSAO_SAMPLE_COUNT];
}GlobalVariables;

class GlobalUniforms : public UniformDataStorage
{
public:
	void SetProjectionMatrix(const Matrix4f& proj);
	Matrix4f GetProjectionMatrix() const { return m_globalVariables.projectionMatrix; }

	void SetGameWindowSize(const Vector2f& size);
	Vector2f GetGameWindowSize() const { return { m_globalVariables.gameWindowSize.x, m_globalVariables.gameWindowSize.y }; }
	void SetEnvGenWindowSize(const Vector2f& size);
	Vector2f GetEnvGenWindowSize() const { return { m_globalVariables.envGenWindowSize.x, m_globalVariables.envGenWindowSize.y }; }
	void SetShadowGenWindowSize(const Vector2f& size);
	Vector2f GetShadowGenWindowSize() const { return { m_globalVariables.shadowGenWindowSize.x, m_globalVariables.shadowGenWindowSize.y }; }
	void SetSSAOSSRWindowSize(const Vector2f& size);
	Vector2f GetSSAOSSRWindowSize() const { return { m_globalVariables.SSAOSSRWindowSize.x, m_globalVariables.SSAOSSRWindowSize.y }; }
	void SetBloomWindowSize(const Vector2f& size);
	Vector2f GetBloomWindowSize() const { return { m_globalVariables.bloomWindowSize.x, m_globalVariables.bloomWindowSize.y }; }
	void SetMotionTileSize(const Vector2f& size);
	Vector2f GetMotionTileSize() const { return { m_globalVariables.motionTileWindowSize.x, m_globalVariables.motionTileWindowSize.y }; }
	Vector2f GetMotionTileWindowSize() const { return { m_globalVariables.motionTileWindowSize.z, m_globalVariables.motionTileWindowSize.w }; }

	void SetMainLightDir(const Vector3f& dir);
	Vector4f GetMainLightDir() const { return m_globalVariables.mainLightDir; }
	void SetMainLightColor(const Vector3f& color);
	Vector4f GetMainLightColor() const { return m_globalVariables.mainLightColor; }
	void SetMainLightVP(const Matrix4f& vp);
	Matrix4f GetmainLightVP() const { return m_globalVariables.mainLightVP; }

	void SetMainCameraSettings0(const Vector4f& settings);
	Vector4f GetMainCameraSettings0() const { return m_globalVariables.mainCameraSettings0; }
	void SetMainCameraSettings1(const Vector4f& settings);
	Vector4f GetMainCameraSettings1() const { return m_globalVariables.mainCameraSettings1; }
	void SetMainCameraSettings2(const Vector4f& settings);
	Vector4f GetMainCameraSettings2() const { return m_globalVariables.mainCameraSettings2; }
	void SetMainCameraAspect(float aspect);
	float GetMainCameraAspect() const { return m_globalVariables.mainCameraSettings0.x; }
	void SetMainCameraFilmWidth(float filmWidth);
	float GetMainCameraFilmWidth() const { return m_globalVariables.mainCameraSettings0.y; }
	void SetMainCameraFilmHeight(float filmHeight);
	float GetMainCameraFilmHeight() const { return m_globalVariables.mainCameraSettings0.z; }
	void SetMainCameraFocalLength(float focalLength);
	float GetMainCameraFocalLength() const { return m_globalVariables.mainCameraSettings0.w; }
	void SetMainCameraFocusDistance(float focusDistance);
	float GetMainCameraFocusDistance() const { return m_globalVariables.mainCameraSettings1.x; }
	void SetMainCameraFStop(float fstop);
	float GetMainCameraFStop() const { return m_globalVariables.mainCameraSettings1.y; }
	void SetMainCameraShutterSpeed(float shutterSpeed);
	float GetMainCameraShutterSpeed() const { return m_globalVariables.mainCameraSettings1.z; }
	void SetMainCameraISO(float ISO);
	float GetMainCameraISO() const { return m_globalVariables.mainCameraSettings1.w; }
	void SetMainCameraFarPlane(float farPlane);
	float GetMainCameraFarPlane() const { return m_globalVariables.mainCameraSettings2.x; }
	void SetMainCameraHorizontalFOV(float horizontalFOV);
	float GetMainCameraHorizontalFOV() const { return m_globalVariables.mainCameraSettings2.y; }
	void SetMainCameraVerticalFOV(float verticalFOV);
	float GetMainCameraVerticalFOV() const { return m_globalVariables.mainCameraSettings2.z; }
	void SetMainCameraApertureDiameter(float apertureDiameter);
	float GetMainCameraApertureDiameter() const { return m_globalVariables.mainCameraSettings2.w; }

	void SetRenderSettings(const Vector4f& setting);
	Vector4f GetRenderSettings() const { return m_globalVariables.GEW; }

	void SetSSRSettings0(const Vector4f& setting);
	Vector4f GetSSRSettings0() const { return m_globalVariables.SSRSettings0; }
	void SetSSRSettings1(const Vector4f& setting);
	Vector4f GetSSRSettings1() const { return m_globalVariables.SSRSettings1; }
	void SetSSRSettings2(const Vector4f& setting);
	Vector4f GetSSRSettings2() const { return m_globalVariables.SSRSettings2; }
	void SetBRDFBias(float BRDFBias);
	float GetBRDFBias() const { return m_globalVariables.SSRSettings0.x; }
	void SetSSRMip(float SSRMip);
	float GetSSRMip() const { return m_globalVariables.SSRSettings0.y; }
	void SetSampleNormalRegenCount(float count);
	float GetSampleNormalRegenCount() const { return m_globalVariables.SSRSettings0.z; }
	void SetSampleNormalRegenMargin(float margin);
	float GetSampleNormalRegenMargin() const { return m_globalVariables.SSRSettings0.w; }
	void SetSSRTStride(float stride);
	float GetSSRTStride() const { return m_globalVariables.SSRSettings1.x; }
	void SetSSRTInitOffset(float offset);
	float GetSSRTInitOffset() const { return m_globalVariables.SSRSettings1.y; }
	void SetMaxSSRTStepCount(float count);
	float GetMaxSSRTStepCount() const { return m_globalVariables.SSRSettings1.z; }
	void SetSSRTThickness(float thickness);
	float GetSSRTThickness() const { return m_globalVariables.SSRSettings1.w; }
	void SetSSRTBorderFadingDist(float dist);
	float GetSSRTBorderFadingDist() const { return m_globalVariables.SSRSettings2.x; }
	void SetSSRTStepCountFadingDist(float dist);
	float GetSSRTStepCountFadingDist() const { return m_globalVariables.SSRSettings2.y; }
	void SetScreenSizeMipLevel(float mipLevel);
	float GetScreenSizeMipLevel() const { return m_globalVariables.SSRSettings2.z; }

	void SetTemporalSettings0(const Vector4f& setting);
	Vector4f GetTemporalSettings0() const { return m_globalVariables.TemporalSettings0; }
	void SetMotionImpactLowerBound(float motionImpactLowerBound);
	float GetMotionImpactLowerBound() const { return m_globalVariables.TemporalSettings0.x; }
	void SetMotionImpactUpperBound(float motionImpactUpperBound);
	float GetMotionImpactUpperBound() const { return m_globalVariables.TemporalSettings0.y; }
	void SetHighResponseSSRPortion(float highResponseSSRPortion);
	float GetHighResponseSSRPortion() const { return m_globalVariables.TemporalSettings0.z; }

	void SetBloomSettings0(const Vector4f& setting);
	Vector4f GetBloomSettings0() const { return m_globalVariables.BloomSettings0; }
	void SetBloomSettings1(const Vector4f& setting);
	Vector4f GetBloomSettings1() const { return m_globalVariables.BloomSettings1; }
	void SetBloomClampingLowerBound(float lowerBound);
	float GetBloomClampingLowerBound() const { return m_globalVariables.BloomSettings0.x; }
	void SetBloomClampingUpperBound(float upperBound);
	float GetBloomClampingUpperBound() const { return m_globalVariables.BloomSettings0.y; }
	void SetUpsampleScale(float upsampleScale);
	float GetUpsampleScale() const { return m_globalVariables.BloomSettings0.z; }
	void SetBloomAmplify(float bloomAmplify);
	float GetBloomAmplify() const { return m_globalVariables.BloomSettings1.x; }
	void SetBloomSlope(float bloomSlope);
	float GetBloomSlope() const { return m_globalVariables.BloomSettings1.y; }

	void SetMaxCOC(float maxCOC);
	float GetMaxCOC() const { return m_globalVariables.DOFSettings0.x; }
	float GetRcpMaxCOC() const { return m_globalVariables.DOFSettings0.y; }
	float GetCOCCoeff() const { return m_globalVariables.DOFSettings0.z; }

	void SetMotionBlurSettings(const Vector4f& settings);
	void SetMotionBlurAmplify(float motionBlurAmplify);
	float GetMotionBlurAmplify() const { return m_globalVariables.MotionBlurSettings.x; }
	void SetMotionBlurSampleCount(uint32_t sampleCount);
	uint32_t GetMotionBlurSampleCount() const { return (uint32_t)m_globalVariables.MotionBlurSettings.y; }

	void SetVignetteSettings(const Vector4f& settings);
	void SetVignetteMinDist(float minDist);
	float GetVignetteMinDist() const { return m_globalVariables.VignetteSettings.x; }
	void SetVignetteMaxDist(float maxDist);
	float GetVignetteMaxDist() const { return m_globalVariables.VignetteSettings.y; }
	void SetVignetteAmplify(float vignetteAmplify);
	float GetVignetteAmplify() const { return m_globalVariables.VignetteSettings.z; }

public:
	bool Init(const std::shared_ptr<GlobalUniforms>& pSelf);
	static std::shared_ptr<GlobalUniforms> Create();

	std::vector<UniformVarList> PrepareUniformVarList() const override;
	uint32_t SetupDescriptorSet(const std::shared_ptr<DescriptorSet>& pDescriptorSet, uint32_t bindingIndex) const override;

protected:
	void UpdateUniformDataInternal() override;
	void SetDirtyInternal() override;
	const void* AcquireDataPtr() const override { return &m_globalVariables; }
	uint32_t AcquireDataSize() const override { return sizeof(GlobalVariables); }

	void InitSSAORandomSample();

protected:
	GlobalVariables							m_globalVariables;
};

typedef struct _BoneData
{
	DualQuaternionf	currBoneOffsetDQ;
	DualQuaternionf	prevBoneOffsetDQ;
}BoneData;

class BoneIndirectUniform;

class PerBoneUniforms : public ChunkBasedUniforms
{
protected:
	bool Init(const std::shared_ptr<PerBoneUniforms>& pSelf);

public:
	static std::shared_ptr<PerBoneUniforms> Create();

public:
	std::vector<UniformVarList> PrepareUniformVarList() const override;
	uint32_t SetupDescriptorSet(const std::shared_ptr<DescriptorSet>& pDescriptorSet, uint32_t bindingIndex) const override;

protected:
	void SetBoneOffsetTransform(uint32_t chunkIndex, const DualQuaternionf& offsetDQ);
	DualQuaternionf GetBoneOffsetTransform(uint32_t chunkIndex) const;

protected:
	void UpdateDirtyChunkInternal(uint32_t index) override;
	const void* AcquireDataPtr() const override { return &m_boneData[0]; }
	uint32_t AcquireDataSize() const override { return sizeof(m_boneData); }

protected:
	BoneData	m_boneData[MAXIMUM_OBJECTS];

	friend class BoneIndirectUniform;
};

class Mesh;
class SkeletonAnimationInstance;
class AnimationController;

class BoneIndirectUniform : public ChunkBasedUniforms
{
public:
	typedef std::map<std::wstring, uint32_t>	BoneIndexLookupTable;

protected:
	bool Init(const std::shared_ptr<BoneIndirectUniform>& pSelf, uint32_t type);

public:
	static std::shared_ptr<BoneIndirectUniform> Create(uint32_t type);
	uint32_t AllocatePerObjectChunk() override { ASSERTION(false); return -1; }
	uint32_t AllocateConsecutiveChunks(uint32_t chunkSize) override;

	bool GetBoneIndex(uint32_t chunkIndex, const std::wstring& boneName, uint32_t& outBoneIndex) const;
	bool GetBoneCount(uint32_t chunkIndex, uint32_t& outBoneCount) const;

	std::wstring GetBoneName(uint32_t chunkIndex, uint32_t index) const;

	// Disable the visibility of these access functions, since meshChunkIndex is something internal only within mesh
	// Let specific mesh to deal with these functions and make wrappers of them
protected:
	// Bone index automatically generated if not exists
	void SetBoneTransform(uint32_t chunkIndex, const std::wstring& boneName, const DualQuaternionf& offsetDQ);
	bool GetBoneTransform(uint32_t chunkIndex, const std::wstring& boneName, DualQuaternionf& outBoneOffsetTransformDQ) const;

	// Input bone index
	void SetBoneTransform(uint32_t chunkIndex, const std::wstring& boneName, uint32_t boneIndex, const DualQuaternionf& offsetDQ);
	bool GetBoneTransform(uint32_t chunkIndex, const std::wstring& boneName, uint32_t boneIndex, DualQuaternionf& outBoneOffsetTransformDQ) const;

public:
	std::vector<UniformVarList> PrepareUniformVarList() const override;
	uint32_t SetupDescriptorSet(const std::shared_ptr<DescriptorSet>& pDescriptorSet, uint32_t bindingIndex) const override;

protected:
	void UpdateDirtyChunkInternal(uint32_t index) override;
	const void* AcquireDataPtr() const override { return &m_boneChunkIndex[0]; }
	uint32_t AcquireDataSize() const override { return sizeof(m_boneChunkIndex); }

protected:
	uint32_t									m_boneChunkIndex[MAXIMUM_OBJECTS];
	// index stands for instance chunk index of a set of bones
	std::map<uint32_t, BoneIndexLookupTable>	m_boneIndexLookupTables;

	uint32_t									m_boneBufferType;			// Which binding bone data buffer is located

	friend class Mesh;
	friend class SkeletonAnimationInstance;
	friend class AnimationController;
};

class PerMeshUniforms : public ChunkBasedUniforms
{
	typedef struct _MeshData
	{
		uint32_t	boneChunkIndexOffset;
	}MeshData;

protected:
	bool Init(const std::shared_ptr<PerMeshUniforms>& pSelf);

public:
	static std::shared_ptr<PerMeshUniforms> Create();

public:
	std::vector<UniformVarList> PrepareUniformVarList() const override;
	uint32_t SetupDescriptorSet(const std::shared_ptr<DescriptorSet>& pDescriptorSet, uint32_t bindingIndex) const override;

protected:
	void SetBoneChunkIndexOffset(uint32_t chunkIndex, uint32_t boneChunkIndexOffset);
	uint32_t GetBoneChunkIndexOffset(uint32_t chunkIndex) const { return m_meshData[chunkIndex].boneChunkIndexOffset; }

protected:
	void UpdateDirtyChunkInternal(uint32_t index) override;
	const void* AcquireDataPtr() const override { return &m_meshData[0]; }
	uint32_t AcquireDataSize() const override { return sizeof(m_meshData); }

protected:
	MeshData	m_meshData[MAXIMUM_OBJECTS];

	friend class Mesh;
};

class PerAnimationUniforms : public ChunkBasedUniforms
{
	typedef struct _AnimationData
	{
		uint32_t	boneChunkIndexOffset;
	}AnimationData;

protected:
	bool Init(const std::shared_ptr<PerAnimationUniforms>& pSelf);

public:
	static std::shared_ptr<PerAnimationUniforms> Create();

public:
	std::vector<UniformVarList> PrepareUniformVarList() const override;
	uint32_t SetupDescriptorSet(const std::shared_ptr<DescriptorSet>& pDescriptorSet, uint32_t bindingIndex) const override;

protected:
	void SetBoneChunkIndexOffset(uint32_t chunkIndex, uint32_t boneChunkIndexOffset);
	uint32_t GetBoneChunkIndexOffset(uint32_t chunkIndex) const { return m_animationData[chunkIndex].boneChunkIndexOffset; }

protected:
	void UpdateDirtyChunkInternal(uint32_t index) override;
	const void* AcquireDataPtr() const override { return &m_animationData[0]; }
	uint32_t AcquireDataSize() const override { return sizeof(m_animationData); }

protected:
	AnimationData	m_animationData[MAXIMUM_OBJECTS];

	friend class SkeletonAnimationInstance;
};