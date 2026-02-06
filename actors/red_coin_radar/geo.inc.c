#include "src/game/envfx_snow.h"

extern Gfx red_coin_radar_Plane_mesh_layer_4[];
const GeoLayout red_coin_radar_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, red_coin_radar_Plane_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
