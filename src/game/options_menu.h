#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include <engine/n64-stdbool.h>

void optmenu_toggle(void);
void optmenu_draw(void);
void optmenu_draw_prompt(void);
void optmenu_check_buttons(void);

extern u8 optmenu_open;

extern bool configVIAntialiasing;
extern bool configVIDedither;
#define OPTION(name) extern bool name;
#include "options_menu_x.h"
#undef OPTION

extern u32 configPreset;

#define configLives (!configNoLives)

void gen_preset();

#endif // OPTIONS_MENU_H
