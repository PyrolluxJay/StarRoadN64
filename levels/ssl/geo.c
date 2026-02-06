#include <ultra64.h>
#include "sm64.h"
#include "geo_commands.h"

#include "game/level_geo.h"
#include "game/geo_misc.h"
#include "game/camera.h"
#include "game/moving_texture.h"
#include "game/screen_transition.h"
#include "game/paintings.h"

#include "make_const_nonconst.h"

#include "levels/ssl/header.h"

extern const Gfx purple_switch_seg8_dl_0800C718[];
extern const Gfx zo_enter[];
extern const Gfx zo_exit[];
const GeoLayout purple_switch_zo_geo[] = {
   GEO_CULLING_RADIUS(300),
   GEO_OPEN_NODE(),
      GEO_DISPLAY_LIST(LAYER_OPAQUE, zo_enter),
      GEO_DISPLAY_LIST(LAYER_OPAQUE, purple_switch_seg8_dl_0800C718),
      GEO_DISPLAY_LIST(LAYER_OPAQUE, zo_exit),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

#include "levels/ssl/geo.inc.c"
#include "levels/ssl/custom_c/custom.modelopt.inc.h"
#include "levels/ssl/custom_c/custom.geo.inc.c"
#include "levels/ssl/custom_c/object.geo.inc.c"
