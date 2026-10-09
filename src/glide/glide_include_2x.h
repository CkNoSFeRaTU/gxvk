#pragma once

#include "glide_include_common.h"

namespace glide2x {
  // TODO: state stub
  constexpr ::FxI32 GLIDE_STATE_PAD_SIZE = 312;
  typedef struct _GrState_s {
    char pad[GLIDE_STATE_PAD_SIZE];
  } GrState;

  enum GrAspectRatio_t : ::FxI32 {
    GR_ASPECT_8x1 = 0x00, /* 8W x 1H */
    GR_ASPECT_4x1 = 0x01, /* 4W x 1H */
    GR_ASPECT_2x1 = 0x02, /* 2W x 1H */
    GR_ASPECT_1x1 = 0x03, /* 1W x 1H */
    GR_ASPECT_1x2 = 0x04, /* 1W x 2H */
    GR_ASPECT_1x4 = 0x05, /* 1W x 4H */
    GR_ASPECT_1x8 = 0x06, /* 1W x 8H */
  };

  enum GrControl_t : ::FxU32 {
    GR_CONTROL_ACTIVATE = 0x01,
    GR_CONTROL_DEACTIVATE = 0x02,
    GR_CONTROL_RESIZE = 0x03,
    GR_CONTROL_MOVE = 0x04,
  };

  enum GrLfbSrcFmt_t : ::FxU32 {
    GR_LFB_SRC_FMT_565 = 0x00,
    GR_LFB_SRC_FMT_555 = 0x01,
    GR_LFB_SRC_FMT_1555 = 0x02,
    GR_LFB_SRC_FMT_888 = 0x04,
    GR_LFB_SRC_FMT_8888 = 0x05,
    GR_LFB_SRC_FMT_565_DEPTH = 0x0C,
    GR_LFB_SRC_FMT_555_DEPTH = 0x0D,
    GR_LFB_SRC_FMT_1555_DEPTH = 0x0E,
    GR_LFB_SRC_FMT_ZA16 = 0x0F,
    GR_LFB_SRC_FMT_RLE16 = 0x80,
  };

  enum GrLOD_t : FxI32 {
    GR_LOD_256 = 0x00,
    GR_LOD_128 = 0x01,
    GR_LOD_64 = 0x02,
    GR_LOD_32 = 0x03,
    GR_LOD_16 = 0x04,
    GR_LOD_8 = 0x05,
    GR_LOD_4 = 0x06,
    GR_LOD_2 = 0x07,
    GR_LOD_1 = 0x08,
  };

  enum GrPassthruMode_t : ::FxI32 {
    GR_PASSTHRU_SHOW_VGA = 0x00,
    GR_PASSTHRU_SHOW_SST1 = 0x01,
  };

  enum GrHint_t : ::FxU32 {
    GR_HINTTYPE_MIN = 0x00,
    GR_HINT_STWHINT = 0x00,
    GR_HINT_FIFOCHECKHINT = 0x01,
    GR_HINT_FPUPRECISION = 0x02,
    GR_HINT_ALLOW_MIPMAP_DITHER = 0x03,
    GR_HINT_LFB_WRITE = 0x04,
    GR_HINT_LFB_PROTECT = 0x05,
    GR_HINT_LFB_RESET = 0x06,
    GR_HINTTYPE_MAX = GR_HINT_LFB_RESET,
  };

  enum GrFogMode_t : ::FxI32 {
    GR_FOG_DISABLE = 0x0,
    GR_FOG_WITH_ITERATED_ALPHA = 0x1,
    GR_FOG_WITH_TABLE = 0x2,
    GR_FOG_WITH_ITERATED_Z = 0x3,
    GR_FOG_MULT2 = 0x100,
    GR_FOG_ADD2 = 0x200,
  };

  enum GrSTWHint_t : ::FxU32 {
    GR_STWHINT_W_DIFF_FBI = FXBIT(0),
    GR_STWHINT_W_DIFF_TMU0 = FXBIT(1),
    GR_STWHINT_ST_DIFF_TMU0 = FXBIT(2),
    GR_STWHINT_W_DIFF_TMU1 = FXBIT(3),
    GR_STWHINT_ST_DIFF_TMU1 = FXBIT(4),
    GR_STWHINT_W_DIFF_TMU2 = FXBIT(5),
    GR_STWHINT_ST_DIFF_TMU2 = FXBIT(6),
  };

  #define GR_GENERATE_FIFOCHECK_HINT_MASK(swHWM, swLWM) \
    (((swHWM & 0xffff) << 16) | (swLWM & 0xffff))

  enum GrVertexLayoutOffset_t : ::FxU32 {
    GR_VERTEX_X_OFFSET = 0x00,
    GR_VERTEX_Y_OFFSET = 0x01,
    GR_VERTEX_Z_OFFSET = 0x02,
    GR_VERTEX_R_OFFSET = 0x03,
    GR_VERTEX_G_OFFSET = 0x04,
    GR_VERTEX_B_OFFSET = 0x05,
    GR_VERTEX_OOZ_OFFSET = 0x06,
    GR_VERTEX_A_OFFSET = 0x07,
    GR_VERTEX_OOW_OFFSET = 0x08,
    GR_VERTEX_SOW_TMU0_OFFSET = 0x09,
    GR_VERTEX_TOW_TMU0_OFFSET = 0x10,
    GR_VERTEX_OOW_TMU0_OFFSET = 0x11,
    GR_VERTEX_SOW_TMU1_OFFSET = 0x12,
    GR_VERTEX_TOW_TMU1_OFFSET = 0x13,
    GR_VERTEX_OOW_TMU1_OFFSET = 0x14,
    GR_VERTEX_SOW_TMU2_OFFSET = 0x15,
    GR_VERTEX_TOW_TMU2_OFFSET = 0x16,
    GR_VERTEX_OOW_TMU2_OFFSET = 0x17,
  };

  typedef struct {
    float  sow;    /* s texture ordinate (s over w) */
    float  tow;    /* t texture ordinate (t over w) */
    float  oow;    /* 1/w (used mipmapping - really 0xfff/w) */
  } GrTmuVertex;

  typedef struct {
    float x, y, z; /* X, Y, and Z of scrn space -- Z is ignored */
    float r, g, b; /* R, G, B, ([0..255.0]) */
    float ooz;     /* 65535/Z (used for Z-buffering) */
    float a;       /* Alpha [0..255.0] */
    float oow;     /* 1/W (used for W-buffering, texturing) */
    GrTmuVertex tmuvtx[GLIDE_NUM_TMU];
  } GrVertex;

  typedef struct {
    FxU32               width, height;
    int                 small_lod, large_lod;
    GrAspectRatio_t     aspect_ratio;
    GrTextureFormat_t   format;
  } Gu3dfHeader;

  typedef struct {
    Gu3dfHeader  header;
    GuTexTable   table;
    void        *data;
    FxU32        mem_required;    /* memory required for mip map in bytes. */
  } Gu3dfInfo;

  typedef struct {
    GrLOD_t           smallLod;
    GrLOD_t           largeLod;
    GrAspectRatio_t   aspectRatio;
    GrTextureFormat_t format;
    void              *data;
  } GrTexInfo;

  typedef struct {
    int           sst;                    /* SST where this texture map was stored  */
    FxBool        valid;                  /* set when this table entry is allocated*/
    int           width, height;
    GrAspectRatio_t aspect_ratio;         /* aspect ratio of the mip map.  */
    void          *data;                  /* actual texture data  */

    GrTextureFormat_t  format;                    /* format of the texture table */
    GrMipMapMode_t     mipmap_mode;               /* mip map mode for this texture */
    GrTextureFilterMode_t   magfilter_mode;       /* filtering to be used when magnified */
    GrTextureFilterMode_t   minfilter_mode;       /* filtering to be used with minified  */
    GrTextureClampMode_t    s_clamp_mode;         /* how this texture should be clamped in s */
    GrTextureClampMode_t    t_clamp_mode;         /* how this texture should be clamped in t */
    FxU32         tLOD;                   /* Register value for tLOD register */
    FxU32         tTextureMode;           /* Register value for tTextureMode register
                                            not including non-texture specific bits */
    FxU32         lod_bias;               /* LOD bias of the mip map in preshifted 4.2*/
    GrLOD_t       lod_min, lod_max;       /* largest and smallest levels of detail  */
    int           tmu;                    /* tmu on which this texture resides */
    FxU32         odd_even_mask;          /* mask specifying levels on this tmu  */
    FxU32         tmu_base_address;       /* base addr (in TMU mem) of this texture */
    FxBool        trilinear;              /* should we blend by lod? */

    GuNccTable    ncc_table;              /* NCC compression table (optional) */
  } GrMipMapInfo;
}
