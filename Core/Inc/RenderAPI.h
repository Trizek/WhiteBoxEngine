#ifndef __RENDERAPI_H__
#define __RENDERAPI_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class CApplication;

struct SVertexInfo
{
	Vec3	position;
	Vec3	normal;
	Vec3	color;
};

struct IRenderMesh : public CSmartPointerData
{
	virtual void SetCastShadow( bool bCastShadow ) = 0;
	
	virtual ~IRenderMesh(){}
};
DEFINE_SMART_PTR( IRenderMesh );

struct IRenderSceneNode : public CSmartPointerData
{
	virtual void AttachChild( IRenderSceneNode& childSceneNode ) = 0;
	virtual	void AttachMesh( IRenderMeshPtr pMesh ) = 0;
	
	virtual void SetLocalPosition( const Vec3& localPosition ) = 0;
	virtual void SetLocalRotAngles( const Vec3& localRotAngles ) = 0;	
	virtual void SetLocalScale( const Vec3& localScale ) = 0;	
	
	virtual ~IRenderSceneNode(){}
};
DEFINE_SMART_PTR( IRenderSceneNode );



struct IRenderApplication : public CSmartPointerData
{
	virtual bool	Init() = 0;
	virtual void	Run() = 0;
	virtual void	Destroy() = 0;

	virtual IRenderSceneNodePtr	CreateSceneNode() = 0;
	virtual void AttachToRoot( IRenderSceneNodePtr pSceneNode, bool bAttach )	= 0;
	
	virtual IRenderMeshPtr	CreateMesh( const String& meshName ) = 0;
	
	virtual void CreateManualMesh( const CString& meshName, const SVertexInfo* pVertices, size_t verticeCount, unsigned short* pIndices, size_t indexCount ) = 0;

	
	virtual ~IRenderApplication(){}
};
DEFINE_SMART_PTR( IRenderApplication );

struct SRenderAPI
{
	IRenderApplicationPtr	CreateRenderApplication( CApplication& application );	
};

WHITEBOX_END

#endif
