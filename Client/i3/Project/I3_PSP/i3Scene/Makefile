INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_PSP_ROOT)/include -I$(I3SCENE_INC)

TARGET		=	$(OUTDIR)/libi3Scene.a
OBJS		=	i3Node.o i3Transform.o i3Camera.o i3Geometry.o i3LODSelector.o i3NodeSelector.o i3AttrSet.o \
				i3Joint.o i3LightSet.o i3SceneObject.o i3OverrideAttrSet.o \
				i3ProjectiveTextureShader.o i3ProjectiveShadowShader.o \
				i3Animation.o i3Animation2.o i3AnimationState.o i3AttrStack.o i3AttrStackManager.o i3CommonNodeProcs.o \
				i3Quad.o i3Sprite.o i3Billboard.o \
				i3NodeProcs.o i3NodeStack.o i3SceneDebug.o i3SceneFile.o i3SceneMeta.o i3SceneTracer.o \
				i3UserNode.o i3AbstractBoolAttr.o i3AlphaFuncAttr.o i3AlphaTestEnableAttr.o \
				i3BlendEnableAttr.o i3BlendModeAttr.o i3ClippingEnableAttr.o i3ColorAttr.o \
				i3EdgeAntiAliasEnableAttr.o i3Sprite2D.o i3Sprite2DAttr.o\
				i3Navigation.o i3NavigationMesh.o i3NavigationMeshNode.o i3NavigationMeshPathFinder.o \
				i3Collidee.o i3CollideeCylinder.o i3CollideeGroup.o i3CollideeLine.o \
				i3CollideeMesh.o i3CollideeSphere.o i3Collider.o i3ColliderSet.o i3CollisionContext.o i3CollisionSolver.o 				
				
OBJS		+=	i3FaceCullModeAttr.o i3FogColorAttr.o i3FogDensityAttr.o i3FogEnableAttr.o i3FogModeAttr.o i3FogNearFarAttr.o \
				i3GeometryAttr.o i3SpriteAttr.o i3BillboardAttr.o i3QuadAttr.o \
				i3LightAttr.o i3LightingEnableAttr.o i3TextureCoordOffsetAttr.o \
				i3MaterialAttr.o i3ModelViewMatrixAttr.o \
				i3PointSpriteAttr.o i3ProjectMatrixAttr.o \
				i3RenderAttr.o i3RenderAttrListAttr.o i3RenderModeAttr.o \
				i3ShadeModeAttr.o i3StencilFuncAttr.o i3StencilOperationAttr.o i3StencilTestEnableAttr.o \
				i3TextureBindAttr.o i3TextureEnableAttr.o i3TextureFilterAttr.o i3TextureWrapAttr.o i3TextureCoordScaleAttr.o \
				i3TextureFunctionAttr.o i3TextureMatrixAttr.o i3TextureCoordSourceAttr.o \
				i3VertexBlendEnableAttr.o i3VertexBlendMatrixListAttr.o i3VertexColorEnableAttr.o i3ViewMatrixAttr.o \
				i3ZFuncAttr.o i3ZTestEnableAttr.o i3ZWriteEnableAttr.o \
				i3RenderTargetAttr.o i3ClearRenderTargetAttr.o i3GPUCommandListAttr.o \
				i3SceneUtil.o i3SphereEnvironmentMapShader.o i3TimeSequence.o i3Particle.o i3Font.o i3TextAttr.o i3TimeSequenceInfo.o
				
OBJS		+=	i3TextNode.o \
				i3Navigation.o i3NavigationMesh.o i3NavigationMeshNode.o i3NavigationMeshPathFinder.o i3NavigationPoint.o \
				i3NavigationPointPathFinder.o

include		../Common/makebase

vpath %.cpp		$(I3SCENE_SRC)

