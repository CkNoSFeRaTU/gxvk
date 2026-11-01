#pragma once

#include "glide_include.h"

namespace dxvk {
  typedef struct {
    float x, y, z;
    float nx, ny, nz;
    float s, t;
  } Vert;

  typedef struct {
    int v[3];
    int matIndex;
    int edgeAntialias;
  } Face;

  typedef float Vector[3];
  typedef float Matrix[16];

  typedef FxU32 Palette[VOODOO_PALETTE_TABLE_SIZE];

  typedef struct {
    FxU8  yRGB[16];
    FxI16 iRGB[4][3];
    FxI16 qRGB[4][3];
    FxU32 packed_data[12];
  } NCCTable;

  typedef union {
    Palette  palette;
    NCCTable nccTable;
  } TextureTable;

  typedef struct {
    glide3x::GrTexInfo    info;
    FxU32        addr;
    GrTexTable_t tableType;
    TextureTable tableData;
  } Texture;

  void doSplash(GlideDevice* device, FxFloat x, FxFloat y, FxFloat w, FxFloat h, FxU32 frame);

}