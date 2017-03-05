
// General
#define DebugMode	true
//#define	TerrainRenderMultiTask					// multitask = instance rendering, !multitask = single draw call
//#define	TerrainPolygonModeLine

// Texture
#define TextureFilterAnisotropic	false

// Perspective
#define	mNear	1.0f
#define	mFar	700.0f

#define mWidthWindow	1080
#define	mHeightWindow	720

#define	mFovy	60.0f

#define	mMouseMoveCameraFactor	2.0f

// Map
#ifndef TerrainRenderMultiTask
	#define	DetailChunk	12							// Gêstoœæ chunk'a
	#define	ChunkSize	1000.0f						// D³ugosæ pojedyñczego chunk'a
	#define	HalfNumRowChunks	1					// £¹czna iloœæ chunków - (1 + HalfNumRowChunks * 2) ^ 2
#else
	#define	DetailChunk	7
	#define	ChunkSize	600
	#define	HalfNumRowChunks	2	
#endif
#define	UpdateChunkOffsetFactor	0.3f			// po³owa rozmiaru chunk'a + to

// Shadow
#define	mNumCascadeShadow	3
#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_
const float mCascadedShadowFactorEnd = 30.0f;
const float mCascadedShadowEnd[mNumCascadeShadow + 1] = { mNear, 70.0f, 150.0f, 400.0f };
#endif

// Postprocessor varaiable
#define HalfChunkSize	(0.5f * ChunkSize)
#define	NumRowChunks	(HalfNumRowChunks * 2 + 1)
#define	NumChunks	(NumRowChunks * NumRowChunks)

//#define	GAME_EDITOR


