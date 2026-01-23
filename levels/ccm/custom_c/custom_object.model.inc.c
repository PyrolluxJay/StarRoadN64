#include "custom.model.inc.h"
Vtx VB_wf_geo_000A00_0x700d388[] = {
{ 0, 192, -193, 0, -1024, 1024, -127, 0, 0, 255 },
{ 0, -192, 193, 0, 0, 0, -127, 0, 0, 255 },
{ 0, 192, 193, 0, 0, 1024, -127, 0, 0, 255 },
{ 0, -192, 193, 0, 0, 0, -127, 0, 0, 255 },
{ 0, 192, -193, 0, -1024, 1024, -127, 0, 0, 255 },
{ 0, -192, -193, 0, -1024, 0, -127, 0, 0, 255 },
};

#if 0
ALIGNED8 u8 wf_geo_000A00__texture_09008000[] = {
#include "levels/ccm/custom_textures/wf_geo_000A00_0x9008000_custom.rgba16.inc.c"
};
#else
extern u8 ccm_1__texture_0E00A810[];
#define wf_geo_000A00__texture_09008000 ccm_1__texture_0E00A810
#endif

Gfx DL_wf_geo_000A00_0x700d708[] = {
gsDPPipeSync(),
gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, SHADE, TEXEL0, 0, SHADE, 0, 0, 0, 0, SHADE),
gsSPGeometryMode(G_SHADING_SMOOTH, 0),
gsDPSetTile(0, 2, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0),
gsSPTexture(65535, 65535, 0, 0, 1),
gsDPTileSync(),
gsDPSetTile(0, 2, 8, 0, 0, 0, 0, 5, 0, 0, 5, 0),
gsDPSetTileSize(0, 0, 0, 124, 124),

gsDPSetTextureImage(0, 2, 1, wf_geo_000A00__texture_09008000),
gsDPLoadSync(),
gsDPLoadBlock(7, 0, 0, 1023, 256),
gsSPLightColor(LIGHT_1, 0xffffffff),
gsSPLightColor(LIGHT_2, 0x7f7f7fff),
gsSPVertex(VB_wf_geo_000A00_0x700d388, 6, 0),
gsSP2Triangles(0, 1, 2, 0,3, 4, 5, 0),

gsSPTexture(65535, 65535, 0, 0, 0),
gsDPPipeSync(),
gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, SHADE, 0, 0, 0, SHADE, 0, 0, 0, SHADE),
gsSPGeometryMode(0, G_SHADING_SMOOTH),
gsSPEndDisplayList(),
};
