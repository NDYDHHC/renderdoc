/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Baldur Karlsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#pragma once

// do not include any headers here, they must all be in QRDInterface.h
#include "QRDInterface.h"

struct ICaptureContext;

DOCUMENT("Information about a single resource bound to a slot in an API-specific way.");
struct BoundResource
{
  DOCUMENT("");
  BoundResource()
  {
    Id = ResourceId();
    HighestMip = -1;
    FirstSlice = -1;
    typeHint = CompType::Typeless;
  }
  BoundResource(ResourceId id)
  {
    Id = id;
    HighestMip = -1;
    FirstSlice = -1;
    typeHint = CompType::Typeless;
  }

  bool operator==(const BoundResource &o) const
  {
    return Id == o.Id && HighestMip == o.HighestMip && FirstSlice == o.FirstSlice &&
           typeHint == o.typeHint;
  }
  bool operator<(const BoundResource &o) const
  {
    if(Id != o.Id)
      return Id < o.Id;
    if(HighestMip != o.HighestMip)
      return HighestMip < o.HighestMip;
    if(FirstSlice != o.FirstSlice)
      return FirstSlice < o.FirstSlice;
    if(typeHint != o.typeHint)
      return typeHint < o.typeHint;
    return false;
  }
  DOCUMENT("A :class:`~renderdoc.ResourceId` identifying the bound resource.");
  ResourceId Id;
  DOCUMENT("For textures, the highest mip level available on this binding, or -1 for all mips");
  int HighestMip;
  DOCUMENT("For textures, the first array slice available on this binding. or -1 for all slices.");
  int FirstSlice;
  DOCUMENT(
      "For textures, a :class:`~renderdoc.CompType` hint for how to interpret typeless textures.");
  CompType typeHint;
};

DECLARE_REFLECTION_STRUCT(BoundResource);

// TODO this should be replaced with an rdcmap
DOCUMENT(R"(Contains all of the bound resources at a particular bindpoint. In APIs that don't
support resource arrays, there will only be one bound resource.
)");
struct BoundResourceArray
{
  DOCUMENT("");
  BoundResourceArray() = default;
  BoundResourceArray(BindpointMap b) : BindPoint(b) {}
  BoundResourceArray(BindpointMap b, const rdcarray<BoundResource> &r) : BindPoint(b), Resources(r)
  {
  }

  // for convenience for searching the array, we compare only using the BindPoint
  bool operator==(const BoundResourceArray &o) const { return BindPoint == o.BindPoint; }
  bool operator!=(const BoundResourceArray &o) const { return !(BindPoint == o.BindPoint); }
  bool operator<(const BoundResourceArray &o) const { return BindPoint < o.BindPoint; }
  DOCUMENT("The bind point for this array of bound resources.");
  BindpointMap BindPoint;

  DOCUMENT("The resources at this bind point");
  rdcarray<BoundResource> Resources;
};

DECLARE_REFLECTION_STRUCT(BoundResourceArray);

DOCUMENT("Information about a single vertex or index buffer binding.");
struct BoundBuffer
{
  DOCUMENT("");
  bool operator==(const BoundBuffer &o) const
  {
    return Buffer == o.Buffer && ByteOffset == o.ByteOffset && ByteStride == o.ByteStride;
  }
  bool operator<(const BoundBuffer &o) const
  {
    if(Buffer != o.Buffer)
      return Buffer < o.Buffer;
    if(ByteOffset != o.ByteOffset)
      return ByteOffset < o.ByteOffset;
    if(ByteStride != o.ByteStride)
      return ByteStride < o.ByteStride;
    return false;
  }
  DOCUMENT("A :class:`~renderdoc.ResourceId` identifying the buffer.");
  ResourceId Buffer;
  DOCUMENT("The offset in bytes from the start of the buffer to the data.");
  uint64_t ByteOffset = 0;
  DOCUMENT("The stride in bytes between the start of one element and the start of the next.");
  uint32_t ByteStride = 0;
};

DECLARE_REFLECTION_STRUCT(BoundBuffer);

DOCUMENT("Information about a single constant buffer binding.");
struct BoundCBuffer
{
  DOCUMENT("A :class:`~renderdoc.ResourceId` identifying the buffer.");
  ResourceId Buffer;
  DOCUMENT("The offset in bytes from the start of the buffer to the constant data.");
  uint64_t ByteOffset = 0;
  DOCUMENT("The size in bytes for the constant buffer. Access outside this size returns 0.");
  uint32_t ByteSize = 0;
};

DECLARE_REFLECTION_STRUCT(BoundCBuffer);

DOCUMENT("Information about a vertex input attribute feeding the vertex shader.");
struct VertexInputAttribute
{
  DOCUMENT("");
  bool operator==(const VertexInputAttribute &o) const
  {
    return Name == o.Name && VertexBuffer == o.VertexBuffer &&
           RelativeByteOffset == o.RelativeByteOffset && PerInstance == o.PerInstance &&
           InstanceRate == o.InstanceRate && Format == o.Format &&
           !memcmp(&GenericValue, &o.GenericValue, sizeof(GenericValue)) &&
           GenericEnabled == o.GenericEnabled && Used == o.Used;
  }
  bool operator<(const VertexInputAttribute &o) const
  {
    if(Name != o.Name)
      return Name < o.Name;
    if(VertexBuffer != o.VertexBuffer)
      return VertexBuffer < o.VertexBuffer;
    if(RelativeByteOffset != o.RelativeByteOffset)
      return RelativeByteOffset < o.RelativeByteOffset;
    if(PerInstance != o.PerInstance)
      return PerInstance < o.PerInstance;
    if(InstanceRate != o.InstanceRate)
      return InstanceRate < o.InstanceRate;
    if(Format != o.Format)
      return Format < o.Format;
    if(memcmp(&GenericValue, &o.GenericValue, sizeof(GenericValue)) < 0)
      return true;
    if(GenericEnabled != o.GenericEnabled)
      return GenericEnabled < o.GenericEnabled;
    if(Used != o.Used)
      return Used < o.Used;
    return false;
  }

  DOCUMENT("The name of this input. This may be a variable name or a semantic name.");
  rdcstr Name;
  DOCUMENT("The index of the vertex buffer used to provide this attribute.");
  int VertexBuffer;
  DOCUMENT("The byte offset from the start of the vertex data for this VB to this attribute.");
  uint32_t RelativeByteOffset;
  DOCUMENT("``True`` if this attribute runs at instance rate.");
  bool PerInstance;
  DOCUMENT(R"(If :data:`PerInstance` is ``True``, the number of instances that source the same value
from the vertex buffer before advancing to the next value.
)");
  int InstanceRate;
  DOCUMENT("A :class:`~renderdoc.ResourceFormat` with the interpreted format of this attribute.");
  ResourceFormat Format;
  DOCUMENT(R"(A :class:`~renderdoc.PixelValue` with the generic value for this attribute if it has
no VB bound.
)");
  PixelValue GenericValue;
  DOCUMENT("``True`` if this attribute is using :data:`GenericValue` for its data.");
  bool GenericEnabled;
  DOCUMENT("``True`` if this attribute is enabled and used by the vertex shader.");
  bool Used;
};

DECLARE_REFLECTION_STRUCT(VertexInputAttribute);

DOCUMENT("Information about a viewport.");
struct Viewport
{
  DOCUMENT("The X co-ordinate of the viewport.");
  float x;
  DOCUMENT("The Y co-ordinate of the viewport.");
  float y;
  DOCUMENT("The width of the viewport.");
  float width;
  DOCUMENT("The height of the viewport.");
  float height;
};

DECLARE_REFLECTION_STRUCT(Viewport);

DOCUMENT(R"(An API-agnostic view of the common aspects of the pipeline state. This allows simple
access to e.g. find out the bound resources or vertex buffers, or certain pipeline state which is
available on all APIs.

For more detailed or precise information without abstraction, access the specific pipeline state
for the capture that's open.
)");
class CommonPipelineState
{
public:
  CommonPipelineState(ICaptureContext &ctx) : m_Ctx(ctx) {}
  DOCUMENT(R"(Set the source API-specific states to read data from.

:param ~renderdoc.APIProperties props: The properties of the current capture.
:param ~renderdoc.D3D11_State d3d11: The D3D11 state.
:param ~renderdoc.D3D12_State d3d12: The D3D11 state.
:param ~renderdoc.GL_State gl: The OpenGL state.
:param ~renderdoc.VK_State vk: The Vulkan state.
)");
  void SetStates(APIProperties props, const D3D11Pipe::State *d3d11, const D3D12Pipe::State *d3d12,
                 const GLPipe::State *gl, const VKPipe::State *vk)
  {
    m_APIProps = props;
    m_D3D11 = d3d11;
    m_D3D12 = d3d12;
    m_GL = gl;
    m_Vulkan = vk;
  }

  DOCUMENT(
      "The default :class:`~renderdoc.GraphicsAPI` to pretend to contain, if no capture is "
      "loaded.");
  GraphicsAPI DefaultType = GraphicsAPI::D3D11;

  DOCUMENT(R"(Determines whether or not a capture is currently loaded.

:return: A boolean indicating if a capture is currently loaded.
:rtype: ``bool``
)");
  bool IsCaptureLoaded()
  {
    return m_D3D11 != NULL || m_D3D12 != NULL || m_GL != NULL || m_Vulkan != NULL;
  }

  DOCUMENT(R"(Determines whether or not a D3D11 capture is currently loaded.

:return: A boolean indicating if a D3D11 capture is currently loaded.
:rtype: ``bool``
)");
  bool IsCaptureD3D11()
  {
    return IsCaptureLoaded() && m_APIProps.pipelineType == GraphicsAPI::D3D11 && m_D3D11 != NULL;
  }

  DOCUMENT(R"(Determines whether or not a D3D12 capture is currently loaded.

:return: A boolean indicating if a D3D12 capture is currently loaded.
:rtype: ``bool``
)");
  bool IsCaptureD3D12()
  {
    return IsCaptureLoaded() && m_APIProps.pipelineType == GraphicsAPI::D3D12 && m_D3D12 != NULL;
  }

  DOCUMENT(R"(Determines whether or not an OpenGL capture is currently loaded.

:return: A boolean indicating if an OpenGL capture is currently loaded.
:rtype: ``bool``
)");
  bool IsCaptureGL()
  {
    return IsCaptureLoaded() && m_APIProps.pipelineType == GraphicsAPI::OpenGL && m_GL != NULL;
  }

  DOCUMENT(R"(Determines whether or not a Vulkan capture is currently loaded.

:return: A boolean indicating if a Vulkan capture is currently loaded.
:rtype: ``bool``
)");
  bool IsCaptureVK()
  {
    return IsCaptureLoaded() && m_APIProps.pipelineType == GraphicsAPI::Vulkan && m_Vulkan != NULL;
  }

  // add a bunch of generic properties that people can check to save having to see which pipeline
  // state is valid and look at the appropriate part of it
  DOCUMENT(R"(Determines whether or not tessellation is currently enabled.

:return: A boolean indicating if tessellation is currently enabled.
:rtype: ``bool``
)");
  bool IsTessellationEnabled()
  {
    if(IsCaptureLoaded())
    {
      if(IsCaptureD3D11())
        return m_D3D11 != NULL && m_D3D11->m_HS.Object != ResourceId();

      if(IsCaptureD3D12())
        return m_D3D12 != NULL && m_D3D12->m_HS.Object != ResourceId();

      if(IsCaptureGL())
        return m_GL != NULL && m_GL->m_TES.Object != ResourceId();

      if(IsCaptureVK())
        return m_Vulkan != NULL && m_Vulkan->m_TES.Object != ResourceId();
    }

    return false;
  }

  DOCUMENT(R"(Determines whether or not the current capture supports binding arrays of resources.

:return: A boolean indicating if binding arrays of resources is supported.
:rtype: ``bool``
)");
  bool SupportsResourceArrays() { return IsCaptureLoaded() && IsCaptureVK(); }
  DOCUMENT(R"(Determines whether or not the current capture uses explicit barriers.

:return: A boolean indicating if explicit barriers are used.
:rtype: ``bool``
)");
  bool SupportsBarriers() { return IsCaptureLoaded() && (IsCaptureVK() || IsCaptureD3D12()); }
  DOCUMENT(R"(Determines whether or not the PostVS data is aligned in the typical fashion (ie.
vectors not crossing ``float4`` boundaries). APIs that use stream-out or transform feedback have
tightly packed data, but APIs that rewrite shaders to dump data might have these alignment
requirements.

:return: A boolean indicating if post-VS data is aligned.
:rtype: ``bool``
)");
  bool HasAlignedPostVSData() { return IsCaptureLoaded() && IsCaptureVK(); }
  DOCUMENT(R"(For APIs that have explicit barriers, retrieves the current layout of a resource.

:return: The name of the current resource layout.
:rtype: ``str``
)");
  rdcstr GetResourceLayout(ResourceId id);

  DOCUMENT(R"(Retrieves a suitable two or three letter abbreviation of the given shader stage.

:param ~renderdoc.ShaderStage stage: The shader stage to abbreviate.
:return: The abbreviation of the stage.
:rtype: ``str``
)");
  rdcstr Abbrev(ShaderStage stage);
  DOCUMENT(R"(Retrieves a suitable two or three letter abbreviation of the output stage. Typically
'OM' or 'FBO'.

:return: The abbreviation of the output stage.
:rtype: ``str``
)");
  rdcstr OutputAbbrev();

  DOCUMENT(R"(Retrieves the viewport for a given index.

:param int index: The viewport index to retrieve.
:return: The viewport for the given index.
:rtype: Viewport
)");
  Viewport GetViewport(int index);

  DOCUMENT(R"(Retrieves the current bindpoint mapping for a shader stage.

This returns an empty bindpoint mapping if no shader is bound.

:param ~renderdoc.ShaderStage stage: The shader stage to fetch.
:return: The bindpoint mapping for the given shader.
:rtype: ~renderdoc.ShaderBindpointMapping
)");
  const ShaderBindpointMapping &GetBindpointMapping(ShaderStage stage);

  DOCUMENT(R"(Retrieves the shader reflection information for a shader stage.

This returns ``None`` if no shader is bound.

:param ~renderdoc.ShaderStage stage: The shader stage to fetch.
:return: The reflection data for the given shader.
:rtype: :class:`~renderdoc.ShaderBindpointMapping` or ``None``
)");
  const ShaderReflection *GetShaderReflection(ShaderStage stage);

  DOCUMENT(R"(Retrieves the the compute pipeline state object, if applicable.

:return: The object ID for the given pipeline object.
:rtype: ~renderdoc.ResourceId
)");
  ResourceId GetComputePipelineObject();

  DOCUMENT(R"(Retrieves the the graphics pipeline state object, if applicable.

:return: The object ID for the given pipeline object.
:rtype: ~renderdoc.ResourceId
)");
  ResourceId GetGraphicsPipelineObject();

  DOCUMENT(R"(Retrieves the name of the entry point function for a shader stage.

For some APIs that don't distinguish by entry point, this may be empty.

:param ~renderdoc.ShaderStage stage: The shader stage to fetch.
:return: The entry point name for the given shader.
:rtype: ``str``
)");
  rdcstr GetShaderEntryPoint(ShaderStage stage);

  DOCUMENT(R"(Retrieves the object ID of the shader bound at a shader stage.

:param ~renderdoc.ShaderStage stage: The shader stage to fetch.
:return: The object ID for the given shader.
:rtype: ~renderdoc.ResourceId
)");
  ResourceId GetShader(ShaderStage stage);

  DOCUMENT(R"(Retrieves the name of the shader object at a shader stage.

:param ~renderdoc.ShaderStage stage: The shader stage to fetch.
:return: The object name for the given shader.
:rtype: ``str``
)");
  rdcstr GetShaderName(ShaderStage stage);

  DOCUMENT(R"(Retrieves the common file extension for high level shaders in the current API.

Typically this is ``glsl`` or ``hlsl``.

:return: The file extension with no ``.``.
:rtype: ``str``
)");
  rdcstr GetShaderExtension();

  DOCUMENT(R"(Retrieves the current index buffer binding.

:return: A :class:`BoundBuffer` with the index buffer details. The stride is always 0.
:rtype: ``BoundBuffer``
)");
  BoundBuffer GetIBuffer();

  DOCUMENT(R"(Determines whether or not primitive restart is enabled.

:return: A boolean indicating if primitive restart is enabled.
:rtype: ``bool``
)");
  bool IsStripRestartEnabled();

  DOCUMENT(R"(Retrieves the primitive restart index.

:param int indexByteWidth: The width in bytes of the indices.
:return: The index value that represents a strip restart not a real index.
:rtype: ``int``
)");
  uint32_t GetStripRestartIndex();

  DOCUMENT(R"(Retrieves the currently bound vertex buffers.

:return: The list of bound vertex buffers.
:rtype: ``list`` of :class:`BoundBuffer`.
)");
  rdcarray<BoundBuffer> GetVBuffers();

  DOCUMENT(R"(Retrieves the currently specified vertex attributes.

:return: The list of current vertex attributes.
:rtype: ``list`` of :class:`VertexInputAttribute`.
)");
  rdcarray<VertexInputAttribute> GetVertexInputs();

  DOCUMENT(R"(Retrieves the constant buffer at a given binding.

:param ~renderdoc.ShaderStage stage: The shader stage to fetch from.
:param int BufIdx: The index in the shader's ConstantBlocks array to look up.
:param int ArrayIdx: For APIs that support arrays of constant buffers in a single binding, the index
  in that array to look up.
:return: The constant buffer at the specified binding.
:rtype: BoundCBuffer
)");
  BoundCBuffer GetConstantBuffer(ShaderStage stage, uint32_t BufIdx, uint32_t ArrayIdx);

  DOCUMENT(R"(Retrieves the read-only resources bound to a particular shader stage.

:param ~renderdoc.ShaderStage stage: The shader stage to fetch from.
:return: The currently bound read-only resoruces.
:rtype: ``list`` of :class:`BoundResourceArray` entries
)");
  rdcarray<BoundResourceArray> GetReadOnlyResources(ShaderStage stage);

  DOCUMENT(R"(Retrieves the read/write resources bound to a particular shader stage.

:param ~renderdoc.ShaderStage stage: The shader stage to fetch from.
:return: The currently bound read/write resoruces.
:rtype: ``list`` of :class:`BoundResourceArray` entries
)");
  rdcarray<BoundResourceArray> GetReadWriteResources(ShaderStage stage);

  DOCUMENT(R"(Retrieves the read/write resources bound to the depth-stencil output.

:return: The currently bound depth-stencil resource.
:rtype: BoundResource
)");
  BoundResource GetDepthTarget();

  DOCUMENT(R"(Retrieves the resources bound to the colour outputs.

:return: The currently bound output targets.
:rtype: ``list`` of :class:`BoundResource`.
)");
  rdcarray<BoundResource> GetOutputTargets();

private:
  const D3D11Pipe::State *m_D3D11 = NULL;
  const D3D12Pipe::State *m_D3D12 = NULL;
  const GLPipe::State *m_GL = NULL;
  const VKPipe::State *m_Vulkan = NULL;
  APIProperties m_APIProps;

  ICaptureContext &m_Ctx;

  const D3D11Pipe::Shader &GetD3D11Stage(ShaderStage stage);
  const D3D12Pipe::Shader &GetD3D12Stage(ShaderStage stage);
  const GLPipe::Shader &GetGLStage(ShaderStage stage);
  const VKPipe::Shader &GetVulkanStage(ShaderStage stage);
};