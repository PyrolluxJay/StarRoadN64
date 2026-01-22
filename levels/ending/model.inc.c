Vtx ending_dl_Plane_001_mesh_layer_1_vtx_cull[8] = {
	{{ {-5304, -3000, 5304}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-5304, -3000, 5304}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-5304, -3000, -5304}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-5304, -3000, -5304}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {5304, -3000, 5304}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {5304, -3000, 5304}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {5304, -3000, -5304}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {5304, -3000, -5304}, 0, {0, 0}, {0, 0, 0, 0} }},
};

Vtx ending_dl_Plane_001_mesh_layer_1_vtx_0[4] = {
	{{ {5304, -3000, -5304}, 0, {-16, 1008}, {0, 127, 0, 255} }},
	{{ {-5304, -3000, -5304}, 0, {1008, 1008}, {0, 127, 0, 255} }},
	{{ {-5304, -3000, 5304}, 0, {1008, -16}, {0, 127, 0, 255} }},
	{{ {5304, -3000, 5304}, 0, {-16, -16}, {0, 127, 0, 255} }},
};

Gfx ending_dl_Plane_001_mesh_layer_1[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(ending_dl_Plane_001_mesh_layer_1_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPLightColor(LIGHT_1, 0xFFFFFFFF),
	gsSPLightColor(LIGHT_2, 0x7F7F7FFF),
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsDPSetAlphaDither(G_AD_NOISE),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPVertex(ending_dl_Plane_001_mesh_layer_1_vtx_0 + 0, 4, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsDPPipeSync(),
	gsDPSetAlphaDither(G_AD_DISABLE),
	gsSPEndDisplayList(),
};

Gfx ending_dl_final_revert_mesh_layer_1[] = {
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

