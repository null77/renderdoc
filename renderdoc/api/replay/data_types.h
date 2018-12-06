/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2018 Baldur Karlsson
 * Copyright (c) 2014 Crytek
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

#include "replay_enums.h"

DOCUMENT("A floating point four-component vector");
struct FloatVector
{
  FloatVector() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
  FloatVector(const FloatVector &) = default;
  FloatVector(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
#if defined(RENDERDOC_QT_COMPAT)
  FloatVector(const QColor &col) : x(col.redF()), y(col.greenF()), z(col.blueF()), w(col.alphaF())
  {
  }
#endif
  DOCUMENT("The x component.");
  float x;
  DOCUMENT("The y component.");
  float y;
  DOCUMENT("The z component.");
  float z;
  DOCUMENT("The w component.");
  float w;
};

DECLARE_REFLECTION_STRUCT(FloatVector);

DOCUMENT("Properties of a path on a remote filesystem.");
struct PathEntry
{
  DOCUMENT("");
  PathEntry() : flags(PathProperty::NoFlags), lastmod(0), size(0) {}
  PathEntry(const PathEntry &) = default;
  PathEntry(const char *fn, PathProperty f) : filename(fn), flags(f), lastmod(0), size(0) {}
  bool operator==(const PathEntry &o) const
  {
    return filename == o.filename && flags == o.flags && lastmod == o.lastmod && size == o.size;
  }
  bool operator<(const PathEntry &o) const
  {
    if(!(filename == o.filename))
      return filename < o.filename;
    if(!(flags == o.flags))
      return flags < o.flags;
    if(!(lastmod == o.lastmod))
      return lastmod < o.lastmod;
    if(!(size == o.size))
      return size < o.size;
    return false;
  }
  DOCUMENT("The filename of this path. This contains only the filename, not the full path.");
  rdcstr filename;

  DOCUMENT("The :class:`PathProperty` flags for this path.");
  PathProperty flags;

  DOCUMENT("The last modified date of this path, as a unix timestamp in UTC.");
  uint32_t lastmod;

  DOCUMENT("The size of the path in bytes.");
  uint64_t size;
};

DECLARE_REFLECTION_STRUCT(PathEntry);

DOCUMENT("Properties of a section in a renderdoc capture file.");
struct SectionProperties
{
  DOCUMENT("");
  SectionProperties() = default;
  SectionProperties(const SectionProperties &) = default;

  DOCUMENT("The name of this section.");
  rdcstr name;

  DOCUMENT("The type of this section, if it is a known pre-defined section.");
  SectionType type = SectionType::Unknown;

  DOCUMENT("The flags describing how this section is stored.");
  SectionFlags flags = SectionFlags::NoFlags;

  DOCUMENT("The version of this section - the meaning of which is up to the type.");
  uint64_t version = 0;

  DOCUMENT("The number of bytes of data contained in this section, once uncompressed.");
  uint64_t uncompressedSize = 0;

  DOCUMENT("The number of bytes of data in this section when compressed on disk.");
  uint64_t compressedSize = 0;
};

DECLARE_REFLECTION_STRUCT(SectionProperties);

struct ResourceFormat;

DOCUMENT("Internal function for getting the name for a resource format.");
extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_ResourceFormatName(const ResourceFormat &fmt,
                                                                        rdcstr &name);

DOCUMENT("Description of the format of a resource or element.");
struct ResourceFormat
{
  DOCUMENT("");
  ResourceFormat()
  {
    type = ResourceFormatType::Undefined;

    compCount = compByteWidth = 0;
    compType = CompType::Typeless;

    flags = 0;
  }
  ResourceFormat(const ResourceFormat &) = default;

  bool operator==(const ResourceFormat &r) const
  {
    return type == r.type && compCount == r.compCount && compByteWidth == r.compByteWidth &&
           compType == r.compType && flags == r.flags;
  }
  bool operator<(const ResourceFormat &r) const
  {
    if(type != r.type)
      return type < r.type;
    if(compCount != r.compCount)
      return compCount < r.compCount;
    if(compByteWidth != r.compByteWidth)
      return compByteWidth < r.compByteWidth;
    if(compType != r.compType)
      return compType < r.compType;
    if(flags != r.flags)
      return flags < r.flags;
    return false;
  }

  bool operator!=(const ResourceFormat &r) const { return !(*this == r); }
  DOCUMENT(R"(:return: The name of the format.
:rtype: ``str``
)");
  rdcstr Name() const
  {
    rdcstr ret;
    RENDERDOC_ResourceFormatName(*this, ret);
    return ret;
  }

  DOCUMENT(R"(:return: ``True`` if the ``ResourceFormat`` is a 'special' non-regular type.
:rtype: ``bool``
)");
  bool Special() const { return type != ResourceFormatType::Regular; }
  DOCUMENT(R"(The :class:`ResourceFormatType` of this format. If the value is not
:attr:`ResourceFormatType.Regular` then it's a non-uniform layout like block-compressed.
)");

  DOCUMENT(R"(:return: ``True`` if the components are to be read in ``BGRA`` order.
:rtype: ``bool``
)");
  bool bgraOrder() const { return (flags & ResourceFormat_BGRA) != 0; }
  DOCUMENT(R"(:return: ``True`` if the components are SRGB corrected on read and write.
:rtype: ``bool``
)");
  bool srgbCorrected() const { return (flags & ResourceFormat_SRGB) != 0; }
  DOCUMENT(R"(Set BGRA order flag. See :meth:`bgraOrder`.

:param bool flag: The new flag value.
)");
  void setBgraOrder(bool flag)
  {
    if(flag)
      flags |= ResourceFormat_BGRA;
    else
      flags &= ~ResourceFormat_BGRA;
  }

  DOCUMENT(R"(Set SRGB correction flag. See :meth:`srgbCorrected`.

:param bool flag: The new flag value.
)");
  void setSrgbCorrected(bool flag)
  {
    if(flag)
      flags |= ResourceFormat_SRGB;
    else
      flags &= ~ResourceFormat_SRGB;
  }

  ResourceFormatType type;

  DOCUMENT("The :class:`type <CompType>` of each component.");
  CompType compType;
  DOCUMENT("The number of components in each element.");
  uint8_t compCount;
  DOCUMENT("The width in bytes of each component.");
  uint8_t compByteWidth;

private:
  enum
  {
    ResourceFormat_BGRA = 0x001,
    ResourceFormat_SRGB = 0x002,
  };
  uint16_t flags;

  // make DoSerialise a friend so it can serialise flags
  template <typename SerialiserType>
  friend void DoSerialise(SerialiserType &ser, ResourceFormat &el);
};

DECLARE_REFLECTION_STRUCT(ResourceFormat);

DOCUMENT("The details of a texture filter in a sampler.");
struct TextureFilter
{
  DOCUMENT("");
  TextureFilter() = default;
  TextureFilter(const TextureFilter &) = default;

  bool operator==(const TextureFilter &o) const
  {
    return minify == o.minify && magnify == o.magnify && mip == o.mip && filter == o.filter;
  }
  bool operator<(const TextureFilter &o) const
  {
    if(!(minify == o.minify))
      return minify < o.minify;
    if(!(magnify == o.magnify))
      return magnify < o.magnify;
    if(!(mip == o.mip))
      return mip < o.mip;
    if(!(filter == o.filter))
      return filter < o.filter;
    return false;
  }
  DOCUMENT("The :class:`FilterMode` to use when minifying the texture.");
  FilterMode minify = FilterMode::NoFilter;
  DOCUMENT("The :class:`FilterMode` to use when magnifying the texture.");
  FilterMode magnify = FilterMode::NoFilter;
  DOCUMENT("The :class:`FilterMode` to use when interpolating between mips.");
  FilterMode mip = FilterMode::NoFilter;
  DOCUMENT("The :class:`FilterFunction` to apply after interpolating values.");
  FilterFunction filter = FilterFunction::Normal;
};

DECLARE_REFLECTION_STRUCT(TextureFilter);

DOCUMENT("A description of any type of resource.");
struct ResourceDescription
{
  DOCUMENT("");
  ResourceDescription() = default;
  ResourceDescription(const ResourceDescription &) = default;

  bool operator==(const ResourceDescription &o) const { return resourceId == o.resourceId; }
  bool operator<(const ResourceDescription &o) const { return resourceId < o.resourceId; }
  DOCUMENT("The unique :class:`ResourceId` that identifies this resource.");
  ResourceId resourceId;

  DOCUMENT("The :class:`ResourceType` of the resource.");
  ResourceType type = ResourceType::Unknown;

  DOCUMENT(R"(``True`` if :data:`name` was just autogenerated based on the ID, not assigned a
human-readable name by the application.
)");
  bool autogeneratedName = true;

  DOCUMENT("The name given to this resource.");
  rdcstr name;

  DOCUMENT(R"(The chunk indices in the structured file that initialised this resource.

This will at least contain the first call that created it, but may contain other auxilliary calls.
)");
  rdcarray<uint32_t> initialisationChunks;

  DOCUMENT(R"(The :class:`ResourceId` of any derived resources, such as resource views or aliases.

Can be empty if there are no derived resources.

This is the inverse of :data:`parentResources` in a potentially many:many relationship, but
typically it is one parent to many derived.
)");
  rdcarray<ResourceId> derivedResources;

  DOCUMENT(R"(The :class:`ResourceId` of parent resources, of which this is derived.

Can be empty if there are no parent resources.

This is the inverse of :data:`derivedResources` in a potentially many:many relationship, but
typically it is one parent to many derived.
)");
  rdcarray<ResourceId> parentResources;

  DOCUMENT(R"(Utility function for setting up a custom name to overwrite the auto-generated one.

:param str givenName: The custom name to use.
)");
  inline void SetCustomName(const rdcstr &givenName)
  {
    autogeneratedName = false;
    name = givenName.isEmpty() ? "<empty>" : givenName;
  }
};

DECLARE_REFLECTION_STRUCT(ResourceDescription);

DOCUMENT("A description of a buffer resource.");
struct BufferDescription
{
  DOCUMENT("");
  BufferDescription() = default;
  BufferDescription(const BufferDescription &) = default;

  bool operator==(const BufferDescription &o) const
  {
    return resourceId == o.resourceId && creationFlags == o.creationFlags && length == o.length;
  }
  bool operator<(const BufferDescription &o) const
  {
    if(!(resourceId == o.resourceId))
      return resourceId < o.resourceId;
    if(!(creationFlags == o.creationFlags))
      return creationFlags < o.creationFlags;
    if(!(length == o.length))
      return length < o.length;
    return false;
  }
  DOCUMENT("The unique :class:`ResourceId` that identifies this buffer.");
  ResourceId resourceId;

  DOCUMENT("The way this buffer will be used in the pipeline.");
  BufferCategory creationFlags;

  DOCUMENT("The byte length of the buffer.");
  uint64_t length;
};

DECLARE_REFLECTION_STRUCT(BufferDescription);

DOCUMENT("A description of a texture resource.");
struct TextureDescription
{
  DOCUMENT("");
  TextureDescription() = default;
  TextureDescription(const TextureDescription &) = default;

  bool operator==(const TextureDescription &o) const
  {
    return format == o.format && dimension == o.dimension && type == o.type && width == o.width &&
           height == o.height && depth == o.depth && resourceId == o.resourceId &&
           cubemap == o.cubemap && mips == o.mips && arraysize == o.arraysize &&
           creationFlags == o.creationFlags && msQual == o.msQual && msSamp == o.msSamp &&
           byteSize == o.byteSize;
  }
  bool operator<(const TextureDescription &o) const
  {
    if(!(format == o.format))
      return format < o.format;
    if(!(dimension == o.dimension))
      return dimension < o.dimension;
    if(!(type == o.type))
      return type < o.type;
    if(!(width == o.width))
      return width < o.width;
    if(!(height == o.height))
      return height < o.height;
    if(!(depth == o.depth))
      return depth < o.depth;
    if(!(resourceId == o.resourceId))
      return resourceId < o.resourceId;
    if(!(cubemap == o.cubemap))
      return cubemap < o.cubemap;
    if(!(mips == o.mips))
      return mips < o.mips;
    if(!(arraysize == o.arraysize))
      return arraysize < o.arraysize;
    if(!(creationFlags == o.creationFlags))
      return creationFlags < o.creationFlags;
    if(!(msQual == o.msQual))
      return msQual < o.msQual;
    if(!(msSamp == o.msSamp))
      return msSamp < o.msSamp;
    if(!(byteSize == o.byteSize))
      return byteSize < o.byteSize;
    return false;
  }
  DOCUMENT("The :class:`ResourceFormat` that describes the format of each pixel in the texture.");
  ResourceFormat format;

  DOCUMENT("The base dimension of the texture - either 1, 2, or 3.");
  uint32_t dimension;

  DOCUMENT("The :class:`TextureType` of the texture.");
  TextureType type;

  DOCUMENT("The width of the texture, or length for buffer textures.");
  uint32_t width;

  DOCUMENT("The height of the texture, or 1 if not applicable.");
  uint32_t height;

  DOCUMENT("The depth of the texture, or 1 if not applicable.");
  uint32_t depth;

  DOCUMENT("The unique :class:`ResourceId` that identifies this texture.");
  ResourceId resourceId;

  DOCUMENT("``True`` if this texture is used as a cubemap or cubemap array.");
  bool cubemap;

  DOCUMENT("How many mips this texture has, will be at least 1.");
  uint32_t mips;

  DOCUMENT("How many array elements this texture has, will be at least 1.");
  uint32_t arraysize;

  DOCUMENT("The way this texture will be used in the pipeline.");
  TextureCategory creationFlags;

  DOCUMENT("The quality setting of this texture, or 0 if not applicable.");
  uint32_t msQual;

  DOCUMENT("How many multisampled samples this texture has, will be at least 1.");
  uint32_t msSamp;

  DOCUMENT("How many bytes would be used to store this texture and all its mips/slices.");
  uint64_t byteSize;
};

DECLARE_REFLECTION_STRUCT(TextureDescription);

DOCUMENT("An individual API-level event, generally corresponds one-to-one with an API call.");
struct APIEvent
{
  DOCUMENT("");
  APIEvent() = default;
  APIEvent(const APIEvent &) = default;

  bool operator==(const APIEvent &o) const { return eventId == o.eventId; }
  bool operator<(const APIEvent &o) const { return eventId < o.eventId; }
  DOCUMENT(R"(The API event's Event ID.

This is a 1-based count of API events in the capture. The eventId is used as a reference point in
many places in the API to represent where in the capture the 'current state' is, and to perform
analysis in reference to the state at a particular point in the frame.

eventIds are always increasing and positive, but they may not be contiguous - in some circumstances
there may be gaps if some events are consumed entirely internally, such as debug marker pops which
only modify the internal drawcall tree structures.

Also eventIds may not correspond directly to an actual function call - sometimes a function such as
a multi draw indirect will be one function call that expands to multiple events to allow inspection
of results part way through the multi draw.
)");
  uint32_t eventId;

  DOCUMENT("A list of addresses in the CPU callstack where this function was called.");
  rdcarray<uint64_t> callstack;

  DOCUMENT("The chunk index for this function call in the structured file.");
  uint32_t chunkIndex;

  DOCUMENT(R"(A byte offset in the data stream where this event happens.

.. note:: This should only be used as a relative measure, it is not a literal number of bytes from
  the start of the file on disk.
)");
  uint64_t fileOffset;
};

DECLARE_REFLECTION_STRUCT(APIEvent);

DOCUMENT("A debugging message from the API validation or internal analysis and error detection.");
struct DebugMessage
{
  DOCUMENT("");
  DebugMessage() = default;
  DebugMessage(const DebugMessage &) = default;

  bool operator==(const DebugMessage &o) const
  {
    return eventId == o.eventId && category == o.category && severity == o.severity &&
           source == o.source && messageID == o.messageID && description == o.description;
  }
  bool operator<(const DebugMessage &o) const
  {
    if(!(eventId == o.eventId))
      return eventId < o.eventId;
    if(!(category == o.category))
      return category < o.category;
    if(!(severity == o.severity))
      return severity < o.severity;
    if(!(source == o.source))
      return source < o.source;
    if(!(messageID == o.messageID))
      return messageID < o.messageID;
    if(!(description == o.description))
      return description < o.description;
    return false;
  }
  DOCUMENT("The :data:`eventId <APIEvent.eventId>` where this debug message was found.");
  uint32_t eventId;

  DOCUMENT("The :class:`category <MessageCategory>` of this debug message.");
  MessageCategory category;

  DOCUMENT("The :class:`severity <MessageSeverity>` of this debug message.");
  MessageSeverity severity;

  DOCUMENT("The :class:`source <MessageSource>` of this debug message.");
  MessageSource source;

  DOCUMENT("An ID that identifies this particular debug message uniquely.");
  uint32_t messageID;

  DOCUMENT("The string contents of the message.");
  rdcstr description;
};

DECLARE_REFLECTION_STRUCT(DebugMessage);

DOCUMENT(R"(The type of bucketing method for recording statistics.

.. data:: Linear

  Each bucket contains a fixed number of elements. The highest bucket also accumulates any values
  too high for any of the buckets.

.. data:: Pow2

  Each bucket holds twice as many elements as the previous one, with the first bucket containing
  just 1 (bucket index is ``log2(value)``).
)");
enum class BucketRecordType : int
{
  Linear,
  Pow2,
};
DECLARE_REFLECTION_ENUM(BucketRecordType);

DOCUMENT(R"(Contains the statistics for constant binds in a frame.

.. data:: BucketType

  The type of buckets being used. See :class:`BucketRecordType`.

.. data:: BucketCount

  How many buckets there are in the arrays.
)");
struct ConstantBindStats
{
  DOCUMENT("");
  ConstantBindStats() = default;
  ConstantBindStats(const ConstantBindStats &) = default;

  static const BucketRecordType BucketType = BucketRecordType::Pow2;
  static const size_t BucketCount = 31;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT("A list where the Nth element contains the number of calls that bound N buffers.");
  rdcarray<uint32_t> bindslots;

  DOCUMENT("A :class:`bucketed <BucketType>` list over the sizes of buffers bound.");
  rdcarray<uint32_t> sizes;
};

DECLARE_REFLECTION_STRUCT(ConstantBindStats);

DOCUMENT("Contains the statistics for sampler binds in a frame.");
struct SamplerBindStats
{
  DOCUMENT("");
  SamplerBindStats() = default;
  SamplerBindStats(const SamplerBindStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT("A list where the Nth element contains the number of calls that bound N samplers.");
  rdcarray<uint32_t> bindslots;
};

DECLARE_REFLECTION_STRUCT(SamplerBindStats);

DOCUMENT("Contains the statistics for resource binds in a frame.");
struct ResourceBindStats
{
  DOCUMENT("");
  ResourceBindStats() = default;
  ResourceBindStats(const ResourceBindStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT(R"(A list with one element for each type in :class:`TextureType`.

The Nth element contains the number of times a resource of that type was bound.
)");
  rdcarray<uint32_t> types;

  DOCUMENT("A list where the Nth element contains the number of calls that bound N resources.");
  rdcarray<uint32_t> bindslots;
};

DECLARE_REFLECTION_STRUCT(ResourceBindStats);

DOCUMENT(R"(Contains the statistics for resource updates in a frame.

.. data:: BucketType

  The type of buckets being used. See :class:`BucketRecordType`.

.. data:: BucketCount

  How many buckets there are in the arrays.
)");
struct ResourceUpdateStats
{
  DOCUMENT("");
  ResourceUpdateStats() = default;
  ResourceUpdateStats(const ResourceUpdateStats &) = default;

  static const BucketRecordType BucketType = BucketRecordType::Pow2;
  static const size_t BucketCount = 31;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many of :data:`calls` were mapped pointers written by the CPU.");
  uint32_t clients;

  DOCUMENT("How many of :data:`calls` were batched updates written in the command queue.");
  uint32_t servers;

  DOCUMENT(R"(A list with one element for each type in :class:`TextureType`.

The Nth element contains the number of times a resource of that type was updated.
)");
  rdcarray<uint32_t> types;

  DOCUMENT("A :class:`bucketed <BucketType>` list over the number of bytes in the update.");
  rdcarray<uint32_t> sizes;
};

DECLARE_REFLECTION_STRUCT(ResourceUpdateStats);

DOCUMENT(R"(Contains the statistics for draws in a frame.

.. data:: BucketType

  The type of buckets being used. See :class:`BucketRecordType`.

.. data:: BucketSize

  How many elements each bucket contains.

.. data:: BucketCount

  How many buckets there are in the arrays.
)");
struct DrawcallStats
{
  DOCUMENT("");
  DrawcallStats() = default;
  DrawcallStats(const DrawcallStats &) = default;

  static const BucketRecordType BucketType = BucketRecordType::Linear;
  static const size_t BucketSize = 1;
  static const size_t BucketCount = 16;

  DOCUMENT("How many draw calls were made.");
  uint32_t calls;
  DOCUMENT("How many of :data:`calls` were instanced.");
  uint32_t instanced;
  DOCUMENT("How many of :data:`calls` were indirect.");
  uint32_t indirect;

  DOCUMENT("A :class:`bucketed <BucketType>` list over the number of instances in the draw.");
  rdcarray<uint32_t> counts;
};

DECLARE_REFLECTION_STRUCT(DrawcallStats);

DOCUMENT("Contains the statistics for compute dispatches in a frame.");
struct DispatchStats
{
  DOCUMENT("");
  DispatchStats() = default;
  DispatchStats(const DispatchStats &) = default;

  DOCUMENT("How many dispatch calls were made.");
  uint32_t calls;

  DOCUMENT("How many of :data:`calls` were indirect.");
  uint32_t indirect;
};

DECLARE_REFLECTION_STRUCT(DispatchStats);

DOCUMENT("Contains the statistics for index buffer binds in a frame.");
struct IndexBindStats
{
  DOCUMENT("");
  IndexBindStats() = default;
  IndexBindStats(const IndexBindStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;
};

DECLARE_REFLECTION_STRUCT(IndexBindStats);

DOCUMENT("Contains the statistics for vertex buffer binds in a frame.");
struct VertexBindStats
{
  DOCUMENT("");
  VertexBindStats() = default;
  VertexBindStats(const VertexBindStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT(
      "A list where the Nth element contains the number of calls that bound N vertex buffers.");
  rdcarray<uint32_t> bindslots;
};

DECLARE_REFLECTION_STRUCT(VertexBindStats);

DOCUMENT("Contains the statistics for vertex layout binds in a frame.");
struct LayoutBindStats
{
  DOCUMENT("");
  LayoutBindStats() = default;
  LayoutBindStats(const LayoutBindStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;
};

DECLARE_REFLECTION_STRUCT(LayoutBindStats);

DOCUMENT("Contains the statistics for shader binds in a frame.");
struct ShaderChangeStats
{
  DOCUMENT("");
  ShaderChangeStats() = default;
  ShaderChangeStats(const ShaderChangeStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT("How many calls made no change due to the existing bind being identical.");
  uint32_t redundants;
};

DECLARE_REFLECTION_STRUCT(ShaderChangeStats);

DOCUMENT("Contains the statistics for blend state binds in a frame.");
struct BlendStats
{
  DOCUMENT("");
  BlendStats() = default;
  BlendStats(const BlendStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT("How many calls made no change due to the existing bind being identical.");
  uint32_t redundants;
};

DECLARE_REFLECTION_STRUCT(BlendStats);

DOCUMENT("Contains the statistics for depth stencil state binds in a frame.");
struct DepthStencilStats
{
  DOCUMENT("");
  DepthStencilStats() = default;
  DepthStencilStats(const DepthStencilStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT("How many calls made no change due to the existing bind being identical.");
  uint32_t redundants;
};

DECLARE_REFLECTION_STRUCT(DepthStencilStats);

DOCUMENT("Contains the statistics for rasterizer state binds in a frame.");
struct RasterizationStats
{
  DOCUMENT("");
  RasterizationStats() = default;
  RasterizationStats(const RasterizationStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT("How many calls made no change due to the existing bind being identical.");
  uint32_t redundants;

  DOCUMENT("A list where the Nth element contains the number of calls that bound N viewports.");
  rdcarray<uint32_t> viewports;

  DOCUMENT("A list where the Nth element contains the number of calls that bound N scissor rects.");
  rdcarray<uint32_t> rects;
};

DECLARE_REFLECTION_STRUCT(RasterizationStats);

DOCUMENT("Contains the statistics for output merger or UAV binds in a frame.");
struct OutputTargetStats
{
  DOCUMENT("");
  OutputTargetStats() = default;
  OutputTargetStats(const OutputTargetStats &) = default;

  DOCUMENT("How many function calls were made.");
  uint32_t calls;

  DOCUMENT("How many objects were bound.");
  uint32_t sets;

  DOCUMENT("How many objects were unbound.");
  uint32_t nulls;

  DOCUMENT("A list where the Nth element contains the number of calls that bound N targets.");
  rdcarray<uint32_t> bindslots;
};

DECLARE_REFLECTION_STRUCT(OutputTargetStats);

DOCUMENT(R"(Contains all the available statistics about the captured frame.

Currently this information is only available on D3D11 and is fairly API-centric.
)");
struct FrameStatistics
{
  DOCUMENT("");
  FrameStatistics() = default;
  FrameStatistics(const FrameStatistics &) = default;

  DOCUMENT("``True`` if the statistics in this structure are valid.");
  bool recorded;

  DOCUMENT("A list of constant buffer bind statistics, one per each :class:`stage <ShaderStage>`.");
  ConstantBindStats constants[ENUM_ARRAY_SIZE(ShaderStage)];

  DOCUMENT("A list of sampler bind statistics, one per each :class:`stage <ShaderStage>`.");
  SamplerBindStats samplers[ENUM_ARRAY_SIZE(ShaderStage)];

  DOCUMENT("A list of resource bind statistics, one per each :class:`stage <ShaderStage>`.");
  ResourceBindStats resources[ENUM_ARRAY_SIZE(ShaderStage)];

  DOCUMENT("Information about resource contents updates.");
  ResourceUpdateStats updates;

  DOCUMENT("Information about drawcalls.");
  DrawcallStats draws;

  DOCUMENT("Information about compute dispatches.");
  DispatchStats dispatches;

  DOCUMENT("Information about index buffer binds.");
  IndexBindStats indices;

  DOCUMENT("Information about vertex buffer binds.");
  VertexBindStats vertices;

  DOCUMENT("Information about vertex layout binds.");
  LayoutBindStats layouts;

  DOCUMENT("A list of shader bind statistics, one per each :class:`stage <ShaderStage>`.");
  ShaderChangeStats shaders[ENUM_ARRAY_SIZE(ShaderStage)];

  DOCUMENT("Information about blend state binds.");
  BlendStats blends;

  DOCUMENT("Information about depth-stencil state binds.");
  DepthStencilStats depths;

  DOCUMENT("Information about rasterizer state binds.");
  RasterizationStats rasters;

  DOCUMENT("Information about output merger and UAV binds.");
  OutputTargetStats outputs;
};

DECLARE_REFLECTION_STRUCT(FrameStatistics);

DOCUMENT("Contains frame-level global information");
struct FrameDescription
{
  DOCUMENT("");
  FrameDescription()
      : frameNumber(0),
        fileOffset(0),
        uncompressedFileSize(0),
        compressedFileSize(0),
        persistentSize(0),
        initDataSize(0),
        captureTime(0)
  {
  }
  FrameDescription(const FrameDescription &) = default;

  DOCUMENT(R"(Starting from frame #1 defined as the time from application startup to first present,
this counts the frame number when the capture was made.

.. note:: This value is only accurate if the capture was triggered through the default mechanism, if
  it was triggered from the application API it doesn't correspond to anything.
)");
  uint32_t frameNumber;

  DOCUMENT(R"(The offset into the file of the start of the frame.

.. note:: Similarly to :data:`APIEvent.fileOffset` this should only be used as a relative measure,
  as it is not a literal number of bytes from the start of the file on disk.
)");
  uint64_t fileOffset;

  DOCUMENT("The total file size of the whole capture in bytes, after decompression.");
  uint64_t uncompressedFileSize;

  DOCUMENT("The total file size of the whole capture in bytes, before decompression.");
  uint64_t compressedFileSize;

  DOCUMENT("The byte size of the section of the file that must be kept in memory persistently.");
  uint64_t persistentSize;

  DOCUMENT("The byte size of the section of the file that contains frame-initial contents.");
  uint64_t initDataSize;

  DOCUMENT("The time when the capture was created, as a unix timestamp in UTC.");
  uint64_t captureTime;

  DOCUMENT("The :class:`frame statistics <FrameStatistics>`.");
  FrameStatistics stats;

  DOCUMENT("A list of debug messages that are not associated with any particular event.");
  rdcarray<DebugMessage> debugMessages;
};

DECLARE_REFLECTION_STRUCT(FrameDescription);

DOCUMENT(
    "Describes a particular use of a resource at a specific :data:`eventId <APIEvent.eventId>`.");
struct EventUsage
{
  DOCUMENT("");
  EventUsage() : eventId(0), usage(ResourceUsage::Unused) {}
  EventUsage(const EventUsage &) = default;
  EventUsage(uint32_t e, ResourceUsage u) : eventId(e), usage(u) {}
  EventUsage(uint32_t e, ResourceUsage u, ResourceId v) : eventId(e), usage(u), view(v) {}
  bool operator<(const EventUsage &o) const
  {
    if(!(eventId == o.eventId))
      return eventId < o.eventId;
    return usage < o.usage;
  }

  bool operator==(const EventUsage &o) const { return eventId == o.eventId && usage == o.usage; }
  DOCUMENT("The :data:`eventId <APIEvent.eventId>` where this usage happened.");
  uint32_t eventId;

  DOCUMENT("The :class:`ResourceUsage` in question.");
  ResourceUsage usage;

  DOCUMENT("An optional :class:`ResourceId` identifying the view through which the use happened.");
  ResourceId view;
};

DECLARE_REFLECTION_STRUCT(EventUsage);

DOCUMENT("Describes the properties of a drawcall, dispatch, debug marker, or similar event.");
struct DrawcallDescription
{
  DOCUMENT("");
  DrawcallDescription() { Reset(); }
  DrawcallDescription(const DrawcallDescription &) = default;

  DOCUMENT("Resets the drawcall back to a default/empty state.");
  void Reset()
  {
    eventId = 0;
    drawcallId = 0;
    flags = DrawFlags::NoFlags;
    markerColor[0] = markerColor[1] = markerColor[2] = markerColor[3] = 0.0f;
    numIndices = 0;
    numInstances = 0;
    indexOffset = 0;
    baseVertex = 0;
    vertexOffset = 0;
    instanceOffset = 0;
    drawIndex = 0;

    dispatchDimension[0] = dispatchDimension[1] = dispatchDimension[2] = 0;
    dispatchThreadsDimension[0] = dispatchThreadsDimension[1] = dispatchThreadsDimension[2] = 0;
    dispatchBase[0] = dispatchBase[1] = dispatchBase[2] = 0;

    indexByteWidth = 0;
    topology = Topology::Unknown;

    copySource = ResourceId();
    copyDestination = ResourceId();

    parent = previous = next = NULL;

    for(int i = 0; i < 8; i++)
      outputs[i] = ResourceId();
    depthOut = ResourceId();
  }
  DOCUMENT("");
  bool operator==(const DrawcallDescription &o) const { return eventId == o.eventId; }
  bool operator<(const DrawcallDescription &o) const { return eventId < o.eventId; }
  DOCUMENT("The :data:`eventId <APIEvent.eventId>` that actually produced the drawcall.");
  uint32_t eventId;
  DOCUMENT("A 1-based index of this drawcall relative to other drawcalls.");
  uint32_t drawcallId;

  DOCUMENT(R"(The name of this drawcall. Typically a summarised/concise list of parameters.

.. note:: For drawcalls, the convention is to list primary parameters (vertex/index count, instance
  count) and omit secondary parameters (vertex offset, instance offset).
)");
  rdcstr name;

  DOCUMENT("A set of :class:`DrawFlags` properties describing what kind of drawcall this is.");
  DrawFlags flags;

  DOCUMENT("A RGBA color specified by a debug marker call.");
  float markerColor[4];

  DOCUMENT("The number of indices or vertices as appropriate for the drawcall. 0 if not used.");
  uint32_t numIndices;

  DOCUMENT("The number of instances for the drawcall. 0 if not used.");
  uint32_t numInstances;

  DOCUMENT("For indexed drawcalls, the offset added to each index after fetching.");
  int32_t baseVertex;

  DOCUMENT("For indexed drawcalls, the first index to fetch from the index buffer.");
  uint32_t indexOffset;

  DOCUMENT("For non-indexed drawcalls, the offset applied before looking up each vertex input.");
  uint32_t vertexOffset;

  DOCUMENT(
      "For instanced drawcalls, the offset applied before looking up instanced vertex inputs.");
  uint32_t instanceOffset;

  DOCUMENT(R"(The index of this draw in an call with multiple draws, e.g. an indirect draw.

0 if not part of a multi-draw.
)");
  uint32_t drawIndex;

  DOCUMENT("The 3D number of workgroups to dispatch in a dispatch call.");
  uint32_t dispatchDimension[3];

  DOCUMENT("The 3D size of each workgroup in threads if the call allows an override, or 0 if not.");
  uint32_t dispatchThreadsDimension[3];

  DOCUMENT("The 3D base offset of the workgroup ID if the call allows an override, or 0 if not.");
  uint32_t dispatchBase[3];

  DOCUMENT(R"(The width in bytes of each index.

Valid values are 1 (depending on API), 2 or 4, or 0 if the drawcall is not an indexed draw.
)");
  uint32_t indexByteWidth;

  DOCUMENT("The :class:`Topology` used in this drawcall.");
  Topology topology;

  DOCUMENT(R"(The :class:`ResourceId` identifying the source object in a copy, resolve or blit
operation.
)");
  ResourceId copySource;
  DOCUMENT(R"(The :class:`ResourceId` identifying the destination object in a copy, resolve or blit
operation.
)");
  ResourceId copyDestination;

  DOCUMENT(R"(The parent of this drawcall, or ``None`` if there is no parent for this drawcall.
)");
  const DrawcallDescription *parent;

  DOCUMENT(R"(The previous drawcall in the frame, or ``None`` if this is the first drawcall in the
frame.
)");
  const DrawcallDescription *previous;
  DOCUMENT(
      "The next drawcall in the frame, or ``None`` if this is the last drawcall in the frame.");
  const DrawcallDescription *next;

  DOCUMENT(R"(A simple list of the :class:`ResourceId` ids for the color outputs, which can be used
for very coarse bucketing of drawcalls into similar passes by their outputs.
)");
  ResourceId outputs[8];
  DOCUMENT("The resource used for depth output - see :data:`outputs`.");
  ResourceId depthOut;

  DOCUMENT("A list of the :class:`APIEvent` events that happened since the previous drawcall.");
  rdcarray<APIEvent> events;

  DOCUMENT("A list of :class:`DrawcallDescription` child drawcalls.");
  rdcarray<DrawcallDescription> children;
};

DECLARE_REFLECTION_STRUCT(DrawcallDescription);

DOCUMENT("Gives some API-specific information about the capture.");
struct APIProperties
{
  DOCUMENT("");
  APIProperties() = default;
  APIProperties(const APIProperties &) = default;

  DOCUMENT("The :class:`GraphicsAPI` of the actual log/capture.");
  GraphicsAPI pipelineType = GraphicsAPI::D3D11;

  DOCUMENT(R"(The :class:`GraphicsAPI` used to render the log. For remote replay this could be
different to the above, and lets the UI make decisions e.g. to flip rendering of images.
)");
  GraphicsAPI localRenderer = GraphicsAPI::D3D11;

  DOCUMENT("The :class:`GPUVendor` of the active GPU being used.");
  GPUVendor vendor = GPUVendor::Unknown;

  DOCUMENT(R"(``True`` if the capture was loaded successfully but running in a degraded mode - e.g.
with software rendering, or with some functionality disabled due to lack of support.
)");
  bool degraded = false;

  DOCUMENT(R"(``True`` if the driver mutates shader reflection structures from event to event.
Currently this is only true for OpenGL where the superfluous indirect in the binding model must be
worked around by re-sorting bindings.
)");
  bool shadersMutable = false;

  DOCUMENT("``True`` if the driver and system are configured to allow creating RGP captures.");
  bool rgpCapture = false;

#if !defined(SWIG)
  // flags about edge-case parts of the APIs that might be used in the capture.
  bool ShaderLinkage = false;
  bool YUVTextures = false;
  bool SparseResources = false;
  bool MultiGPU = false;
  bool D3D12Bundle = false;
#endif
};

DECLARE_REFLECTION_STRUCT(APIProperties);

DOCUMENT("Gives information about the driver for this API.");
struct DriverInformation
{
  DOCUMENT("The :class:`GPUVendor` that provides this driver");
  GPUVendor vendor;

  DOCUMENT("The version string for the driver");
  char version[128];
};

DECLARE_REFLECTION_STRUCT(DriverInformation);

DOCUMENT("A 128-bit Uuid.");
struct Uuid
{
  DOCUMENT("");
  Uuid(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
  {
    words[0] = a;
    words[1] = b;
    words[2] = c;
    words[3] = d;
  }

  Uuid() { words[0] = words[1] = words[2] = words[3] = 0; }
  Uuid(const Uuid &) = default;

  DOCUMENT("Compares two ``Uuid`` objects for less-than.");
  bool operator<(const Uuid &rhs) const
  {
    return std::lexicographical_compare(words, words + 4, rhs.words, rhs.words + 4);
  }

  DOCUMENT("Compares two ``Uuid`` objects for equality.");
  bool operator==(const Uuid &rhs) const { return ::memcmp(words, rhs.words, sizeof(words)) == 0; }
  DOCUMENT("The Uuid bytes as an array of four 32-bit integers.")
  uint32_t words[4];
};

DECLARE_REFLECTION_STRUCT(Uuid);

DOCUMENT("Describes a GPU counter's purpose and result value.");
struct CounterDescription
{
  DOCUMENT("");
  CounterDescription() = default;
  CounterDescription(const CounterDescription &) = default;

  DOCUMENT(R"(The :class:`GPUCounter` this counter represents.

.. note:: The value may not correspond to any of the predefined values if it's a hardware-specific
  counter value.
)");
  GPUCounter counter;

  DOCUMENT("A short human-readable name for the counter.");
  rdcstr name;

  DOCUMENT("The counter category. Can be empty for uncategorized counters.");
  rdcstr category;

  DOCUMENT("If available, a longer human-readable description of the value this counter measures.");
  rdcstr description;

  DOCUMENT("The :class:`type of value <CompType>` returned by this counter.");
  CompType resultType;

  DOCUMENT("The number of bytes in the resulting value.");
  uint32_t resultByteWidth;

  DOCUMENT("The :class:`CounterUnit` for the result value.");
  CounterUnit unit;

  DOCUMENT("The :class:`Uuid` of this counter, which uniquely identifies it.");
  Uuid uuid;
};

DECLARE_REFLECTION_STRUCT(CounterDescription);

DOCUMENT(R"(A resulting value from a GPU counter. Only one member is valid, see
:class:`CounterDescription`.
)");
union CounterValue
{
  DOCUMENT("A ``float`` value.");
  float f;
  DOCUMENT("A ``double`` value.");
  double d;
  DOCUMENT("A 32-bit unsigned integer.");
  uint32_t u32;
  DOCUMENT("A 64-bit unsigned integer.");
  uint64_t u64;
};

DECLARE_REFLECTION_STRUCT(CounterValue);

DOCUMENT("The resulting value from a counter at an event.");
struct CounterResult
{
  CounterResult() : eventId(0), counter(GPUCounter::EventGPUDuration) { value.u64 = 0; }
  CounterResult(const CounterResult &) = default;
  CounterResult(uint32_t e, GPUCounter c, float data) : eventId(e), counter(c) { value.f = data; }
  CounterResult(uint32_t e, GPUCounter c, double data) : eventId(e), counter(c) { value.d = data; }
  CounterResult(uint32_t e, GPUCounter c, uint32_t data) : eventId(e), counter(c)
  {
    value.u32 = data;
  }
  CounterResult(uint32_t e, GPUCounter c, uint64_t data) : eventId(e), counter(c)
  {
    value.u64 = data;
  }

  DOCUMENT("Compares two ``CounterResult`` objects for less-than.");
  bool operator<(const CounterResult &o) const
  {
    if(!(eventId == o.eventId))
      return eventId < o.eventId;
    if(!(counter == o.counter))
      return counter < o.counter;

    // don't compare values, just consider equal
    return false;
  }

  DOCUMENT("Compares two ``CounterResult`` objects for equality.");
  bool operator==(const CounterResult &o) const
  {
    // don't compare values, just consider equal by eventId/counterID
    return eventId == o.eventId && counter == o.counter;
  }

  DOCUMENT("The :data:`eventId <APIEvent.eventId>` that produced this value.");
  uint32_t eventId;

  DOCUMENT("The :data:`counter <GPUCounter>` that produced this value.");
  GPUCounter counter;

  DOCUMENT("The value itself.");
  CounterValue value;
};

DECLARE_REFLECTION_STRUCT(CounterResult);

DOCUMENT("The contents of an RGBA pixel.");
union PixelValue
{
  DOCUMENT("The RGBA value interpreted as ``float``.");
  float floatValue[4];
  DOCUMENT("The RGBA value interpreted as 32-bit unsigned integer.");
  uint32_t uintValue[4];
  DOCUMENT("The RGBA value interpreted as 32-bit signed integer.");
  int32_t intValue[4];
};

DECLARE_REFLECTION_STRUCT(PixelValue);

DOCUMENT("The value of pixel output at a particular event.");
struct ModificationValue
{
  DOCUMENT("");
  ModificationValue() = default;
  ModificationValue(const ModificationValue &) = default;

  bool operator==(const ModificationValue &o) const
  {
    return !memcmp(&col, &o.col, sizeof(col)) && depth == o.depth && stencil == o.stencil;
  }
  bool operator<(const ModificationValue &o) const
  {
    if(memcmp(&col, &o.col, sizeof(col)) < 0)
      return true;
    if(!(depth == o.depth))
      return depth < o.depth;
    if(!(stencil == o.stencil))
      return stencil < o.stencil;
    return false;
  }
  DOCUMENT("The color value.");
  PixelValue col;

  DOCUMENT("The depth output, as a ``float``.");
  float depth;

  DOCUMENT("The stencil output, or ``-1`` if not available.");
  int32_t stencil;
};

DECLARE_REFLECTION_STRUCT(ModificationValue);

DOCUMENT("An attempt to modify a pixel by a particular event.");
struct PixelModification
{
  DOCUMENT("");
  PixelModification() = default;
  PixelModification(const PixelModification &) = default;

  bool operator==(const PixelModification &o) const
  {
    return eventId == o.eventId && directShaderWrite == o.directShaderWrite &&
           unboundPS == o.unboundPS && fragIndex == o.fragIndex && primitiveID == o.primitiveID &&
           preMod == o.preMod && shaderOut == o.shaderOut && postMod == o.postMod &&
           sampleMasked == o.sampleMasked && backfaceCulled == o.backfaceCulled &&
           depthClipped == o.depthClipped && viewClipped == o.viewClipped &&
           scissorClipped == o.scissorClipped && shaderDiscarded == o.shaderDiscarded &&
           depthTestFailed == o.depthTestFailed && stencilTestFailed == o.stencilTestFailed;
  }
  bool operator<(const PixelModification &o) const
  {
    if(!(eventId == o.eventId))
      return eventId < o.eventId;
    if(!(directShaderWrite == o.directShaderWrite))
      return directShaderWrite < o.directShaderWrite;
    if(!(unboundPS == o.unboundPS))
      return unboundPS < o.unboundPS;
    if(!(fragIndex == o.fragIndex))
      return fragIndex < o.fragIndex;
    if(!(primitiveID == o.primitiveID))
      return primitiveID < o.primitiveID;
    if(!(preMod == o.preMod))
      return preMod < o.preMod;
    if(!(shaderOut == o.shaderOut))
      return shaderOut < o.shaderOut;
    if(!(postMod == o.postMod))
      return postMod < o.postMod;
    if(!(sampleMasked == o.sampleMasked))
      return sampleMasked < o.sampleMasked;
    if(!(backfaceCulled == o.backfaceCulled))
      return backfaceCulled < o.backfaceCulled;
    if(!(depthClipped == o.depthClipped))
      return depthClipped < o.depthClipped;
    if(!(viewClipped == o.viewClipped))
      return viewClipped < o.viewClipped;
    if(!(scissorClipped == o.scissorClipped))
      return scissorClipped < o.scissorClipped;
    if(!(shaderDiscarded == o.shaderDiscarded))
      return shaderDiscarded < o.shaderDiscarded;
    if(!(depthTestFailed == o.depthTestFailed))
      return depthTestFailed < o.depthTestFailed;
    if(!(stencilTestFailed == o.stencilTestFailed))
      return stencilTestFailed < o.stencilTestFailed;
    return false;
  }
  DOCUMENT("The :data:`eventId <APIEvent.eventId>` where the modification happened.");
  uint32_t eventId;

  DOCUMENT("``True`` if this event came as part of an arbitrary shader write.");
  bool directShaderWrite;

  DOCUMENT("``True`` if no pixel shader was bound at this event.");
  bool unboundPS;

  DOCUMENT(R"(A 0-based index of which fragment this modification corresponds to, in the case that
multiple fragments from a single draw wrote to a pixel.
)");
  uint32_t fragIndex;

  DOCUMENT("The primitive that generated this fragment.");
  uint32_t primitiveID;

  DOCUMENT(R"(The :class:`ModificationValue` of the texture before this fragment ran.

This is valid only for the first fragment if multiple fragments in the same event write to the same
pixel.
)");
  ModificationValue preMod;
  DOCUMENT("The :class:`ModificationValue` that this fragment wrote from the pixel shader.");
  ModificationValue shaderOut;
  DOCUMENT(R"(The :class:`ModificationValue` of the texture after this fragment ran.)");
  ModificationValue postMod;

  DOCUMENT("``True`` if the sample mask eliminated this fragment.");
  bool sampleMasked;
  DOCUMENT("``True`` if the backface culling test eliminated this fragment.");
  bool backfaceCulled;
  DOCUMENT("``True`` if depth near/far clipping eliminated this fragment.");
  bool depthClipped;
  DOCUMENT("``True`` if viewport clipping eliminated this fragment.");
  bool viewClipped;
  DOCUMENT("``True`` if scissor clipping eliminated this fragment.");
  bool scissorClipped;
  DOCUMENT("``True`` if the pixel shader executed a discard on this fragment.");
  bool shaderDiscarded;
  DOCUMENT("``True`` if depth testing eliminated this fragment.");
  bool depthTestFailed;
  DOCUMENT("``True`` if stencil testing eliminated this fragment.");
  bool stencilTestFailed;
  DOCUMENT("``True`` if predicated rendering skipped this call.");
  bool predicationSkipped;

  DOCUMENT(R"(Determine if this fragment passed all tests and wrote to the texture.

:return: ``True`` if it passed all tests, ``False`` if it failed any.
:rtype: ``bool``
)");
  bool Passed() const
  {
    return !sampleMasked && !backfaceCulled && !depthClipped && !viewClipped && !scissorClipped &&
           !shaderDiscarded && !depthTestFailed && !stencilTestFailed && !predicationSkipped;
  }
};

DECLARE_REFLECTION_STRUCT(PixelModification);

DOCUMENT("Contains the bytes and metadata describing a thumbnail.");
struct Thumbnail
{
  DOCUMENT("");
  Thumbnail() = default;
  Thumbnail(const Thumbnail &) = default;

  DOCUMENT("The :class:`FileType` of the data in the thumbnail.");
  FileType type = FileType::Raw;

  DOCUMENT("The ``bytes`` byte array containing the raw data.");
  bytebuf data;

  DOCUMENT("The width of the thumbnail image.");
  uint32_t width = 0;

  DOCUMENT("The height of the thumbnail image.");
  uint32_t height = 0;
};

DECLARE_REFLECTION_STRUCT(Thumbnail);
