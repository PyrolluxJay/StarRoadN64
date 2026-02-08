#include "src/game/envfx_snow.h"

extern Gfx *geo_update_layer_alpha(s32 callContext, struct GraphNode *node, UNUSED void *context);
const GeoLayout checkmark_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ASM(LAYER_TRANSPARENT, geo_update_layer_alpha),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, checkmark_Plane_mesh_layer_5),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
