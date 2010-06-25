/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkTextureObject.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkTextureObject - abstracts an OpenGL texture object.
// .SECTION Description
// vtkTextureObject represents an OpenGL texture object. It provides API to
// create textures using data already loaded into pixel buffer objects. It can
// also be used to create textures without uploading any data.
// .SECTION Caveats
// DON'T PLAY WITH IT YET.
#ifndef __vtkTextureObject_h
#define __vtkTextureObject_h

#include "vtkObject.h"
#include "vtkWeakPointer.h" // needed for vtkWeakPointer.

class vtkRenderWindow;
class vtkOpenGLExtensionManager;
class vtkPixelBufferObject;

class VTK_RENDERING_EXPORT vtkTextureObject : public vtkObject
{
public:

  //BTX
  // DepthTextureCompareFunction values.
  enum 
  {
    Lequal=0, // r=R<=Dt ? 1.0 : 0.0
    Gequal, // r=R>=Dt ? 1.0 : 0.0
    Less, // r=R<D_t ? 1.0 : 0.0
    Greater, // r=R>Dt ? 1.0 : 0.0
    Equal, // r=R==Dt ? 1.0 : 0.0
    NotEqual, // r=R!=Dt ? 1.0 : 0.0
    AlwaysTrue, //  r=1.0 // WARNING "Always" is macro defined in X11/X.h...
    Never // r=0.0
  };
  
  // DepthTextureMode values.
  enum
  {
    Luminance=0, // (R,G,B,A)=(r,r,r,1)
    Intensity, // (R,G,B,A)=(r,r,r,r)
    Alpha // (R.G.B.A)=(0,0,0,r)
  };
  
  // Wrap values.
  enum
  {
    Clamp=0,
    ClampToEdge,
    Repeat,
    ClampToBorder,
    MirroredRepeat
  };
  
  // MinificationFilter values.
  enum
  {
    Nearest=0,
    Linear,
    NearestMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapNearest,
    LinearMipmapLinear
  };
  //ETX
  
  static vtkTextureObject* New();
  vtkTypeRevisionMacro(vtkTextureObject, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the context. This does not increase the reference count of the 
  // context to avoid reference loops.
  // SetContext() may raise an error is the OpenGL context does not support the
  // required OpenGL extensions.
  void SetContext(vtkRenderWindow*);
  vtkRenderWindow* GetContext();

  // Description:
  // Get the texture dimensions.
  // These are the properties of the OpenGL texture this instance represents.
  vtkGetMacro(Width, unsigned int);
  vtkGetMacro(Height, unsigned int);
  vtkGetMacro(Depth, unsigned int);
  vtkGetMacro(Components, int);

  vtkGetMacro(NumberOfDimensions, int);

  // Description:
  // Returns OpenGL texture target to which the texture is/can be bound.
  vtkGetMacro(Target, int);

  // Description:
  // Returns the OpenGL handle.
  vtkGetMacro(Handle, unsigned int);

  // Description:
  // Activate the texture. The texture must have been created using Create().
  // RenderWindow must be set before calling this. 
  void Bind();
  void UnBind();

  // Description:
  // Tells if the texture object is bound.
  bool IsBound();
  
  // Description:
  // Send all the texture object parameters to the hardware if not done yet.
  // \pre is_bound: IsBound()
  void SendParameters();
  
  // Description:
  // Create a 1D texture using the PBO.
  // Eventually we may start supporting creating a texture from subset of data
  // in the PBO, but for simplicity we'll begin with entire PBO data.
  // numComps must be in [1-4].
  // shaderSupportsTextureInt is true if the shader has an alternate
  // implementation supporting sampler with integer values.
  // Even if the card supports texture int, it does not mean that
  // the implementor of the shader made a version that supports texture int.
  bool Create1D(int numComps,
                vtkPixelBufferObject *pbo,
                bool shaderSupportsTextureInt);

  // Description:
  // Create a 2D texture using the PBO.
  // Eventually we may start supporting creating a texture from subset of data
  // in the PBO, but for simplicity we'll begin with entire PBO data.
  // numComps must be in [1-4].
  bool Create2D(unsigned int width, unsigned int height, int numComps,
                vtkPixelBufferObject *pbo,
                bool shaderSupportsTextureInt);

  // Description:
  // Create a 3D texture using the PBO.
  // Eventually we may start supporting creating a texture from subset of data
  // in the PBO, but for simplicity we'll begin with entire PBO data.
  // numComps must be in [1-4].
  bool Create3D(unsigned int width, unsigned int height, unsigned int depth, 
                int numComps, vtkPixelBufferObject *pbo,
                bool shaderSupportsTextureInt);


  // Description:
  // Create texture without uploading any data.
  // To create a DEPTH_COMPONENT texture, vtktype must be set to VTK_VOID and
  // numComps must be 1.
  bool Create2D(unsigned int width, unsigned int height, int numComps,
                int vtktype,
                bool shaderSupportsTextureInt);
  bool Create3D(unsigned int width, unsigned int height, unsigned int depth,
                int numComps, int vtktype,
                bool shaderSupportsTextureInt);

  // Description:
  // This is used to download raw data from the texture into a pixel bufer. The
  // pixel buffer API can then be used to download the pixel buffer data to CPU
  // arrays. The caller takes on the responsibility of deleting the returns
  // vtkPixelBufferObject once it done with it.
  vtkPixelBufferObject* Download();

  // Description:
  // Get the data type for the texture as a vtk type int i.e. VTK_INT etc.
  int GetDataType();

  int GetInternalFormat(int vtktype, int numComps,
                        bool shaderSupportsTextureInt);
  int GetFormat(int vtktype, int numComps,
                bool shaderSupportsTextureInt);
  
  // Description:
  // Wrap mode for the first texture coordinate "s"
  // Valid values are:
  // - Clamp
  // - ClampToEdge
  // - Repeat
  // - ClampToBorder
  // - MirroredRepeat
  // Initial value is Repeat (as in OpenGL spec)
  vtkGetMacro(WrapS,int);
  vtkSetMacro(WrapS,int);
  
  // Description:
  // Wrap mode for the first texture coordinate "t"
  // Valid values are:
  // - Clamp
  // - ClampToEdge
  // - Repeat
  // - ClampToBorder
  // - MirroredRepeat
  // Initial value is Repeat (as in OpenGL spec)
  vtkGetMacro(WrapT,int);
  vtkSetMacro(WrapT,int);
  
  // Description:
  // Wrap mode for the first texture coordinate "r"
  // Valid values are:
  // - Clamp
  // - ClampToEdge
  // - Repeat
  // - ClampToBorder
  // - MirroredRepeat
  // Initial value is Repeat (as in OpenGL spec)
  vtkGetMacro(WrapR,int);
  vtkSetMacro(WrapR,int);
  
  // Description:
  // Minification filter mode.
  // Valid values are:
  // - Nearest
  // - Linear
  // - NearestMipmapNearest
  // - NearestMipmapLinear
  // - LinearMipmapNearest
  // - LinearMipmapLinear
  // Initial value is NearestMipMapLinear (note initial value in OpenGL spec
  // is NearestMipMapLinear but this is error-prone because it makes the
  // texture object incomplete. ).
  vtkGetMacro(MinificationFilter,int);
  vtkSetMacro(MinificationFilter,int);
  
  // Description:
  // Tells if the magnification mode is linear (true) or nearest (false).
  // Initial value is false (initial value in OpenGL spec is true).
  vtkGetMacro(LinearMagnification,bool);
  vtkSetMacro(LinearMagnification,bool);
  
  // Description:
  // Border Color (RGBA). Each component is in [0.0f,1.0f].
  // Initial value is (0.0f,0.0f,0.0f,0.0f), as in OpenGL spec.
  vtkSetVector4Macro(BorderColor,float);
  vtkGetVector4Macro(BorderColor,float);
  
  // Description:
  // Priority of the texture object to be resident on the card for higher
  // performance in the range [0.0f,1.0f].
  // Initial value is 1.0f, as in OpenGL spec.
  vtkSetMacro(Priority,float);
  vtkGetMacro(Priority,float);
  
  // Description:
  // Lower-clamp the computed LOD against this value. Any float value is valid.
  // Initial value is -1000.0f, as in OpenGL spec.
  vtkSetMacro(MinLOD,float);
  vtkGetMacro(MinLOD,float);
  
  // Description:
  // Upper-clamp the computed LOD against this value. Any float value is valid.
  // Initial value is 1000.0f, as in OpenGL spec.
  vtkSetMacro(MaxLOD,float);
  vtkGetMacro(MaxLOD,float);
  
  // Description:
  // Level of detail of the first texture image. A texture object is a list of
  // texture images. It is a non-negative integer value.
  // Initial value is 0, as in OpenGL spec.
  vtkSetMacro(BaseLevel,int);
  vtkGetMacro(BaseLevel,int);
  
  // Description:
  // Level of detail of the first texture image. A texture object is a list of
  // texture images. It is a non-negative integer value.
  // Initial value is 1000, as in OpenGL spec.
  vtkSetMacro(MaxLevel,int);
  vtkGetMacro(MaxLevel,int);
  
  // Description:
  // Tells if the output of a texture unit with a depth texture uses
  // comparison or not.
  // Comparison happens between D_t the depth texture value in the range [0,1]
  // and with R the interpolated third texture coordinate clamped to range
  // [0,1]. The result of the comparison is noted `r'. If this flag is false,
  // r=D_t.
  // Initial value is false, as in OpenGL spec.
  // Ignored if the texture object is not a depth texture.
  vtkGetMacro(DepthTextureCompare,bool);
  vtkSetMacro(DepthTextureCompare,bool);
  
  // Description:
  // In case DepthTextureCompare is true, specify the comparison function in
  // use. The result of the comparison is noted `r'.
  // Valid values are:
  // - Value
  // - Lequal: r=R<=Dt ? 1.0 : 0.0
  // - Gequal: r=R>=Dt ? 1.0 : 0.0
  // - Less: r=R<D_t ? 1.0 : 0.0
  // - Greater: r=R>Dt ? 1.0 : 0.0
  // - Equal: r=R==Dt ? 1.0 : 0.0
  // - NotEqual: r=R!=Dt ? 1.0 : 0.0
  // - AlwaysTrue: r=1.0
  // - Never: r=0.0
  // If the magnification of minification factor are not nearest, percentage
  // closer filtering (PCF) is used: R is compared to several D_t and r is
  // the average of the comparisons (it is NOT the average of D_t compared
  // once to R).
  // Initial value is Lequal, as in OpenGL spec.
  // Ignored if the texture object is not a depth texture.
  vtkGetMacro(DepthTextureCompareFunction,int);
  vtkSetMacro(DepthTextureCompareFunction,int);
  
  // Description:
  // Defines the mapping from depth component `r' to RGBA components.
  // Ignored if the texture object is not a depth texture.
  // Valid modes are:
  // - Luminance: (R,G,B,A)=(r,r,r,1)
  // - Intensity: (R,G,B,A)=(r,r,r,r)
  // - Alpha: (R.G.B.A)=(0,0,0,r)
  // Initial value is Luminance, as in OpenGL spec.
  vtkGetMacro(DepthTextureMode,int);
  vtkSetMacro(DepthTextureMode,int);
  
  // Description:
  // Tells the hardware to generate mipmap textures from the first texture
  // image at BaseLevel.
  // Initial value is false, as in OpenGL spec.
  vtkGetMacro(GenerateMipmap,bool);
  vtkSetMacro(GenerateMipmap,bool);
  
  // Description:
  // Returns if the context supports the required extensions.
  static bool IsSupported(vtkRenderWindow* renWin);
//BTX
protected:
  vtkTextureObject();
  ~vtkTextureObject();

  // Description:
  // Load all necessary extensions.
  bool LoadRequiredExtensions(vtkOpenGLExtensionManager*);

  // Description:
  // Creates a texture handle if not already created.
  void CreateTexture();

  // Description:
  // Destory the texture.
  void DestroyTexture();

  int NumberOfDimensions;
  unsigned int Width;
  unsigned int Height;
  unsigned int Depth;

  int Target; // GLenum
  int Format; // GLint
  int Type; // GLint
  int Components;

  vtkWeakPointer<vtkRenderWindow> Context;
  unsigned int Handle;
  bool SupportsTextureInteger;

  int WrapS;
  int WrapT;
  int WrapR;
  int MinificationFilter;
  bool LinearMagnification;
  float BorderColor[4];
  
  float Priority;
  float MinLOD;
  float MaxLOD;
  int BaseLevel;
  int MaxLevel;
  
  
  bool DepthTextureCompare;
  int DepthTextureCompareFunction;
  int DepthTextureMode;
  
  bool GenerateMipmap;
  
  vtkTimeStamp SendParametersTime;
  
private:
  vtkTextureObject(const vtkTextureObject&); // Not implemented.
  void operator=(const vtkTextureObject&); // Not implemented.
//ETX
};

#endif

