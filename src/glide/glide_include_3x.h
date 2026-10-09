#pragma once

#include "glide_include_common.h"

namespace glide3x {

  constexpr ::FxU32 GR_QUERY_ANY = 0xFFFFFFFF;
  constexpr ::FxU32 GR_EXTENDED = 0x7FFF7FFF;
  typedef ::FxI32 GrVertexLayoutOffset_t;
  typedef ::FxU32 GrStipplePattern_t;

  enum GrAspectRatio_t : ::FxI32 {
    GR_ASPECT_LOG2_8x1 = 3,  /* 8W x 1H */
    GR_ASPECT_LOG2_4x1 = 2,  /* 4W x 1H */
    GR_ASPECT_LOG2_2x1 = 1,  /* 2W x 1H */
    GR_ASPECT_LOG2_1x1 = 0,  /* 1W x 1H */
    GR_ASPECT_LOG2_1x2 = -1, /* 1W x 2H */
    GR_ASPECT_LOG2_1x4 = -2, /* 1W x 4H */
    GR_ASPECT_LOG2_1x8 = -3, /* 1W x 8H */
  };

  enum GrCoordinateSpaceMode_t : ::FxU32 {
    GR_WINDOW_COORDS = 0x00,
    GR_CLIP_COORDS = 0x01,
  };

  enum GrFogMode_t : ::FxI32 {
    GR_FOG_DISABLE = 0x00,
    GR_FOG_WITH_TABLE_ON_FOGCOORD_EXT = 0x01,
    GR_FOG_WITH_TABLE_ON_Q = 0x02,
    GR_FOG_WITH_TABLE_ON_W = GR_FOG_WITH_TABLE_ON_Q,
    GR_FOG_WITH_ITERATED_Z = 0x03,
    GR_FOG_WITH_ITERATED_ALPHA_EXT = 0x04,
    GR_FOG_MULT2 = 0x0100,
    GR_FOG_ADD2 = 0x0200,
  };

  enum GrLOD_t : FxI32 {
    GR_LOD_LOG2_1 = 0x00,
    GR_LOD_LOG2_2 = 0x01,
    GR_LOD_LOG2_4 = 0x02,
    GR_LOD_LOG2_8 = 0x03,
    GR_LOD_LOG2_16 = 0x04,
    GR_LOD_LOG2_32 = 0x05,
    GR_LOD_LOG2_64 = 0x06,
    GR_LOD_LOG2_128 = 0x07,
    GR_LOD_LOG2_256 = 0x08,
    // Napalm
    GR_LOD_LOG2_512 = 0x09,
    GR_LOD_LOG2_1024 = 0x0a,
    GR_LOD_LOG2_2048 = 0x0b,
  };

  enum GrStippleMode_t : ::FxI32 {
    GR_STIPPLE_DISABLE = 0x00,
    GR_STIPPLE_PATTERN = 0x01,
    GR_STIPPLE_ROTATE = 0x02,
  };

  enum GrColorType_t : ::FxU32 {
    GR_FLOAT = 0x00,
    GR_U8 = 0x01,
  };

  enum GrVertexLayoutParam_t : ::FxU32 {
    GR_PARAM_XY = 0x01,
    GR_PARAM_Z = 0x02,
    GR_PARAM_W = 0x03,
    GR_PARAM_Q = 0x04,
    GR_PARAM_FOG_EXT = 0x05,
    GR_PARAM_A = 0x10,
    GR_PARAM_RGB = 0x20,
    GR_PARAM_PARGB = 0x30,
    GR_PARAM_ST0 = 0x40,
    GR_PARAM_ST1 = GR_PARAM_ST0 + 0x01,
    GR_PARAM_ST2 = GR_PARAM_ST0 + 0x02,
    GR_PARAM_Q0 = 0x50,
    GR_PARAM_Q1 = GR_PARAM_Q0 + 0x01,
    GR_PARAM_Q2 = GR_PARAM_Q0 + 0x02,
  };

  enum GrVertexLayoutMode_t : ::FxU32 {
    GR_PARAM_DISABLE = 0x00,
    GR_PARAM_ENABLE = 0x01,
  };

  enum GrDrawVertexArrayMode_t : ::FxU32 {
    GR_POINTS = 0x00,
    GR_LINE_STRIP = 0x01,
    GR_LINES = 0x02,
    GR_POLYGON = 0x03,
    GR_TRIANGLE_STRIP = 0x04,
    GR_TRIANGLE_FAN = 0x05,
    GR_TRIANGLES = 0x06,
    GR_TRIANGLE_STRIP_CONTINUE = 0x07,
    GR_TRIANGLE_FAN_CONTINUE = 0x08,
  };

  enum GrGetReset_t : ::FxU32 {
    GR_BITS_DEPTH = 0x01,
    GR_BITS_RGBA = 0x02,
    GR_FIFO_FULLNESS = 0x03,
    GR_FOG_TABLE_ENTRIES = 0x04,
    GR_GAMMA_TABLE_ENTRIES = 0x05,
    GR_GLIDE_STATE_SIZE = 0x06,
    GR_GLIDE_VERTEXLAYOUT_SIZE = 0x07,
    GR_IS_BUSY = 0x08,
    GR_LFB_PIXEL_PIPE = 0x09,
    GR_MAX_TEXTURE_SIZE = 0x0A,
    GR_MAX_TEXTURE_ASPECT_RATIO = 0x0B,
    GR_MEMORY_FB = 0x0C,
    GR_MEMORY_TMU = 0x0D,
    GR_MEMORY_UMA = 0x0E,
    GR_NUM_BOARDS = 0x0F,
    GR_NON_POWER_OF_TWO_TEXTURES = 0x10,
    GR_NUM_FB = 0x11,
    GR_NUM_SWAP_HISTORY_BUFFER = 0x12,
    GR_NUM_TMU = 0x13,
    GR_PENDING_BUFFERSWAPS = 0x14,
    GR_REVISION_FB = 0x15,
    GR_REVISION_TMU = 0x16,
    GR_STATS_LINES = 0x17,
    GR_STATS_PIXELS_AFUNC_FAIL = 0x18,
    GR_STATS_PIXELS_CHROMA_FAIL = 0x19,
    GR_STATS_PIXELS_DEPTHFUNC_FAIL = 0x1A,
    GR_STATS_PIXELS_IN = 0x1B,
    GR_STATS_PIXELS_OUT = 0x1C,
    GR_STATS_PIXELS = 0x1D,
    GR_STATS_POINTS = 0x1E,
    GR_STATS_TRIANGLES_IN = 0x1F,
    GR_STATS_TRIANGLES_OUT = 0x20,
    GR_STATS_TRIANGLES = 0x21,
    GR_SWAP_HISTORY = 0x22,
    GR_SUPPORTS_PASSTHRU = 0x23,
    GR_TEXTURE_ALIGN = 0x24,
    GR_VIDEO_POSITION = 0x25,
    GR_VIEWPORT = 0x26,
    GR_WDEPTH_MIN_MAX = 0x27,
    GR_ZDEPTH_MIN_MAX = 0x28,
    GR_VERTEX_PARAMETER = 0x29,
    GR_BITS_GAMMA = 0x2A,
    GR_GET_RESERVED_1 = 0x1000,
  };

  enum GrGetParam_t : ::FxU32 {
    GR_EXTENSION = 0xA0,
    GR_HARDWARE = 0xA1,
    GR_RENDERER = 0xA2,
    GR_VENDOR = 0xA3,
    GR_VERSION = 0xA4,
  };

  typedef struct {
    GrScreenResolution_t resolution;
    GrScreenRefresh_t    refresh;
    FxI32                numColorBuffers;
    FxI32                numAuxBuffers;
  } GrResolution;

  typedef struct {
    FxU32               width, height;
    FxI32               small_lod, large_lod;
    GrAspectRatio_t     aspect_ratio;
    GrTextureFormat_t   format;
  } Gu3dfHeader;

  typedef struct
  {
    Gu3dfHeader  header;
    GuTexTable   table;
    void        *data;
    FxU32        mem_required;    /* memory required for mip map in bytes. */
  } Gu3dfInfo;

  typedef struct {
      GrLOD_t           smallLodLog2;
      GrLOD_t           largeLodLog2;
      GrAspectRatio_t   aspectRatioLog2;
      GrTextureFormat_t format;
      void              *data;
  } GrTexInfo;
}
