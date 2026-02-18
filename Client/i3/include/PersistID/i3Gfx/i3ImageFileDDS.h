#if !defined( I3G_IMAGE_FILE_DDS_H)
#define I3G_IMAGE_FILE_DDS_H

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

#if !defined( I3_WINDOWS)
/*
 * ddsCaps field is valid.
 */
#define DDSD_CAPS               0x00000001l     // default

/*
 * dwHeight field is valid.
 */
#define DDSD_HEIGHT             0x00000002l

/*
 * dwWidth field is valid.
 */
#define DDSD_WIDTH              0x00000004l

/*
 * lPitch is valid.
 */
#define DDSD_PITCH              0x00000008l

/*
 * dwBackBufferCount is valid.
 */
#define DDSD_BACKBUFFERCOUNT    0x00000020l

/*
 * dwZBufferBitDepth is valid.  (shouldnt be used in DDSURFACEDESC2)
 */
#define DDSD_ZBUFFERBITDEPTH    0x00000040l

/*
 * dwAlphaBitDepth is valid.
 */
#define DDSD_ALPHABITDEPTH      0x00000080l


/*
 * lpSurface is valid.
 */
#define DDSD_LPSURFACE          0x00000800l

/*
 * ddpfPixelFormat is valid.
 */
#define DDSD_PIXELFORMAT        0x00001000l

/*
 * ddckCKDestOverlay is valid.
 */
#define DDSD_CKDESTOVERLAY      0x00002000l

/*
 * ddckCKDestBlt is valid.
 */
#define DDSD_CKDESTBLT          0x00004000l

/*
 * ddckCKSrcOverlay is valid.
 */
#define DDSD_CKSRCOVERLAY       0x00008000l

/*
 * ddckCKSrcBlt is valid.
 */
#define DDSD_CKSRCBLT           0x00010000l

/*
 * dwMipMapCount is valid.
 */
#define DDSD_MIPMAPCOUNT        0x00020000l

 /*
  * dwRefreshRate is valid
  */
#define DDSD_REFRESHRATE        0x00040000l

/*
 * dwLinearSize is valid
 */
#define DDSD_LINEARSIZE         0x00080000l

/*
 * dwTextureStage is valid
 */
#define DDSD_TEXTURESTAGE       0x00100000l
/*
 * dwFVF is valid
 */
#define DDSD_FVF                0x00200000l
/*
 * dwSrcVBHandle is valid
 */
#define DDSD_SRCVBHANDLE        0x00400000l

/*
 * dwDepth is valid
 */
#define DDSD_DEPTH              0x00800000l

/*
 * All input fields are valid.
 */
#define DDSD_ALL                0x00fff9eel

/*
 * guid field is valid.
 */
#define DDOSD_GUID                  0x00000001l

/*
 * dwCompressionRatio field is valid.
 */
#define DDOSD_COMPRESSION_RATIO     0x00000002l

/*
 * ddSCaps field is valid.
 */
#define DDOSD_SCAPS                 0x00000004l

/*
 * ddOSCaps field is valid.
 */
#define DDOSD_OSCAPS                0x00000008l

/*
 * All input fields are valid.
 */
#define DDOSD_ALL                   0x0000000fl

/*
 * The surface's optimized pixelformat is compressed
 */
#define DDOSDCAPS_OPTCOMPRESSED                 0x00000001l

/*
 * The surface's optimized pixelformat is reordered
 */
#define DDOSDCAPS_OPTREORDERED                  0x00000002l

/*
 * The opt surface is a monolithic mipmap
 */
#define DDOSDCAPS_MONOLITHICMIPMAP              0x00000004l

/*
 * The valid Surf caps:
 * #define DDSCAPS_SYSTEMMEMORY                 0x00000800l
 * #define DDSCAPS_VIDEOMEMORY          0x00004000l
 * #define DDSCAPS_LOCALVIDMEM          0x10000000l
 * #define DDSCAPS_NONLOCALVIDMEM       0x20000000l
 */
#define DDOSDCAPS_VALIDSCAPS            0x30004800l

/*
 * The valid OptSurf caps
 */
#define DDOSDCAPS_VALIDOSCAPS           0x00000007l


/*
 * DDCOLORCONTROL
 */
typedef struct _DDCOLORCONTROL
{
    UINT32               dwSize;
    UINT32               dwFlags;
    INT32                lBrightness;
    INT32                lContrast;
    INT32                lHue;
    INT32                lSaturation;
    INT32                lSharpness;
    INT32                lGamma;
    INT32                lColorEnable;
    UINT32               dwReserved1;
} DDCOLORCONTROL;


/*
 * lBrightness field is valid.
 */
#define DDCOLOR_BRIGHTNESS              0x00000001l

/*
 * lContrast field is valid.
 */
#define DDCOLOR_CONTRAST                0x00000002l

/*
 * lHue field is valid.
 */
#define DDCOLOR_HUE                     0x00000004l

/*
 * lSaturation field is valid.
 */
#define DDCOLOR_SATURATION              0x00000008l

/*
 * lSharpness field is valid.
 */
#define DDCOLOR_SHARPNESS               0x00000010l

/*
 * lGamma field is valid.
 */
#define DDCOLOR_GAMMA                   0x00000020l

/*
 * lColorEnable field is valid.
 */
#define DDCOLOR_COLORENABLE             0x00000040l



/*============================================================================
 *
 * Direct Draw Capability Flags
 *
 * These flags are used to describe the capabilities of a given Surface.
 * All flags are bit flags.
 *
 *==========================================================================*/

/****************************************************************************
 *
 * DIRECTDRAWSURFACE CAPABILITY FLAGS
 *
 ****************************************************************************/

/*
 * This bit is reserved. It should not be specified.
 */
#define DDSCAPS_RESERVED1                       0x00000001l

/*
 * Indicates that this surface contains alpha-only information.
 * (To determine if a surface is RGBA/YUVA, the pixel format must be
 * interrogated.)
 */
#define DDSCAPS_ALPHA                           0x00000002l

/*
 * Indicates that this surface is a backbuffer.  It is generally
 * set by CreateSurface when the DDSCAPS_FLIP capability bit is set.
 * It indicates that this surface is THE back buffer of a surface
 * flipping structure.  DirectDraw supports N surfaces in a
 * surface flipping structure.  Only the surface that immediately
 * precedeces the DDSCAPS_FRONTBUFFER has this capability bit set.
 * The other surfaces are identified as back buffers by the presence
 * of the DDSCAPS_FLIP capability, their attachment order, and the
 * absence of the DDSCAPS_FRONTBUFFER and DDSCAPS_BACKBUFFER
 * capabilities.  The bit is sent to CreateSurface when a standalone
 * back buffer is being created.  This surface could be attached to
 * a front buffer and/or back buffers to form a flipping surface
 * structure after the CreateSurface call.  See AddAttachments for
 * a detailed description of the behaviors in this case.
 */
#define DDSCAPS_BACKBUFFER                      0x00000004l

/*
 * Indicates a complex surface structure is being described.  A
 * complex surface structure results in the creation of more than
 * one surface.  The additional surfaces are attached to the root
 * surface.  The complex structure can only be destroyed by
 * destroying the root.
 */
#define DDSCAPS_COMPLEX                         0x00000008l

/*
 * Indicates that this surface is a part of a surface flipping structure.
 * When it is passed to CreateSurface the DDSCAPS_FRONTBUFFER and
 * DDSCAP_BACKBUFFER bits are not set.  They are set by CreateSurface
 * on the resulting creations.  The dwBackBufferCount field in the
 * DDSURFACEDESC structure must be set to at least 1 in order for
 * the CreateSurface call to succeed.  The DDSCAPS_COMPLEX capability
 * must always be set with creating multiple surfaces through CreateSurface.
 */
#define DDSCAPS_FLIP                            0x00000010l

/*
 * Indicates that this surface is THE front buffer of a surface flipping
 * structure.  It is generally set by CreateSurface when the DDSCAPS_FLIP
 * capability bit is set.
 * If this capability is sent to CreateSurface then a standalonw front buffer
 * is created.  This surface will not have the DDSCAPS_FLIP capability.
 * It can be attached to other back buffers to form a flipping structure.
 * See AddAttachments for a detailed description of the behaviors in this
 * case.
 */
#define DDSCAPS_FRONTBUFFER                     0x00000020l

/*
 * Indicates that this surface is any offscreen surface that is not an overlay,
 * texture, zbuffer, front buffer, back buffer, or alpha surface.  It is used
 * to identify plain vanilla surfaces.
 */
#define DDSCAPS_OFFSCREENPLAIN                  0x00000040l

/*
 * Indicates that this surface is an overlay.  It may or may not be directly visible
 * depending on whether or not it is currently being overlayed onto the primary
 * surface.  DDSCAPS_VISIBLE can be used to determine whether or not it is being
 * overlayed at the moment.
 */
#define DDSCAPS_OVERLAY                         0x00000080l

/*
 * Indicates that unique DirectDrawPalette objects can be created and
 * attached to this surface.
 */
#define DDSCAPS_PALETTE                         0x00000100l

/*
 * Indicates that this surface is the primary surface.  The primary
 * surface represents what the user is seeing at the moment.
 */
#define DDSCAPS_PRIMARYSURFACE                  0x00000200l


/*
 * This flag used to be DDSCAPS_PRIMARYSURFACELEFT, which is now
 * obsolete.
 */
#define DDSCAPS_RESERVED3               0x00000400l
#define DDSCAPS_PRIMARYSURFACELEFT              0x00000000l

/*
 * Indicates that this surface memory was allocated in system memory
 */
#define DDSCAPS_SYSTEMMEMORY                    0x00000800l

/*
 * Indicates that this surface can be used as a 3D texture.  It does not
 * indicate whether or not the surface is being used for that purpose.
 */
#define DDSCAPS_TEXTURE                         0x00001000l

/*
 * Indicates that a surface may be a destination for 3D rendering.  This
 * bit must be set in order to query for a Direct3D Device Interface
 * from this surface.
 */
#define DDSCAPS_3DDEVICE                        0x00002000l

/*
 * Indicates that this surface exists in video memory.
 */
#define DDSCAPS_VIDEOMEMORY                     0x00004000l

/*
 * Indicates that changes made to this surface are immediately visible.
 * It is always set for the primary surface and is set for overlays while
 * they are being overlayed and texture maps while they are being textured.
 */
#define DDSCAPS_VISIBLE                         0x00008000l

/*
 * Indicates that only writes are permitted to the surface.  Read accesses
 * from the surface may or may not generate a protection fault, but the
 * results of a read from this surface will not be meaningful.  READ ONLY.
 */
#define DDSCAPS_WRITEONLY                       0x00010000l

/*
 * Indicates that this surface is a z buffer. A z buffer does not contain
 * displayable information.  Instead it contains bit depth information that is
 * used to determine which pixels are visible and which are obscured.
 */
#define DDSCAPS_ZBUFFER                         0x00020000l

/*
 * Indicates surface will have a DC associated long term
 */
#define DDSCAPS_OWNDC                           0x00040000l

/*
 * Indicates surface should be able to receive live video
 */
#define DDSCAPS_LIVEVIDEO                       0x00080000l

/*
 * Indicates surface should be able to have a stream decompressed
 * to it by the hardware.
 */
#define DDSCAPS_HWCODEC                         0x00100000l

/*
 * Surface is a ModeX surface.
 *
 */
#define DDSCAPS_MODEX                           0x00200000l

/*
 * Indicates surface is one level of a mip-map. This surface will
 * be attached to other DDSCAPS_MIPMAP surfaces to form the mip-map.
 * This can be done explicitly, by creating a number of surfaces and
 * attaching them with AddAttachedSurface or by implicitly by CreateSurface.
 * If this bit is set then DDSCAPS_TEXTURE must also be set.
 */
#define DDSCAPS_MIPMAP                          0x00400000l

/*
 * This bit is reserved. It should not be specified.
 */
#define DDSCAPS_RESERVED2                       0x00800000l


/*
 * Indicates that memory for the surface is not allocated until the surface
 * is loaded (via the Direct3D texture Load() function).
 */
#define DDSCAPS_ALLOCONLOAD                     0x04000000l

/*
 * Indicates that the surface will recieve data from a video port.
 */
#define DDSCAPS_VIDEOPORT                       0x08000000l

/*
 * Indicates that a video memory surface is resident in true, local video
 * memory rather than non-local video memory. If this flag is specified then
 * so must DDSCAPS_VIDEOMEMORY. This flag is mutually exclusive with
 * DDSCAPS_NONLOCALVIDMEM.
 */
#define DDSCAPS_LOCALVIDMEM                     0x10000000l

/*
 * Indicates that a video memory surface is resident in non-local video
 * memory rather than true, local video memory. If this flag is specified
 * then so must DDSCAPS_VIDEOMEMORY. This flag is mutually exclusive with
 * DDSCAPS_LOCALVIDMEM.
 */
#define DDSCAPS_NONLOCALVIDMEM                  0x20000000l

/*
 * Indicates that this surface is a standard VGA mode surface, and not a
 * ModeX surface. (This flag will never be set in combination with the
 * DDSCAPS_MODEX flag).
 */
#define DDSCAPS_STANDARDVGAMODE                 0x40000000l

/*
 * Indicates that this surface will be an optimized surface. This flag is
 * currently only valid in conjunction with the DDSCAPS_TEXTURE flag. The surface
 * will be created without any underlying video memory until loaded.
 */
#define DDSCAPS_OPTIMIZED                       0x80000000l



/*
 * This bit is reserved
 */
#define DDSCAPS2_RESERVED4                      0x00000002L
#define DDSCAPS2_HARDWAREDEINTERLACE            0x00000000L

/*
 * Indicates to the driver that this surface will be locked very frequently
 * (for procedural textures, dynamic lightmaps, etc). Surfaces with this cap
 * set must also have DDSCAPS_TEXTURE. This cap cannot be used with
 * DDSCAPS2_HINTSTATIC and DDSCAPS2_OPAQUE.
 */
#define DDSCAPS2_HINTDYNAMIC                    0x00000004L

/*
 * Indicates to the driver that this surface can be re-ordered/retiled on
 * load. This operation will not change the size of the texture. It is
 * relatively fast and symmetrical, since the application may lock these
 * bits (although it will take a performance hit when doing so). Surfaces
 * with this cap set must also have DDSCAPS_TEXTURE. This cap cannot be
 * used with DDSCAPS2_HINTDYNAMIC and DDSCAPS2_OPAQUE.
 */
#define DDSCAPS2_HINTSTATIC                     0x00000008L

/*
 * Indicates that the client would like this texture surface to be managed by the
 * DirectDraw/Direct3D runtime. Surfaces with this cap set must also have
 * DDSCAPS_TEXTURE set.
 */
#define DDSCAPS2_TEXTUREMANAGE                  0x00000010L

/*
 * These bits are reserved for internal use */
#define DDSCAPS2_RESERVED1                      0x00000020L
#define DDSCAPS2_RESERVED2                      0x00000040L

/*
 * Indicates to the driver that this surface will never be locked again.
 * The driver is free to optimize this surface via retiling and actual compression.
 * All calls to Lock() or Blts from this surface will fail. Surfaces with this
 * cap set must also have DDSCAPS_TEXTURE. This cap cannot be used with
 * DDSCAPS2_HINTDYNAMIC and DDSCAPS2_HINTSTATIC.
 */
#define DDSCAPS2_OPAQUE                         0x00000080L

/*
 * Applications should set this bit at CreateSurface time to indicate that they
 * intend to use antialiasing. Only valid if DDSCAPS_3DDEVICE is also set.
 */
#define DDSCAPS2_HINTANTIALIASING               0x00000100L


/*
 * This flag is used at CreateSurface time to indicate that this set of
 * surfaces is a cubic environment map
 */
#define DDSCAPS2_CUBEMAP                        0x00000200L

/*
 * These flags preform two functions:
 * - At CreateSurface time, they define which of the six cube faces are
 *   required by the application.
 * - After creation, each face in the cubemap will have exactly one of these
 *   bits set.
 */
#define DDSCAPS2_CUBEMAP_POSITIVEX              0x00000400L
#define DDSCAPS2_CUBEMAP_NEGATIVEX              0x00000800L
#define DDSCAPS2_CUBEMAP_POSITIVEY              0x00001000L
#define DDSCAPS2_CUBEMAP_NEGATIVEY              0x00002000L
#define DDSCAPS2_CUBEMAP_POSITIVEZ              0x00004000L
#define DDSCAPS2_CUBEMAP_NEGATIVEZ              0x00008000L

/*
 * This macro may be used to specify all faces of a cube map at CreateSurface time
 */
#define DDSCAPS2_CUBEMAP_ALLFACES ( DDSCAPS2_CUBEMAP_POSITIVEX |\
                                    DDSCAPS2_CUBEMAP_NEGATIVEX |\
                                    DDSCAPS2_CUBEMAP_POSITIVEY |\
                                    DDSCAPS2_CUBEMAP_NEGATIVEY |\
                                    DDSCAPS2_CUBEMAP_POSITIVEZ |\
                                    DDSCAPS2_CUBEMAP_NEGATIVEZ )


/*
 * This flag is an additional flag which is present on mipmap sublevels from DX7 onwards
 * It enables easier use of GetAttachedSurface rather than EnumAttachedSurfaces for surface
 * constructs such as Cube Maps, wherein there are more than one mipmap surface attached
 * to the root surface.
 * This caps bit is ignored by CreateSurface
 */
#define DDSCAPS2_MIPMAPSUBLEVEL                 0x00010000L

/* This flag indicates that the texture should be managed by D3D only */
#define DDSCAPS2_D3DTEXTUREMANAGE               0x00020000L

/* This flag indicates that the managed surface can be safely lost */
#define DDSCAPS2_DONOTPERSIST                   0x00040000L

/* indicates that this surface is part of a stereo flipping chain */
#define DDSCAPS2_STEREOSURFACELEFT              0x00080000L


/*
 * Indicates that the surface is a volume.
 * Can be combined with DDSCAPS_MIPMAP to indicate a multi-level volume
 */
#define DDSCAPS2_VOLUME                         0x00200000L

/*
 * Indicates that the surface may be locked multiple times by the application.
 * This cap cannot be used with DDSCAPS2_OPAQUE.
 */
#define DDSCAPS2_NOTUSERLOCKABLE                0x00400000L

/*
 * Indicates that the vertex buffer data can be used to render points and
 * point sprites.
 */
#define DDSCAPS2_POINTS                         0x00800000L

/*
 * Indicates that the vertex buffer data can be used to render rt pactches.
 */
#define DDSCAPS2_RTPATCHES                      0x01000000L

/*
 * Indicates that the vertex buffer data can be used to render n patches.
 */
#define DDSCAPS2_NPATCHES                       0x02000000L

/*
 * This bit is reserved for internal use 
 */
#define DDSCAPS2_RESERVED3                      0x04000000L


/*
 * Indicates that the contents of the backbuffer do not have to be preserved
 * the contents of the backbuffer after they are presented.
 */
#define DDSCAPS2_DISCARDBACKBUFFER              0x10000000L

/*
 * Indicates that all surfaces in this creation chain should be given an alpha channel.
 * This flag will be set on primary surface chains that may have no explicit pixel format
 * (and thus take on the format of the current display mode).
 * The driver should infer that all these surfaces have a format having an alpha channel.
 * (e.g. assume D3DFMT_A8R8G8B8 if the display mode is x888.)
 */
#define DDSCAPS2_ENABLEALPHACHANNEL             0x20000000L

/*
 * Indicates that all surfaces in this creation chain is extended primary surface format.
 * This flag will be set on extended primary surface chains that always have explicit pixel
 * format and the pixel format is typically GDI (Graphics Device Interface) couldn't handle,
 * thus only used with fullscreen application. (e.g. D3DFMT_A2R10G10B10 format)
 */
#define DDSCAPS2_EXTENDEDFORMATPRIMARY          0x40000000L

/*
 * Indicates that all surfaces in this creation chain is additional primary surface.
 * This flag will be set on primary surface chains which must present on the adapter
 * id provided on dwCaps4. Typically this will be used to create secondary primary surface
 * on DualView display adapter.
 */
#define DDSCAPS2_ADDITIONALPRIMARY              0x80000000L

/*
 * This is a mask that indicates the set of bits that may be set
 * at createsurface time to indicate number of samples per pixel
 * when multisampling
 */
#define DDSCAPS3_MULTISAMPLE_MASK               0x0000001FL

/*
 * This is a mask that indicates the set of bits that may be set
 * at createsurface time to indicate the quality level of rendering
 * for the current number of samples per pixel
 */
#define DDSCAPS3_MULTISAMPLE_QUALITY_MASK       0x000000E0L
#define DDSCAPS3_MULTISAMPLE_QUALITY_SHIFT      5

/*
 * This bit is reserved for internal use 
 */
#define DDSCAPS3_RESERVED1                      0x00000100L

/*
 * This bit is reserved for internal use 
 */
#define DDSCAPS3_RESERVED2                      0x00000200L

/*
 * This indicates whether this surface has light-weight miplevels
 */
#define DDSCAPS3_LIGHTWEIGHTMIPMAP              0x00000400L

/*
 * This indicates that the mipsublevels for this surface are auto-generated
 */
#define DDSCAPS3_AUTOGENMIPMAP                  0x00000800L

/*
 * This indicates that the mipsublevels for this surface are auto-generated
 */
#define DDSCAPS3_DMAP                           0x00001000L


 /****************************************************************************
 *
 * DIRECTDRAW DRIVER CAPABILITY FLAGS
 *
 ****************************************************************************/

/*
 * Display hardware has 3D acceleration.
 */
#define DDCAPS_3D                       0x00000001l

/*
 * Indicates that DirectDraw will support only dest rectangles that are aligned
 * on DIRECTDRAWCAPS.dwAlignBoundaryDest boundaries of the surface, respectively.
 * READ ONLY.
 */
#define DDCAPS_ALIGNBOUNDARYDEST        0x00000002l

/*
 * Indicates that DirectDraw will support only source rectangles  whose sizes in
 * BYTEs are DIRECTDRAWCAPS.dwAlignSizeDest multiples, respectively.  READ ONLY.
 */
#define DDCAPS_ALIGNSIZEDEST            0x00000004l
/*
 * Indicates that DirectDraw will support only source rectangles that are aligned
 * on DIRECTDRAWCAPS.dwAlignBoundarySrc boundaries of the surface, respectively.
 * READ ONLY.
 */
#define DDCAPS_ALIGNBOUNDARYSRC         0x00000008l

/*
 * Indicates that DirectDraw will support only source rectangles  whose sizes in
 * BYTEs are DIRECTDRAWCAPS.dwAlignSizeSrc multiples, respectively.  READ ONLY.
 */
#define DDCAPS_ALIGNSIZESRC             0x00000010l

/*
 * Indicates that DirectDraw will create video memory surfaces that have a stride
 * alignment equal to DIRECTDRAWCAPS.dwAlignStride.  READ ONLY.
 */
#define DDCAPS_ALIGNSTRIDE              0x00000020l

/*
 * Display hardware is capable of blt operations.
 */
#define DDCAPS_BLT                      0x00000040l

/*
 * Display hardware is capable of asynchronous blt operations.
 */
#define DDCAPS_BLTQUEUE                 0x00000080l

/*
 * Display hardware is capable of color space conversions during the blt operation.
 */
#define DDCAPS_BLTFOURCC                0x00000100l

/*
 * Display hardware is capable of stretching during blt operations.
 */
#define DDCAPS_BLTSTRETCH               0x00000200l

/*
 * Display hardware is shared with GDI.
 */
#define DDCAPS_GDI                      0x00000400l

/*
 * Display hardware can overlay.
 */
#define DDCAPS_OVERLAY                  0x00000800l

/*
 * Set if display hardware supports overlays but can not clip them.
 */
#define DDCAPS_OVERLAYCANTCLIP          0x00001000l

/*
 * Indicates that overlay hardware is capable of color space conversions during
 * the overlay operation.
 */
#define DDCAPS_OVERLAYFOURCC            0x00002000l

/*
 * Indicates that stretching can be done by the overlay hardware.
 */
#define DDCAPS_OVERLAYSTRETCH           0x00004000l

/*
 * Indicates that unique DirectDrawPalettes can be created for DirectDrawSurfaces
 * other than the primary surface.
 */
#define DDCAPS_PALETTE                  0x00008000l

/*
 * Indicates that palette changes can be syncd with the veritcal refresh.
 */
#define DDCAPS_PALETTEVSYNC             0x00010000l

/*
 * Display hardware can return the current scan line.
 */
#define DDCAPS_READSCANLINE             0x00020000l


/*
 * This flag used to bo DDCAPS_STEREOVIEW, which is now obsolete
 */
#define DDCAPS_RESERVED1                0x00040000l

/*
 * Display hardware is capable of generating a vertical blank interrupt.
 */
#define DDCAPS_VBI                      0x00080000l

/*
 * Supports the use of z buffers with blt operations.
 */
#define DDCAPS_ZBLTS                    0x00100000l

/*
 * Supports Z Ordering of overlays.
 */
#define DDCAPS_ZOVERLAYS                0x00200000l

/*
 * Supports color key
 */
#define DDCAPS_COLORKEY                 0x00400000l

/*
 * Supports alpha surfaces
 */
#define DDCAPS_ALPHA                    0x00800000l

/*
 * colorkey is hardware assisted(DDCAPS_COLORKEY will also be set)
 */
#define DDCAPS_COLORKEYHWASSIST         0x01000000l

/*
 * no hardware support at all
 */
#define DDCAPS_NOHARDWARE               0x02000000l

/*
 * Display hardware is capable of color fill with bltter
 */
#define DDCAPS_BLTCOLORFILL             0x04000000l

/*
 * Display hardware is bank switched, and potentially very slow at
 * random access to VRAM.
 */
#define DDCAPS_BANKSWITCHED             0x08000000l

/*
 * Display hardware is capable of depth filling Z-buffers with bltter
 */
#define DDCAPS_BLTDEPTHFILL             0x10000000l

/*
 * Display hardware is capable of clipping while bltting.
 */
#define DDCAPS_CANCLIP                  0x20000000l

/*
 * Display hardware is capable of clipping while stretch bltting.
 */
#define DDCAPS_CANCLIPSTRETCHED         0x40000000l

/*
 * Display hardware is capable of bltting to or from system memory
 */
#define DDCAPS_CANBLTSYSMEM             0x80000000l


 /****************************************************************************
 *
 * MORE DIRECTDRAW DRIVER CAPABILITY FLAGS (dwCaps2)
 *
 ****************************************************************************/

/*
 * Display hardware is certified
 */
#define DDCAPS2_CERTIFIED              0x00000001l

/*
 * Driver cannot interleave 2D operations (lock and blt) to surfaces with
 * Direct3D rendering operations between calls to BeginScene() and EndScene()
 */
#define DDCAPS2_NO2DDURING3DSCENE       0x00000002l

/*
 * Display hardware contains a video port
 */
#define DDCAPS2_VIDEOPORT               0x00000004l

/*
 * The overlay can be automatically flipped according to the video port
 * VSYNCs, providing automatic doubled buffered display of video port
 * data using an overlay
 */
#define DDCAPS2_AUTOFLIPOVERLAY         0x00000008l

/*
 * Overlay can display each field of interlaced data individually while
 * it is interleaved in memory without causing jittery artifacts.
 */
#define DDCAPS2_CANBOBINTERLEAVED       0x00000010l

/*
 * Overlay can display each field of interlaced data individually while
 * it is not interleaved in memory without causing jittery artifacts.
 */
#define DDCAPS2_CANBOBNONINTERLEAVED    0x00000020l

/*
 * The overlay surface contains color controls (brightness, sharpness, etc.)
 */
#define DDCAPS2_COLORCONTROLOVERLAY     0x00000040l

/*
 * The primary surface contains color controls (gamma, etc.)
 */
#define DDCAPS2_COLORCONTROLPRIMARY     0x00000080l

/*
 * RGBZ -> RGB supported for 16:16 RGB:Z
 */
#define DDCAPS2_CANDROPZ16BIT           0x00000100l

/*
 * Driver supports non-local video memory.
 */
#define DDCAPS2_NONLOCALVIDMEM          0x00000200l

/*
 * Dirver supports non-local video memory but has different capabilities for
 * non-local video memory surfaces. If this bit is set then so must
 * DDCAPS2_NONLOCALVIDMEM.
 */
#define DDCAPS2_NONLOCALVIDMEMCAPS      0x00000400l

/*
 * Driver neither requires nor prefers surfaces to be pagelocked when performing
 * blts involving system memory surfaces
 */
#define DDCAPS2_NOPAGELOCKREQUIRED      0x00000800l

/*
 * Driver can create surfaces which are wider than the primary surface
 */
#define DDCAPS2_WIDESURFACES            0x00001000l

/*
 * Driver supports bob without using a video port by handling the
 * DDFLIP_ODD and DDFLIP_EVEN flags specified in Flip.
 */
#define DDCAPS2_CANFLIPODDEVEN          0x00002000l

/*
 * Driver supports bob using hardware
 */
#define DDCAPS2_CANBOBHARDWARE          0x00004000l

/*
 * Driver supports bltting any FOURCC surface to another surface of the same FOURCC
 */
#define DDCAPS2_COPYFOURCC              0x00008000l


/*
 * Driver supports loadable gamma ramps for the primary surface
 */
#define DDCAPS2_PRIMARYGAMMA            0x00020000l

/*
 * Driver can render in windowed mode.
 */
#define DDCAPS2_CANRENDERWINDOWED       0x00080000l

/*
 * A calibrator is available to adjust the gamma ramp according to the
 * physical display properties so that the result will be identical on
 * all calibrated systems.
 */
#define DDCAPS2_CANCALIBRATEGAMMA       0x00100000l

/*
 * Indicates that the driver will respond to DDFLIP_INTERVALn flags
 */
#define DDCAPS2_FLIPINTERVAL            0x00200000l

/*
 * Indicates that the driver will respond to DDFLIP_NOVSYNC
 */
#define DDCAPS2_FLIPNOVSYNC             0x00400000l

/*
 * Driver supports management of video memory, if this flag is ON,
 * driver manages the texture if requested with DDSCAPS2_TEXTUREMANAGE on
 * DirectX manages the texture if this flag is OFF and surface has DDSCAPS2_TEXTUREMANAGE on
 */
#define DDCAPS2_CANMANAGETEXTURE        0x00800000l

/*
 * The Direct3D texture manager uses this cap to decide whether to put managed
 * surfaces in non-local video memory. If the cap is set, the texture manager will
 * put managed surfaces in non-local vidmem. Drivers that cannot texture from
 * local vidmem SHOULD NOT set this cap.
 */
#define DDCAPS2_TEXMANINNONLOCALVIDMEM  0x01000000l

/*
 * Indicates that the driver supports DX7 type of stereo in at least one mode (which may
 * not necessarily be the current mode). Applications should use IDirectDraw7 (or higher)
 * ::EnumDisplayModes and check the DDSURFACEDESC.ddsCaps.dwCaps2 field for the presence of
 * DDSCAPS2_STEREOSURFACELEFT to check if a particular mode supports stereo. The application
 * can also use IDirectDraw7(or higher)::GetDisplayMode to check the current mode.
 */
#define DDCAPS2_STEREO                  0x02000000L

/*
 * This caps bit is intended for internal DirectDraw use.
 * -It is only valid if DDCAPS2_NONLOCALVIDMEMCAPS is set.
 * -If this bit is set, then DDCAPS_CANBLTSYSMEM MUST be set by the driver (and
 *  all the assoicated system memory blt caps must be correct).
 * -It implies that the system->video blt caps in DDCAPS also apply to system to
 *  nonlocal blts. I.e. the dwSVBCaps, dwSVBCKeyCaps, dwSVBFXCaps and dwSVBRops
 *  members of DDCAPS (DDCORECAPS) are filled in correctly.
 * -Any blt from system to nonlocal memory that matches these caps bits will
 *  be passed to the driver.
 *
 * NOTE: This is intended to enable the driver itself to do efficient reordering
 * of textures. This is NOT meant to imply that hardware can write into AGP memory.
 * This operation is not currently supported.
 */
#define DDCAPS2_SYSTONONLOCAL_AS_SYSTOLOCAL   0x04000000L

/*
 * was DDCAPS2_PUREHAL
 */
#define DDCAPS2_RESERVED1                     0x08000000L

/*
 * Driver supports management of video memory, if this flag is ON,
 * driver manages the resource if requested with DDSCAPS2_TEXTUREMANAGE on
 * DirectX manages the resource if this flag is OFF and surface has DDSCAPS2_TEXTUREMANAGE on
 */
#define DDCAPS2_CANMANAGERESOURCE             0x10000000L

/*
 * Driver supports dynamic textures. This will allow the application to set
 * D3DUSAGE_DYNAMIC (DDSCAPS2_HINTDYNAMIC for drivers) at texture create time.
 * Video memory dynamic textures WILL be lockable by applications. It is
 * expected that these locks will be very efficient (which implies that the
 * driver should always maintain a linear copy, a pointer to which can be
 * quickly handed out to the application).
 */
#define DDCAPS2_DYNAMICTEXTURES               0x20000000L

/*
 * Driver supports auto-generation of mipmaps.
 */
#define DDCAPS2_CANAUTOGENMIPMAP              0x40000000L


/****************************************************************************
 *
 * DIRECTDRAW FX ALPHA CAPABILITY FLAGS
 *
 ****************************************************************************/

/*
 * Supports alpha blending around the edge of a source color keyed surface.
 * For Blt.
 */
#define DDFXALPHACAPS_BLTALPHAEDGEBLEND         0x00000001l

/*
 * Supports alpha information in the pixel format.  The bit depth of alpha
 * information in the pixel format can be 1,2,4, or 8.  The alpha value becomes
 * more opaque as the alpha value increases.  (0 is transparent.)
 * For Blt.
 */
#define DDFXALPHACAPS_BLTALPHAPIXELS            0x00000002l

/*
 * Supports alpha information in the pixel format.  The bit depth of alpha
 * information in the pixel format can be 1,2,4, or 8.  The alpha value
 * becomes more transparent as the alpha value increases.  (0 is opaque.)
 * This flag can only be set if DDCAPS_ALPHA is set.
 * For Blt.
 */
#define DDFXALPHACAPS_BLTALPHAPIXELSNEG         0x00000004l

/*
 * Supports alpha only surfaces.  The bit depth of an alpha only surface can be
 * 1,2,4, or 8.  The alpha value becomes more opaque as the alpha value increases.
 * (0 is transparent.)
 * For Blt.
 */
#define DDFXALPHACAPS_BLTALPHASURFACES          0x00000008l

/*
 * The depth of the alpha channel data can range can be 1,2,4, or 8.
 * The NEG suffix indicates that this alpha channel becomes more transparent
 * as the alpha value increases. (0 is opaque.)  This flag can only be set if
 * DDCAPS_ALPHA is set.
 * For Blt.
 */
#define DDFXALPHACAPS_BLTALPHASURFACESNEG       0x00000010l

/*
 * Supports alpha blending around the edge of a source color keyed surface.
 * For Overlays.
 */
#define DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND     0x00000020l

/*
 * Supports alpha information in the pixel format.  The bit depth of alpha
 * information in the pixel format can be 1,2,4, or 8.  The alpha value becomes
 * more opaque as the alpha value increases.  (0 is transparent.)
 * For Overlays.
 */
#define DDFXALPHACAPS_OVERLAYALPHAPIXELS        0x00000040l

/*
 * Supports alpha information in the pixel format.  The bit depth of alpha
 * information in the pixel format can be 1,2,4, or 8.  The alpha value
 * becomes more transparent as the alpha value increases.  (0 is opaque.)
 * This flag can only be set if DDCAPS_ALPHA is set.
 * For Overlays.
 */
#define DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG     0x00000080l

/*
 * Supports alpha only surfaces.  The bit depth of an alpha only surface can be
 * 1,2,4, or 8.  The alpha value becomes more opaque as the alpha value increases.
 * (0 is transparent.)
 * For Overlays.
 */
#define DDFXALPHACAPS_OVERLAYALPHASURFACES      0x00000100l

/*
 * The depth of the alpha channel data can range can be 1,2,4, or 8.
 * The NEG suffix indicates that this alpha channel becomes more transparent
 * as the alpha value increases. (0 is opaque.)  This flag can only be set if
 * DDCAPS_ALPHA is set.
 * For Overlays.
 */
#define DDFXALPHACAPS_OVERLAYALPHASURFACESNEG   0x00000200l

#if DIRECTDRAW_VERSION < 0x0600
#endif  //DIRECTDRAW_VERSION


/****************************************************************************
 *
 * DIRECTDRAW FX CAPABILITY FLAGS
 *
 ****************************************************************************/

/*
 * Uses arithmetic operations to stretch and shrink surfaces during blt
 * rather than pixel doubling techniques.  Along the Y axis.
 */
#define DDFXCAPS_BLTARITHSTRETCHY       0x00000020l

/*
 * Uses arithmetic operations to stretch during blt
 * rather than pixel doubling techniques.  Along the Y axis. Only
 * works for x1, x2, etc.
 */
#define DDFXCAPS_BLTARITHSTRETCHYN      0x00000010l

/*
 * Supports mirroring left to right in blt.
 */
#define DDFXCAPS_BLTMIRRORLEFTRIGHT     0x00000040l

/*
 * Supports mirroring top to bottom in blt.
 */
#define DDFXCAPS_BLTMIRRORUPDOWN        0x00000080l

/*
 * Supports arbitrary rotation for blts.
 */
#define DDFXCAPS_BLTROTATION            0x00000100l

/*
 * Supports 90 degree rotations for blts.
 */
#define DDFXCAPS_BLTROTATION90          0x00000200l

/*
 * DirectDraw supports arbitrary shrinking of a surface along the
 * x axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSHRINKX             0x00000400l

/*
 * DirectDraw supports integer shrinking (1x,2x,) of a surface
 * along the x axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSHRINKXN            0x00000800l

/*
 * DirectDraw supports arbitrary shrinking of a surface along the
 * y axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSHRINKY             0x00001000l

/*
 * DirectDraw supports integer shrinking (1x,2x,) of a surface
 * along the y axis (vertical direction) for blts.
 */
#define DDFXCAPS_BLTSHRINKYN            0x00002000l

/*
 * DirectDraw supports arbitrary stretching of a surface along the
 * x axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSTRETCHX            0x00004000l

/*
 * DirectDraw supports integer stretching (1x,2x,) of a surface
 * along the x axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSTRETCHXN           0x00008000l

/*
 * DirectDraw supports arbitrary stretching of a surface along the
 * y axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSTRETCHY            0x00010000l

/*
 * DirectDraw supports integer stretching (1x,2x,) of a surface
 * along the y axis (vertical direction) for blts.
 */
#define DDFXCAPS_BLTSTRETCHYN           0x00020000l

/*
 * Uses arithmetic operations to stretch and shrink surfaces during
 * overlay rather than pixel doubling techniques.  Along the Y axis
 * for overlays.
 */
#define DDFXCAPS_OVERLAYARITHSTRETCHY   0x00040000l

/*
 * Uses arithmetic operations to stretch surfaces during
 * overlay rather than pixel doubling techniques.  Along the Y axis
 * for overlays. Only works for x1, x2, etc.
 */
#define DDFXCAPS_OVERLAYARITHSTRETCHYN  0x00000008l

/*
 * DirectDraw supports arbitrary shrinking of a surface along the
 * x axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSHRINKX         0x00080000l

/*
 * DirectDraw supports integer shrinking (1x,2x,) of a surface
 * along the x axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSHRINKXN        0x00100000l

/*
 * DirectDraw supports arbitrary shrinking of a surface along the
 * y axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSHRINKY         0x00200000l

/*
 * DirectDraw supports integer shrinking (1x,2x,) of a surface
 * along the y axis (vertical direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSHRINKYN        0x00400000l

/*
 * DirectDraw supports arbitrary stretching of a surface along the
 * x axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSTRETCHX        0x00800000l

/*
 * DirectDraw supports integer stretching (1x,2x,) of a surface
 * along the x axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSTRETCHXN       0x01000000l

/*
 * DirectDraw supports arbitrary stretching of a surface along the
 * y axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSTRETCHY        0x02000000l

/*
 * DirectDraw supports integer stretching (1x,2x,) of a surface
 * along the y axis (vertical direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSTRETCHYN       0x04000000l

/*
 * DirectDraw supports mirroring of overlays across the vertical axis
 */
#define DDFXCAPS_OVERLAYMIRRORLEFTRIGHT 0x08000000l

/*
 * DirectDraw supports mirroring of overlays across the horizontal axis
 */
#define DDFXCAPS_OVERLAYMIRRORUPDOWN    0x10000000l

/*
 * DirectDraw supports deinterlacing of overlay surfaces
 */
#define DDFXCAPS_OVERLAYDEINTERLACE		0x20000000l

/*
 * Driver can do alpha blending for blits.
 */
#define DDFXCAPS_BLTALPHA               0x00000001l


/*
 * Driver can do surface-reconstruction filtering for warped blits.
 */
#define DDFXCAPS_BLTFILTER              DDFXCAPS_BLTARITHSTRETCHY

/*
 * Driver can do alpha blending for overlays.
 */
#define DDFXCAPS_OVERLAYALPHA           0x00000004l


/*
 * Driver can do surface-reconstruction filtering for warped overlays.
 */
#define DDFXCAPS_OVERLAYFILTER          DDFXCAPS_OVERLAYARITHSTRETCHY

/****************************************************************************
 *
 * DIRECTDRAW STEREO VIEW CAPABILITIES
 *
 ****************************************************************************/

/*
 * This flag used to be DDSVCAPS_ENIGMA, which is now obsolete
 */

#define DDSVCAPS_RESERVED1              0x00000001l

/*
 * This flag used to be DDSVCAPS_FLICKER, which is now obsolete
 */
#define DDSVCAPS_RESERVED2              0x00000002l

/*
 * This flag used to be DDSVCAPS_REDBLUE, which is now obsolete
 */
#define DDSVCAPS_RESERVED3              0x00000004l

/*
 * This flag used to be DDSVCAPS_SPLIT, which is now obsolete
 */
#define DDSVCAPS_RESERVED4              0x00000008l

/*
 * The stereo view is accomplished with switching technology
 */

#define DDSVCAPS_STEREOSEQUENTIAL       0x00000010L



/****************************************************************************
 *
 * DIRECTDRAWPALETTE CAPABILITIES
 *
 ****************************************************************************/

/*
 * Index is 4 bits.  There are sixteen color entries in the palette table.
 */
#define DDPCAPS_4BIT                    0x00000001l

/*
 * Index is onto a 8 bit color index.  This field is only valid with the
 * DDPCAPS_1BIT, DDPCAPS_2BIT or DDPCAPS_4BIT capability and the target
 * surface is in 8bpp. Each color entry is one byte long and is an index
 * into destination surface's 8bpp palette.
 */
#define DDPCAPS_8BITENTRIES             0x00000002l

/*
 * Index is 8 bits.  There are 256 color entries in the palette table.
 */
#define DDPCAPS_8BIT                    0x00000004l

/*
 * Indicates that this DIRECTDRAWPALETTE should use the palette color array
 * passed into the lpDDColorArray parameter to initialize the DIRECTDRAWPALETTE
 * object.
 * This flag is obsolete. DirectDraw always initializes the color array from
 * the lpDDColorArray parameter. The definition remains for source-level
 * compatibility.
 */
#define DDPCAPS_INITIALIZE              0x00000000l

/*
 * This palette is the one attached to the primary surface.  Changing this
 * table has immediate effect on the display unless DDPSETPAL_VSYNC is specified
 * and supported.
 */
#define DDPCAPS_PRIMARYSURFACE          0x00000010l

/*
 * This palette is the one attached to the primary surface left.  Changing
 * this table has immediate effect on the display for the left eye unless
 * DDPSETPAL_VSYNC is specified and supported.
 */
#define DDPCAPS_PRIMARYSURFACELEFT      0x00000020l

/*
 * This palette can have all 256 entries defined
 */
#define DDPCAPS_ALLOW256                0x00000040l

/*
 * This palette can have modifications to it synced with the monitors
 * refresh rate.
 */
#define DDPCAPS_VSYNC                   0x00000080l

/*
 * Index is 1 bit.  There are two color entries in the palette table.
 */
#define DDPCAPS_1BIT                    0x00000100l

/*
 * Index is 2 bit.  There are four color entries in the palette table.
 */
#define DDPCAPS_2BIT                    0x00000200l

/*
 * The peFlags member of PALETTEENTRY denotes an 8 bit alpha value
 */
#define DDPCAPS_ALPHA                   0x00000400l


/****************************************************************************
 *
 * DIRECTDRAWPALETTE SETENTRY CONSTANTS
 *
 ****************************************************************************/


/****************************************************************************
 *
 * DIRECTDRAWPALETTE GETENTRY CONSTANTS
 *
 ****************************************************************************/

/* 0 is the only legal value */

/****************************************************************************
 *
 * DIRECTDRAWSURFACE SETPRIVATEDATA CONSTANTS
 *
 ****************************************************************************/

/*
 * The passed pointer is an IUnknown ptr. The cbData argument to SetPrivateData
 * must be set to sizeof(IUnknown*). DirectDraw will call AddRef through this
 * pointer and Release when the private data is destroyed. This includes when
 * the surface or palette is destroyed before such priovate data is destroyed.
 */
#define DDSPD_IUNKNOWNPOINTER           0x00000001L

/*
 * Private data is only valid for the current state of the object,
 * as determined by the uniqueness value.
 */
#define DDSPD_VOLATILE                  0x00000002L


/****************************************************************************
 *
 * DIRECTDRAWSURFACE SETPALETTE CONSTANTS
 *
 ****************************************************************************/


/****************************************************************************
 *
 * DIRECTDRAW BITDEPTH CONSTANTS
 *
 * NOTE:  These are only used to indicate supported bit depths.   These
 * are flags only, they are not to be used as an actual bit depth.   The
 * absolute numbers 1, 2, 4, 8, 16, 24 and 32 are used to indicate actual
 * bit depths in a surface or for changing the display mode.
 *
 ****************************************************************************/

/*
 * 1 bit per pixel.
 */
#define DDBD_1                  0x00004000l

/*
 * 2 bits per pixel.
 */
#define DDBD_2                  0x00002000l

/*
 * 4 bits per pixel.
 */
#define DDBD_4                  0x00001000l

/*
 * 8 bits per pixel.
 */
#define DDBD_8                  0x00000800l

/*
 * 16 bits per pixel.
 */
#define DDBD_16                 0x00000400l

/*
 * 24 bits per pixel.
 */
#define DDBD_24                 0X00000200l

/*
 * 32 bits per pixel.
 */
#define DDBD_32                 0x00000100l

/****************************************************************************
 *
 * DIRECTDRAWSURFACE SET/GET COLOR KEY FLAGS
 *
 ****************************************************************************/

/*
 * Set if the structure contains a color space.  Not set if the structure
 * contains a single color key.
 */
#define DDCKEY_COLORSPACE       0x00000001l

/*
 * Set if the structure specifies a color key or color space which is to be
 * used as a destination color key for blt operations.
 */
#define DDCKEY_DESTBLT          0x00000002l

/*
 * Set if the structure specifies a color key or color space which is to be
 * used as a destination color key for overlay operations.
 */
#define DDCKEY_DESTOVERLAY      0x00000004l

/*
 * Set if the structure specifies a color key or color space which is to be
 * used as a source color key for blt operations.
 */
#define DDCKEY_SRCBLT           0x00000008l

/*
 * Set if the structure specifies a color key or color space which is to be
 * used as a source color key for overlay operations.
 */
#define DDCKEY_SRCOVERLAY       0x00000010l


/****************************************************************************
 *
 * DIRECTDRAW COLOR KEY CAPABILITY FLAGS
 *
 ****************************************************************************/

/*
 * Supports transparent blting using a color key to identify the replaceable
 * bits of the destination surface for RGB colors.
 */
#define DDCKEYCAPS_DESTBLT                      0x00000001l

/*
 * Supports transparent blting using a color space to identify the replaceable
 * bits of the destination surface for RGB colors.
 */
#define DDCKEYCAPS_DESTBLTCLRSPACE              0x00000002l

/*
 * Supports transparent blting using a color space to identify the replaceable
 * bits of the destination surface for YUV colors.
 */
#define DDCKEYCAPS_DESTBLTCLRSPACEYUV           0x00000004l

/*
 * Supports transparent blting using a color key to identify the replaceable
 * bits of the destination surface for YUV colors.
 */
#define DDCKEYCAPS_DESTBLTYUV                   0x00000008l

/*
 * Supports overlaying using colorkeying of the replaceable bits of the surface
 * being overlayed for RGB colors.
 */
#define DDCKEYCAPS_DESTOVERLAY                  0x00000010l

/*
 * Supports a color space as the color key for the destination for RGB colors.
 */
#define DDCKEYCAPS_DESTOVERLAYCLRSPACE          0x00000020l

/*
 * Supports a color space as the color key for the destination for YUV colors.
 */
#define DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV       0x00000040l

/*
 * Supports only one active destination color key value for visible overlay
 * surfaces.
 */
#define DDCKEYCAPS_DESTOVERLAYONEACTIVE         0x00000080l

/*
 * Supports overlaying using colorkeying of the replaceable bits of the
 * surface being overlayed for YUV colors.
 */
#define DDCKEYCAPS_DESTOVERLAYYUV               0x00000100l

/*
 * Supports transparent blting using the color key for the source with
 * this surface for RGB colors.
 */
#define DDCKEYCAPS_SRCBLT                       0x00000200l

/*
 * Supports transparent blting using a color space for the source with
 * this surface for RGB colors.
 */
#define DDCKEYCAPS_SRCBLTCLRSPACE               0x00000400l

/*
 * Supports transparent blting using a color space for the source with
 * this surface for YUV colors.
 */
#define DDCKEYCAPS_SRCBLTCLRSPACEYUV            0x00000800l

/*
 * Supports transparent blting using the color key for the source with
 * this surface for YUV colors.
 */
#define DDCKEYCAPS_SRCBLTYUV                    0x00001000l

/*
 * Supports overlays using the color key for the source with this
 * overlay surface for RGB colors.
 */
#define DDCKEYCAPS_SRCOVERLAY                   0x00002000l

/*
 * Supports overlays using a color space as the source color key for
 * the overlay surface for RGB colors.
 */
#define DDCKEYCAPS_SRCOVERLAYCLRSPACE           0x00004000l

/*
 * Supports overlays using a color space as the source color key for
 * the overlay surface for YUV colors.
 */
#define DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV        0x00008000l

/*
 * Supports only one active source color key value for visible
 * overlay surfaces.
 */
#define DDCKEYCAPS_SRCOVERLAYONEACTIVE          0x00010000l

/*
 * Supports overlays using the color key for the source with this
 * overlay surface for YUV colors.
 */
#define DDCKEYCAPS_SRCOVERLAYYUV                0x00020000l

/*
 * there are no bandwidth trade-offs for using colorkey with an overlay
 */
#define DDCKEYCAPS_NOCOSTOVERLAY                0x00040000l


/****************************************************************************
 *
 * DIRECTDRAW PIXELFORMAT FLAGS
 *
 ****************************************************************************/

/*
 * The surface has alpha channel information in the pixel format.
 */
#define DDPF_ALPHAPIXELS                        0x00000001l

/*
 * The pixel format contains alpha only information
 */
#define DDPF_ALPHA                              0x00000002l

/*
 * The FourCC code is valid.
 */
#define DDPF_FOURCC                             0x00000004l

/*
 * The surface is 4-bit color indexed.
 */
#define DDPF_PALETTEINDEXED4                    0x00000008l

/*
 * The surface is indexed into a palette which stores indices
 * into the destination surface's 8-bit palette.
 */
#define DDPF_PALETTEINDEXEDTO8                  0x00000010l

/*
 * The surface is 8-bit color indexed.
 */
#define DDPF_PALETTEINDEXED8                    0x00000020l

/*
 * The RGB data in the pixel format structure is valid.
 */
#define DDPF_RGB                                0x00000040l

/*
 * The surface will accept pixel data in the format specified
 * and compress it during the write.
 */
#define DDPF_COMPRESSED                         0x00000080l

/*
 * The surface will accept RGB data and translate it during
 * the write to YUV data.  The format of the data to be written
 * will be contained in the pixel format structure.  The DDPF_RGB
 * flag will be set.
 */
#define DDPF_RGBTOYUV                           0x00000100l

/*
 * pixel format is YUV - YUV data in pixel format struct is valid
 */
#define DDPF_YUV                                0x00000200l

/*
 * pixel format is a z buffer only surface
 */
#define DDPF_ZBUFFER                            0x00000400l

/*
 * The surface is 1-bit color indexed.
 */
#define DDPF_PALETTEINDEXED1                    0x00000800l

/*
 * The surface is 2-bit color indexed.
 */
#define DDPF_PALETTEINDEXED2                    0x00001000l

/*
 * The surface contains Z information in the pixels
 */
#define DDPF_ZPIXELS                            0x00002000l

/*
 * The surface contains stencil information along with Z
 */
#define DDPF_STENCILBUFFER                      0x00004000l

/*
 * Premultiplied alpha format -- the color components have been
 * premultiplied by the alpha component.
 */
#define DDPF_ALPHAPREMULT                       0x00008000l


/*
 * Luminance data in the pixel format is valid.
 * Use this flag for luminance-only or luminance+alpha surfaces,
 * the bit depth is then ddpf.dwLuminanceBitCount.
 */
#define DDPF_LUMINANCE                          0x00020000l

/*
 * Luminance data in the pixel format is valid.
 * Use this flag when hanging luminance off bumpmap surfaces,
 * the bit mask for the luminance portion of the pixel is then
 * ddpf.dwBumpLuminanceBitMask
 */
#define DDPF_BUMPLUMINANCE                      0x00040000l

/*
 * Bump map dUdV data in the pixel format is valid.
 */
#define DDPF_BUMPDUDV                           0x00080000l


/*===========================================================================
 *
 *
 * DIRECTDRAW CALLBACK FLAGS
 *
 *
 *==========================================================================*/

/****************************************************************************
 *
 * DIRECTDRAW ENUMSURFACES FLAGS
 *
 ****************************************************************************/

/*
 * Enumerate all of the surfaces that meet the search criterion.
 */
#define DDENUMSURFACES_ALL                      0x00000001l

/*
 * A search hit is a surface that matches the surface description.
 */
#define DDENUMSURFACES_MATCH                    0x00000002l

/*
 * A search hit is a surface that does not match the surface description.
 */
#define DDENUMSURFACES_NOMATCH                  0x00000004l

/*
 * Enumerate the first surface that can be created which meets the search criterion.
 */
#define DDENUMSURFACES_CANBECREATED             0x00000008l

/*
 * Enumerate the surfaces that already exist that meet the search criterion.
 */
#define DDENUMSURFACES_DOESEXIST                0x00000010l


/****************************************************************************
 *
 * DIRECTDRAW SETDISPLAYMODE FLAGS
 *
 ****************************************************************************/

/*
 * The desired mode is a standard VGA mode
 */
#define DDSDM_STANDARDVGAMODE                   0x00000001l


/****************************************************************************
 *
 * DIRECTDRAW ENUMDISPLAYMODES FLAGS
 *
 ****************************************************************************/

/*
 * Enumerate Modes with different refresh rates.  EnumDisplayModes guarantees
 * that a particular mode will be enumerated only once.  This flag specifies whether
 * the refresh rate is taken into account when determining if a mode is unique.
 */
#define DDEDM_REFRESHRATES                      0x00000001l

/*
 * Enumerate VGA modes. Specify this flag if you wish to enumerate supported VGA
 * modes such as mode 0x13 in addition to the usual ModeX modes (which are always
 * enumerated if the application has previously called SetCooperativeLevel with the
 * DDSCL_ALLOWMODEX flag set).
 */
#define DDEDM_STANDARDVGAMODES                  0x00000002L


/****************************************************************************
 *
 * DIRECTDRAW SETCOOPERATIVELEVEL FLAGS
 *
 ****************************************************************************/

/*
 * Exclusive mode owner will be responsible for the entire primary surface.
 * GDI can be ignored. used with DD
 */
#define DDSCL_FULLSCREEN                        0x00000001l

/*
 * allow CTRL_ALT_DEL to work while in fullscreen exclusive mode
 */
#define DDSCL_ALLOWREBOOT                       0x00000002l

/*
 * prevents DDRAW from modifying the application window.
 * prevents DDRAW from minimize/restore the application window on activation.
 */
#define DDSCL_NOWINDOWCHANGES                   0x00000004l

/*
 * app wants to work as a regular Windows application
 */
#define DDSCL_NORMAL                            0x00000008l

/*
 * app wants exclusive access
 */
#define DDSCL_EXCLUSIVE                         0x00000010l


/*
 * app can deal with non-windows display modes
 */
#define DDSCL_ALLOWMODEX                        0x00000040l

/*
 * this window will receive the focus messages
 */
#define DDSCL_SETFOCUSWINDOW                    0x00000080l

/*
 * this window is associated with the DDRAW object and will
 * cover the screen in fullscreen mode
 */
#define DDSCL_SETDEVICEWINDOW                   0x00000100l

/*
 * app wants DDRAW to create a window to be associated with the
 * DDRAW object
 */
#define DDSCL_CREATEDEVICEWINDOW                0x00000200l

/*
 * App explicitly asks DDRAW/D3D to be multithread safe. This makes D3D
 * take the global crtisec more frequently.
 */
#define DDSCL_MULTITHREADED                     0x00000400l

/*
 * App specifies that it would like to keep the FPU set up for optimal Direct3D
 * performance (single precision and exceptions disabled) so Direct3D
 * does not need to explicitly set the FPU each time. This is assumed by
 * default in DirectX 7. See also DDSCL_FPUPRESERVE
 */
#define DDSCL_FPUSETUP                          0x00000800l

/*
 * App specifies that it needs either double precision FPU or FPU exceptions
 * enabled. This makes Direct3D explicitly set the FPU state eah time it is
 * called. Setting the flag will reduce Direct3D performance. The flag is
 * assumed by default in DirectX 6 and earlier. See also DDSCL_FPUSETUP
 */
#define DDSCL_FPUPRESERVE                          0x00001000l


/****************************************************************************
 *
 * DIRECTDRAW BLT FLAGS
 *
 ****************************************************************************/

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the destination surface as the alpha channel for this blt.
 */
#define DDBLT_ALPHADEST                         0x00000001l

/*
 * Use the dwConstAlphaDest field in the DDBLTFX structure as the alpha channel
 * for the destination surface for this blt.
 */
#define DDBLT_ALPHADESTCONSTOVERRIDE            0x00000002l

/*
 * The NEG suffix indicates that the destination surface becomes more
 * transparent as the alpha value increases. (0 is opaque)
 */
#define DDBLT_ALPHADESTNEG                      0x00000004l

/*
 * Use the lpDDSAlphaDest field in the DDBLTFX structure as the alpha
 * channel for the destination for this blt.
 */
#define DDBLT_ALPHADESTSURFACEOVERRIDE          0x00000008l

/*
 * Use the dwAlphaEdgeBlend field in the DDBLTFX structure as the alpha channel
 * for the edges of the image that border the color key colors.
 */
#define DDBLT_ALPHAEDGEBLEND                    0x00000010l

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the source surface as the alpha channel for this blt.
 */
#define DDBLT_ALPHASRC                          0x00000020l

/*
 * Use the dwConstAlphaSrc field in the DDBLTFX structure as the alpha channel
 * for the source for this blt.
 */
#define DDBLT_ALPHASRCCONSTOVERRIDE             0x00000040l

/*
 * The NEG suffix indicates that the source surface becomes more transparent
 * as the alpha value increases. (0 is opaque)
 */
#define DDBLT_ALPHASRCNEG                       0x00000080l

/*
 * Use the lpDDSAlphaSrc field in the DDBLTFX structure as the alpha channel
 * for the source for this blt.
 */
#define DDBLT_ALPHASRCSURFACEOVERRIDE           0x00000100l

/*
 * Do this blt asynchronously through the FIFO in the order received.  If
 * there is no room in the hardware FIFO fail the call.
 */
#define DDBLT_ASYNC                             0x00000200l

/*
 * Uses the dwFillColor field in the DDBLTFX structure as the RGB color
 * to fill the destination rectangle on the destination surface with.
 */
#define DDBLT_COLORFILL                         0x00000400l

/*
 * Uses the dwDDFX field in the DDBLTFX structure to specify the effects
 * to use for the blt.
 */
#define DDBLT_DDFX                              0x00000800l

/*
 * Uses the dwDDROPS field in the DDBLTFX structure to specify the ROPS
 * that are not part of the Win32 API.
 */
#define DDBLT_DDROPS                            0x00001000l

/*
 * Use the color key associated with the destination surface.
 */
#define DDBLT_KEYDEST                           0x00002000l

/*
 * Use the dckDestColorkey field in the DDBLTFX structure as the color key
 * for the destination surface.
 */
#define DDBLT_KEYDESTOVERRIDE                   0x00004000l

/*
 * Use the color key associated with the source surface.
 */
#define DDBLT_KEYSRC                            0x00008000l

/*
 * Use the dckSrcColorkey field in the DDBLTFX structure as the color key
 * for the source surface.
 */
#define DDBLT_KEYSRCOVERRIDE                    0x00010000l

/*
 * Use the dwROP field in the DDBLTFX structure for the raster operation
 * for this blt.  These ROPs are the same as the ones defined in the Win32 API.
 */
#define DDBLT_ROP                               0x00020000l

/*
 * Use the dwRotationAngle field in the DDBLTFX structure as the angle
 * (specified in 1/100th of a degree) to rotate the surface.
 */
#define DDBLT_ROTATIONANGLE                     0x00040000l

/*
 * Z-buffered blt using the z-buffers attached to the source and destination
 * surfaces and the dwZBufferOpCode field in the DDBLTFX structure as the
 * z-buffer opcode.
 */
#define DDBLT_ZBUFFER                           0x00080000l

/*
 * Z-buffered blt using the dwConstDest Zfield and the dwZBufferOpCode field
 * in the DDBLTFX structure as the z-buffer and z-buffer opcode respectively
 * for the destination.
 */
#define DDBLT_ZBUFFERDESTCONSTOVERRIDE          0x00100000l

/*
 * Z-buffered blt using the lpDDSDestZBuffer field and the dwZBufferOpCode
 * field in the DDBLTFX structure as the z-buffer and z-buffer opcode
 * respectively for the destination.
 */
#define DDBLT_ZBUFFERDESTOVERRIDE               0x00200000l

/*
 * Z-buffered blt using the dwConstSrcZ field and the dwZBufferOpCode field
 * in the DDBLTFX structure as the z-buffer and z-buffer opcode respectively
 * for the source.
 */
#define DDBLT_ZBUFFERSRCCONSTOVERRIDE           0x00400000l

/*
 * Z-buffered blt using the lpDDSSrcZBuffer field and the dwZBufferOpCode
 * field in the DDBLTFX structure as the z-buffer and z-buffer opcode
 * respectively for the source.
 */
#define DDBLT_ZBUFFERSRCOVERRIDE                0x00800000l

/*
 * wait until the device is ready to handle the blt
 * this will cause blt to not return DDERR_WASSTILLDRAWING
 */
#define DDBLT_WAIT                              0x01000000l

/*
 * Uses the dwFillDepth field in the DDBLTFX structure as the depth value
 * to fill the destination rectangle on the destination Z-buffer surface
 * with.
 */
#define DDBLT_DEPTHFILL                         0x02000000l


/*
 * Return immediately (with DDERR_WASSTILLDRAWING) if the device is not
 * ready to schedule the blt at the time Blt() is called.
 */
#define DDBLT_DONOTWAIT                         0x08000000l

/*
 * These flags indicate a presentation blt (i.e. a blt
 * that moves surface contents from an offscreen back buffer to the primary
 * surface). The driver is not allowed to "queue"  more than three such blts.
 * The "end" of the presentation blt is indicated, since the
 * blt may be clipped, in which case the runtime will call the driver with 
 * several blts. All blts (even if not clipped) are tagged with DDBLT_PRESENTATION
 * and the last (even if not clipped) additionally with DDBLT_LAST_PRESENTATION.
 * Thus the true rule is that the driver must not schedule a DDBLT_PRESENTATION
 * blt if there are 3 or more DDBLT_PRESENTLAST blts in the hardware pipe.
 * If there are such blts in the pipe, the driver should return DDERR_WASSTILLDRAWING
 * until the oldest queued DDBLT_LAST_PRESENTATION blts has been retired (i.e. the
 * pixels have been actually written to the primary surface). Once the oldest blt
 * has been retired, the driver is free to schedule the current blt.
 * The goal is to provide a mechanism whereby the device's hardware queue never
 * gets more than 3 frames ahead of the frames being generated by the application.
 * When excessive queueing occurs, applications become unusable because the application
 * visibly lags user input, and such problems make windowed interactive applications impossible.
 * Some drivers may not have sufficient knowledge of their hardware's FIFO to know
 * when a certain blt has been retired. Such drivers should code cautiously, and 
 * simply not allow any frames to be queued at all. DDBLT_LAST_PRESENTATION should cause
 * such drivers to return DDERR_WASSTILLDRAWING until the accelerator is completely
 * finished- exactly as if the application had called Lock on the source surface
 * before calling Blt. 
 * In other words, the driver is allowed and encouraged to 
 * generate as much latency as it can, but never more than 3 frames worth.
 * Implementation detail: Drivers should count blts against the SOURCE surface, not
 * against the primary surface. This enables multiple parallel windowed application
 * to function more optimally.
 * This flag is passed only to DX8 or higher drivers.
 *
 * APPLICATIONS DO NOT SET THESE FLAGS. THEY ARE SET BY THE DIRECTDRAW RUNTIME.
 * 
 */
#define DDBLT_PRESENTATION                      0x10000000l
#define DDBLT_LAST_PRESENTATION                 0x20000000l

/*
 * If DDBLT_EXTENDED_FLAGS is set, then the driver should re-interpret
 * other flags according to the definitions that follow.
 * For example, bit 0 (0x00000001L) means DDBLT_ALPHADEST, unless
 * DDBLT_EXTENDED_FLAGS is also set, in which case bit 0 means
 * DDBLT_EXTENDED_LINEAR_CONTENT.
 * Only DirectX9 and higher drivers will be given extended blt flags.
 * Only flags explicitly mentioned here should be re-interpreted.
 * All other flags retain their original meanings.
 *
 * List of re-interpreted flags:
 *
 * Bit Hex value   New meaning                                  old meaning
 * ---------------------------------------------------------------
 *  2  0x00000004  DDBLT_EXTENDED_LINEAR_CONTENT                DDBLT_ALPHADESTNEG
 *  4  0x00000010  DDBLT_EXTENDED_PRESENTATION_STRETCHFACTOR    DDBLT_ALPHAEDGEBLEND
 *
 *
 * NOTE: APPLICATIONS SHOULD NOT SET THIS FLAG. THIS FLAG IS INTENDED
 * FOR USE BY THE DIRECT3D RUNTIME.
 */
#define DDBLT_EXTENDED_FLAGS                    0x40000000l

/*
 * EXTENDED FLAG. SEE DEFINITION OF DDBLT_EXTENDED_FLAGS.
 * This flag indidcates that the source surface contains content in a
 * linear color space. The driver may perform gamma correction to the
 * desktop color space (i.e. sRGB, gamma 2.2) as part of this blt.
 * If the device can perform such a conversion as part of the copy,
 * the driver should also set D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION
 *
 * NOTE: APPLICATIONS SHOULD NOT SET THIS FLAG. THIS FLAG IS INTENDED
 * FOR USE BY THE DIRECT3D RUNTIME. Use IDirect3DSwapChain9::Present
 * and specify D3DPRESENT_LINEAR_CONTENT in order to use this functionality.
 */ 
#define DDBLT_EXTENDED_LINEAR_CONTENT           0x00000004l


/****************************************************************************
 *
 * BLTFAST FLAGS
 *
 ****************************************************************************/

#define DDBLTFAST_NOCOLORKEY                    0x00000000
#define DDBLTFAST_SRCCOLORKEY                   0x00000001
#define DDBLTFAST_DESTCOLORKEY                  0x00000002
#define DDBLTFAST_WAIT                          0x00000010
#define DDBLTFAST_DONOTWAIT                     0x00000020

/****************************************************************************
 *
 * FLIP FLAGS
 *
 ****************************************************************************/

#define DDFLIP_WAIT                          0x00000001L

/*
 * Indicates that the target surface contains the even field of video data.
 * This flag is only valid with an overlay surface.
 */
#define DDFLIP_EVEN                          0x00000002L

/*
 * Indicates that the target surface contains the odd field of video data.
 * This flag is only valid with an overlay surface.
 */
#define DDFLIP_ODD                           0x00000004L

/*
 * Causes DirectDraw to perform the physical flip immediately and return
 * to the application. Typically, what was the front buffer but is now the back
 * buffer will still be visible (depending on timing) until the next vertical
 * retrace. Subsequent operations involving the two flipped surfaces will
 * not check to see if the physical flip has finished (i.e. will not return
 * DDERR_WASSTILLDRAWING for that reason (but may for other reasons)).
 * This allows an application to perform Flips at a higher frequency than the
 * monitor refresh rate, but may introduce visible artifacts.
 * Only effective if DDCAPS2_FLIPNOVSYNC is set. If that bit is not set,
 * DDFLIP_NOVSYNC has no effect.
 */
#define DDFLIP_NOVSYNC                       0x00000008L


/*
 * Flip Interval Flags. These flags indicate how many vertical retraces to wait between
 * each flip. The default is one. DirectDraw will return DDERR_WASSTILLDRAWING for each
 * surface involved in the flip until the specified number of vertical retraces has
 * ocurred. Only effective if DDCAPS2_FLIPINTERVAL is set. If that bit is not set,
 * DDFLIP_INTERVALn has no effect.
 */

/*
 * DirectDraw will flip on every other vertical sync
 */
#define DDFLIP_INTERVAL2                     0x02000000L


/*
 * DirectDraw will flip on every third vertical sync
 */
#define DDFLIP_INTERVAL3                     0x03000000L


/*
 * DirectDraw will flip on every fourth vertical sync
 */
#define DDFLIP_INTERVAL4                     0x04000000L

/*
 * DirectDraw will flip and display a main stereo surface
 */
#define DDFLIP_STEREO                        0x00000010L

/*
 * On IDirectDrawSurface7 and higher interfaces, the default is DDFLIP_WAIT. If you wish
 * to override the default and use time when the accelerator is busy (as denoted by
 * the DDERR_WASSTILLDRAWING return code) then use DDFLIP_DONOTWAIT.
 */
#define DDFLIP_DONOTWAIT                     0x00000020L


/****************************************************************************
 *
 * DIRECTDRAW SURFACE OVERLAY FLAGS
 *
 ****************************************************************************/

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the destination surface as the alpha channel for the
 * destination overlay.
 */
#define DDOVER_ALPHADEST                        0x00000001l

/*
 * Use the dwConstAlphaDest field in the DDOVERLAYFX structure as the
 * destination alpha channel for this overlay.
 */
#define DDOVER_ALPHADESTCONSTOVERRIDE           0x00000002l

/*
 * The NEG suffix indicates that the destination surface becomes more
 * transparent as the alpha value increases.
 */
#define DDOVER_ALPHADESTNEG                     0x00000004l

/*
 * Use the lpDDSAlphaDest field in the DDOVERLAYFX structure as the alpha
 * channel destination for this overlay.
 */
#define DDOVER_ALPHADESTSURFACEOVERRIDE         0x00000008l

/*
 * Use the dwAlphaEdgeBlend field in the DDOVERLAYFX structure as the alpha
 * channel for the edges of the image that border the color key colors.
 */
#define DDOVER_ALPHAEDGEBLEND                   0x00000010l

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the source surface as the source alpha channel for this overlay.
 */
#define DDOVER_ALPHASRC                         0x00000020l

/*
 * Use the dwConstAlphaSrc field in the DDOVERLAYFX structure as the source
 * alpha channel for this overlay.
 */
#define DDOVER_ALPHASRCCONSTOVERRIDE            0x00000040l

/*
 * The NEG suffix indicates that the source surface becomes more transparent
 * as the alpha value increases.
 */
#define DDOVER_ALPHASRCNEG                      0x00000080l

/*
 * Use the lpDDSAlphaSrc field in the DDOVERLAYFX structure as the alpha channel
 * source for this overlay.
 */
#define DDOVER_ALPHASRCSURFACEOVERRIDE          0x00000100l

/*
 * Turn this overlay off.
 */
#define DDOVER_HIDE                             0x00000200l

/*
 * Use the color key associated with the destination surface.
 */
#define DDOVER_KEYDEST                          0x00000400l

/*
 * Use the dckDestColorkey field in the DDOVERLAYFX structure as the color key
 * for the destination surface
 */
#define DDOVER_KEYDESTOVERRIDE                  0x00000800l

/*
 * Use the color key associated with the source surface.
 */
#define DDOVER_KEYSRC                           0x00001000l

/*
 * Use the dckSrcColorkey field in the DDOVERLAYFX structure as the color key
 * for the source surface.
 */
#define DDOVER_KEYSRCOVERRIDE                   0x00002000l

/*
 * Turn this overlay on.
 */
#define DDOVER_SHOW                             0x00004000l

/*
 * Add a dirty rect to an emulated overlayed surface.
 */
#define DDOVER_ADDDIRTYRECT                     0x00008000l

/*
 * Redraw all dirty rects on an emulated overlayed surface.
 */
#define DDOVER_REFRESHDIRTYRECTS                0x00010000l

/*
 * Redraw the entire surface on an emulated overlayed surface.
 */
#define DDOVER_REFRESHALL                      0x00020000l


/*
 * Use the overlay FX flags to define special overlay FX
 */
#define DDOVER_DDFX                             0x00080000l

/*
 * Autoflip the overlay when ever the video port autoflips
 */
#define DDOVER_AUTOFLIP                         0x00100000l

/*
 * Display each field of video port data individually without
 * causing any jittery artifacts
 */
#define DDOVER_BOB                              0x00200000l

/*
 * Indicates that bob/weave decisions should not be overridden by other
 * interfaces.
 */
#define DDOVER_OVERRIDEBOBWEAVE                 0x00400000l

/*
 * Indicates that the surface memory is composed of interleaved fields.
 */
#define DDOVER_INTERLEAVED                      0x00800000l

/*
 * Indicates that bob will be performed using hardware rather than
 * software or emulated.
 */
#define DDOVER_BOBHARDWARE                      0x01000000l

/*
 * Indicates that overlay FX structure contains valid ARGB scaling factors.
 */
#define DDOVER_ARGBSCALEFACTORS                 0x02000000l

/*
 * Indicates that ARGB scaling factors can be degraded to fit driver capabilities.
 */
#define DDOVER_DEGRADEARGBSCALING               0x04000000l


/****************************************************************************
 *
 * DIRECTDRAWSURFACE LOCK FLAGS
 *
 ****************************************************************************/

/*
 * The default.  Set to indicate that Lock should return a valid memory pointer
 * to the top of the specified rectangle.  If no rectangle is specified then a
 * pointer to the top of the surface is returned.
 */
#define DDLOCK_SURFACEMEMORYPTR                 0x00000000L     // default

/*
 * Set to indicate that Lock should wait until it can obtain a valid memory
 * pointer before returning.  If this bit is set, Lock will never return
 * DDERR_WASSTILLDRAWING.
 */
#define DDLOCK_WAIT                             0x00000001L

/*
 * Set if an event handle is being passed to Lock.  Lock will trigger the event
 * when it can return the surface memory pointer requested.
 */
#define DDLOCK_EVENT                            0x00000002L

/*
 * Indicates that the surface being locked will only be read from.
 */
#define DDLOCK_READONLY                         0x00000010L

/*
 * Indicates that the surface being locked will only be written to
 */
#define DDLOCK_WRITEONLY                        0x00000020L


/*
 * Indicates that a system wide lock should not be taken when this surface
 * is locked. This has several advantages (cursor responsiveness, ability
 * to call more Windows functions, easier debugging) when locking video
 * memory surfaces. However, an application specifying this flag must
 * comply with a number of conditions documented in the help file.
 * Furthermore, this flag cannot be specified when locking the primary.
 */
#define DDLOCK_NOSYSLOCK                        0x00000800L

/*
 * Used only with Direct3D Vertex Buffer Locks. Indicates that no vertices
 * that were referred to in Draw*PrimtiveVB calls since the start of the
 * frame (or the last lock without this flag) will be modified during the
 * lock. This can be useful when one is only appending data to the vertex
 * buffer
 */
#define DDLOCK_NOOVERWRITE                      0x00001000L

/*
 * Indicates that no assumptions will be made about the contents of the
 * surface or vertex buffer during this lock.
 * This enables two things:
 * -    Direct3D or the driver may provide an alternative memory
 *      area as the vertex buffer. This is useful when one plans to clear the
 *      contents of the vertex buffer and fill in new data.
 * -    Drivers sometimes store surface data in a re-ordered format.
 *      When the application locks the surface, the driver is forced to un-re-order
 *      the surface data before allowing the application to see the surface contents.
 *      This flag is a hint to the driver that it can skip the un-re-ordering process
 *      since the application plans to overwrite every single pixel in the surface
 *      or locked rectangle (and so erase any un-re-ordered pixels anyway).
 *      Applications should always set this flag when they intend to overwrite the entire
 *      surface or locked rectangle.
 */
#define DDLOCK_DISCARDCONTENTS                  0x00002000L
 /*
  * DDLOCK_OKTOSWAP is an older, less informative name for DDLOCK_DISCARDCONTENTS
  */
#define DDLOCK_OKTOSWAP                         0x00002000L

/*
 * On IDirectDrawSurface7 and higher interfaces, the default is DDLOCK_WAIT. If you wish
 * to override the default and use time when the accelerator is busy (as denoted by
 * the DDERR_WASSTILLDRAWING return code) then use DDLOCK_DONOTWAIT.
 */
#define DDLOCK_DONOTWAIT                        0x00004000L

/*
 * This indicates volume texture lock with front and back specified.
 */
#define DDLOCK_HASVOLUMETEXTUREBOXRECT          0x00008000L

/*
 * This indicates that the driver should not update dirty rect information for this lock.
 */
#define DDLOCK_NODIRTYUPDATE                    0x00010000L


/****************************************************************************
 *
 * DIRECTDRAWSURFACE PAGELOCK FLAGS
 *
 ****************************************************************************/

/*
 * No flags defined at present
 */


/****************************************************************************
 *
 * DIRECTDRAWSURFACE PAGEUNLOCK FLAGS
 *
 ****************************************************************************/

/*
 * No flags defined at present
 */


/****************************************************************************
 *
 * DIRECTDRAWSURFACE BLT FX FLAGS
 *
 ****************************************************************************/

/*
 * If stretching, use arithmetic stretching along the Y axis for this blt.
 */
#define DDBLTFX_ARITHSTRETCHY                   0x00000001l

/*
 * Do this blt mirroring the surface left to right.  Spin the
 * surface around its y-axis.
 */
#define DDBLTFX_MIRRORLEFTRIGHT                 0x00000002l

/*
 * Do this blt mirroring the surface up and down.  Spin the surface
 * around its x-axis.
 */
#define DDBLTFX_MIRRORUPDOWN                    0x00000004l

/*
 * Schedule this blt to avoid tearing.
 */
#define DDBLTFX_NOTEARING                       0x00000008l

/*
 * Do this blt rotating the surface one hundred and eighty degrees.
 */
#define DDBLTFX_ROTATE180                       0x00000010l

/*
 * Do this blt rotating the surface two hundred and seventy degrees.
 */
#define DDBLTFX_ROTATE270                       0x00000020l

/*
 * Do this blt rotating the surface ninety degrees.
 */
#define DDBLTFX_ROTATE90                        0x00000040l

/*
 * Do this z blt using dwZBufferLow and dwZBufferHigh as  range values
 * specified to limit the bits copied from the source surface.
 */
#define DDBLTFX_ZBUFFERRANGE                    0x00000080l

/*
 * Do this z blt adding the dwZBufferBaseDest to each of the sources z values
 * before comparing it with the desting z values.
 */
#define DDBLTFX_ZBUFFERBASEDEST                 0x00000100l

/****************************************************************************
 *
 * DIRECTDRAWSURFACE OVERLAY FX FLAGS
 *
 ****************************************************************************/

/*
 * If stretching, use arithmetic stretching along the Y axis for this overlay.
 */
#define DDOVERFX_ARITHSTRETCHY                  0x00000001l

/*
 * Mirror the overlay across the vertical axis
 */
#define DDOVERFX_MIRRORLEFTRIGHT                0x00000002l

/*
 * Mirror the overlay across the horizontal axis
 */
#define DDOVERFX_MIRRORUPDOWN                   0x00000004l

/*
 * Deinterlace the overlay, if possible
 */
#define DDOVERFX_DEINTERLACE                    0x00000008l


/****************************************************************************
 *
 * DIRECTDRAW WAITFORVERTICALBLANK FLAGS
 *
 ****************************************************************************/

/*
 * return when the vertical blank interval begins
 */
#define DDWAITVB_BLOCKBEGIN                     0x00000001l

/*
 * set up an event to trigger when the vertical blank begins
 */
#define DDWAITVB_BLOCKBEGINEVENT                0x00000002l

/*
 * return when the vertical blank interval ends and display begins
 */
#define DDWAITVB_BLOCKEND                       0x00000004l

/****************************************************************************
 *
 * DIRECTDRAW GETFLIPSTATUS FLAGS
 *
 ****************************************************************************/

/*
 * is it OK to flip now?
 */
#define DDGFS_CANFLIP                   0x00000001l

/*
 * is the last flip finished?
 */
#define DDGFS_ISFLIPDONE                0x00000002l

/****************************************************************************
 *
 * DIRECTDRAW GETBLTSTATUS FLAGS
 *
 ****************************************************************************/

/*
 * is it OK to blt now?
 */
#define DDGBS_CANBLT                    0x00000001l

/*
 * is the blt to the surface finished?
 */
#define DDGBS_ISBLTDONE                 0x00000002l


/****************************************************************************
 *
 * DIRECTDRAW ENUMOVERLAYZORDER FLAGS
 *
 ****************************************************************************/

/*
 * Enumerate overlays back to front.
 */
#define DDENUMOVERLAYZ_BACKTOFRONT      0x00000000l

/*
 * Enumerate overlays front to back
 */
#define DDENUMOVERLAYZ_FRONTTOBACK      0x00000001l

/****************************************************************************
 *
 * DIRECTDRAW UPDATEOVERLAYZORDER FLAGS
 *
 ****************************************************************************/

/*
 * Send overlay to front
 */
#define DDOVERZ_SENDTOFRONT             0x00000000l

/*
 * Send overlay to back
 */
#define DDOVERZ_SENDTOBACK              0x00000001l

/*
 * Move Overlay forward
 */
#define DDOVERZ_MOVEFORWARD             0x00000002l

/*
 * Move Overlay backward
 */
#define DDOVERZ_MOVEBACKWARD            0x00000003l

/*
 * Move Overlay in front of relative surface
 */
#define DDOVERZ_INSERTINFRONTOF         0x00000004l

/*
 * Move Overlay in back of relative surface
 */
#define DDOVERZ_INSERTINBACKOF          0x00000005l


/****************************************************************************
 *
 * DIRECTDRAW SETGAMMARAMP FLAGS
 *
 ****************************************************************************/

/*
 * Request calibrator to adjust the gamma ramp according to the physical
 * properties of the display so that the result should appear identical
 * on all systems.
 */
#define DDSGR_CALIBRATE                        0x00000001L


/****************************************************************************
 *
 * DIRECTDRAW STARTMODETEST FLAGS
 *
 ****************************************************************************/

/*
 * Indicates that the mode being tested has passed
 */
#define DDSMT_ISTESTREQUIRED                   0x00000001L


/****************************************************************************
 *
 * DIRECTDRAW EVALUATEMODE FLAGS
 *
 ****************************************************************************/

/*
 * Indicates that the mode being tested has passed
 */
#define DDEM_MODEPASSED                        0x00000001L

/*
 * Indicates that the mode being tested has failed
 */
#define DDEM_MODEFAILED                        0x00000002L

#ifndef DUMMYUNIONNAMEN
#if defined(__cplusplus) || !defined(NONAMELESSUNION)
#define DUMMYUNIONNAMEN(n)
#else
#define DUMMYUNIONNAMEN(n)      u##n
#endif
#endif

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((UINT32)(INT8)(ch0) | ((UINT32)(INT8)(ch1) << 8) |   \
                ((UINT32)(INT8)(ch2) << 16) | ((UINT32)(INT8)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

/*
 * FOURCC codes for DX compressed-texture pixel formats
 */
#define FOURCC_DXT1  (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2  (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3  (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4  (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5  (MAKEFOURCC('D','X','T','5'))

typedef struct PACKED _DDARGB
{
    INT8 blue;
    INT8 green;
    INT8 red;
    INT8 alpha;
} DDARGB;

typedef DDARGB  *LPDDARGB;

/*
 * This version of the structure remains for backwards source compatibility.
 * The DDARGB structure is the one that should be used for all DirectDraw APIs.
 */
typedef struct _DDRGBA
{
    INT8 red;
    INT8 green;
    INT8 blue;
    INT8 alpha;
} DDRGBA;

typedef DDRGBA  *LPDDRGBA;


/*
 * DDCOLORKEY
 */
typedef struct PACKED _DDCOLORKEY
{
    UINT32       dwColorSpaceLowValue;   // low boundary of color space that is to
                                        // be treated as Color Key, inclusive
    UINT32       dwColorSpaceHighValue;  // high boundary of color space that is
                                        // to be treated as Color Key, inclusive
} DDCOLORKEY;

typedef DDCOLORKEY * LPDDCOLORKEY;

typedef struct PACKED _DDSCAPS2
{
    UINT32       dwCaps;         // capabilities of surface wanted
    UINT32       dwCaps2;
    UINT32       dwCaps3;
    union
    {
        UINT32       dwCaps4;
        UINT32       dwVolumeDepth;
    } DUMMYUNIONNAMEN(1);
} DDSCAPS2;

typedef DDSCAPS2 * LPDDSCAPS2;

/*
 * DDPIXELFORMAT
 */
typedef struct PACKED _DDPIXELFORMAT
{
    UINT32       dwSize;                 // size of structure
    UINT32       dwFlags;                // pixel format flags
    UINT32       dwFourCC;               // (FOURCC code)
    union
    {
        UINT32   dwRGBBitCount;          // how many bits per pixel
        UINT32   dwYUVBitCount;          // how many bits per pixel
        UINT32   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
        UINT32   dwAlphaBitDepth;        // how many bits for alpha channels
        UINT32   dwLuminanceBitCount;    // how many bits per pixel
        UINT32   dwBumpBitCount;         // how many bits per "buxel", total
        UINT32   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
                                        // format list and if DDPF_D3DFORMAT is set
    } DUMMYUNIONNAMEN(1);
    union
    {
        UINT32   dwRBitMask;             // mask for red bit
        UINT32   dwYBitMask;             // mask for Y bits
        UINT32   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
        UINT32   dwLuminanceBitMask;     // mask for luminance bits
        UINT32   dwBumpDuBitMask;        // mask for bump map U delta bits
        UINT32   dwOperations;           // DDPF_D3DFORMAT Operations
    } DUMMYUNIONNAMEN(2);
    union
    {
        UINT32   dwGBitMask;             // mask for green bits
        UINT32   dwUBitMask;             // mask for U bits
        UINT32   dwZBitMask;             // mask for Z bits
        UINT32   dwBumpDvBitMask;        // mask for bump map V delta bits
        struct
        {
            UINT16    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
            UINT16    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
        } MultiSampleCaps;

    } DUMMYUNIONNAMEN(3);
    union
    {
        UINT32   dwBBitMask;             // mask for blue bits
        UINT32   dwVBitMask;             // mask for V bits
        UINT32   dwStencilBitMask;       // mask for stencil bits
        UINT32   dwBumpLuminanceBitMask; // mask for luminance in bump map
    } DUMMYUNIONNAMEN(4);
    union
    {
        UINT32   dwRGBAlphaBitMask;      // mask for alpha channel
        UINT32   dwYUVAlphaBitMask;      // mask for alpha channel
        UINT32   dwLuminanceAlphaBitMask;// mask for alpha channel
        UINT32   dwRGBZBitMask;          // mask for Z channel
        UINT32   dwYUVZBitMask;          // mask for Z channel
    } DUMMYUNIONNAMEN(5);
} DDPIXELFORMAT;

typedef DDPIXELFORMAT * LPDDPIXELFORMAT;


typedef struct PACKED _tagDDSURFACEDESC2
{
    UINT32               dwSize;                 // size of the DDSURFACEDESC structure
    UINT32               dwFlags;                // determines what fields are valid
    UINT32               dwHeight;               // height of surface to be created
    UINT32               dwWidth;                // width of input surface
    union
    {
        INT32            lPitch;                 // distance to start of next line (return value only)
        UINT32           dwLinearSize;           // Formless late-allocated optimized surface size
    } DUMMYUNIONNAMEN(1);
    union
    {
        UINT32           dwBackBufferCount;      // number of back buffers requested
        UINT32           dwDepth;                // the depth if this is a volume texture 
    } DUMMYUNIONNAMEN(5);
    union
    {
        UINT32           dwMipMapCount;          // number of mip-map levels requestde
                                                // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
        UINT32           dwRefreshRate;          // refresh rate (used when display mode is described)
        UINT32           dwSrcVBHandle;          // The source used in VB::Optimize
    } DUMMYUNIONNAMEN(2);
    UINT32               dwAlphaBitDepth;        // depth of alpha buffer requested
    UINT32               dwReserved;             // reserved
    void *               lpSurface;              // pointer to the associated surface memory
    union
    {
        DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
        UINT32           dwEmptyFaceColor;       // Physical color for empty cubemap faces
    } DUMMYUNIONNAMEN(3);
    DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
    DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
    DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
    union
    {
        DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
        UINT32           dwFVF;                  // vertex format description of vertex buffers
    } DUMMYUNIONNAMEN(4);
    DDSCAPS2            ddsCaps;                // direct draw surface capabilities
    UINT32               dwTextureStage;         // stage in multitexture cascade
} DDSURFACEDESC2;

#endif

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
