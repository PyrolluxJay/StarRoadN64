#include "sm64.h"
#include "gfx_dimensions.h"
#include "engine/math_util.h"
#include "audio/external.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "game/print.h"
#include "game/segment2.h"
#include "game/save_file.h"
#include "game/mario_misc.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/options_menu.h"

u8 optmenu_open = 0;

static u8 optmenu_binding = 0;
static u8 optmenu_bind_idx = 0;

// How to add stuff:
// strings: add them to include/text_strings.h.in
//          and to optMainStr[] / opts*Str[]
// options: add them to the relevant options list
// menus:   add a new submenu definition and a new
//          option to the optsMain list

static const char toggleStr[][16] = {
    { "Disabled" },
    { "Enabled" },
};

static const char optSmallStr[][32] = {
    { "Ⓡ Options" },
    { "Ⓡ Return" },
    { "▶" },
    { "◀" },
};

enum OptType {
    OPT_INVALID = 0,
    OPT_TOGGLE,
    OPT_CHOICE,
    OPT_SCROLL,
    OPT_SUBMENU,
    OPT_BIND,
    OPT_BUTTON,
};

struct SubMenu;

#define bool u8

struct Option {
    enum OptType type;
    const char *label;
    union {
        u32 *uval;
        bool *bval;
    };
    union {
        struct {
            const char **choices;
            u32 numChoices;
        };
        struct {
            u32 scrMin;
            u32 scrMax;
            u32 scrStep;
        };
        struct SubMenu *nextMenu;
        void (*actionFn)(struct Option *, s32);
    };
};

struct SubMenu {
    struct SubMenu *prev; // this is set at runtime to avoid needless complication
    const char *label;
    struct Option *opts;
    s32 numOpts;
    s32 select;
    s32 scroll;
};

/* helper macros */

#define DEF_OPT_TOGGLE(lbl, bv) \
    { .type = OPT_TOGGLE, .label = lbl, .bval = bv }
#define DEF_OPT_SCROLL(lbl, uv, min, max, st) \
    { .type = OPT_SCROLL, .label = lbl, .uval = uv, .scrMin = min, .scrMax = max, .scrStep = st }
#define DEF_OPT_CHOICE(lbl, uv, ch) \
    { .type = OPT_CHOICE, .label = lbl, .uval = uv, .choices = ch, .numChoices = sizeof(ch) / sizeof(ch[0]) }
#define DEF_OPT_SUBMENU(lbl, nm) \
    { .type = OPT_SUBMENU, .label = lbl, .nextMenu = nm }

#define DEF_SUBMENU(lbl, opt) \
    { .label = lbl, .opts = opt, .numOpts = sizeof(opt) / sizeof(opt[0]) }

/* submenu definitions */

#define MAX_VOLUME 127
unsigned int configMasterVolume = MAX_VOLUME; // 0 - MAX_VOLUME
unsigned int configMusicVolume = MAX_VOLUME;
unsigned int configSfxVolume = MAX_VOLUME;
unsigned int configEnvVolume = MAX_VOLUME;

static const char optMainStr[][32] = {
    { "Options" },
    { "Camera" },
    { "Controls" },
    { "Video" },
    { "Audio" },
    { "Exit Game" },
    { "Cheats" },
};

static const char optsAudioStr[][32] = {
    { "mv" },    
    { "mus" },
    { "sfx" },
    { "env" },
};

static struct Option optsAudio[] = {
    DEF_OPT_SCROLL( optsAudioStr[0], &configMasterVolume, 0, MAX_VOLUME, 1 ),
    DEF_OPT_SCROLL( optsAudioStr[1], &configMusicVolume, 0, MAX_VOLUME, 1),
    DEF_OPT_SCROLL( optsAudioStr[2], &configSfxVolume, 0, MAX_VOLUME, 1),
    DEF_OPT_SCROLL( optsAudioStr[3], &configEnvVolume, 0, MAX_VOLUME, 1),
};

static struct SubMenu menuAudio    = DEF_SUBMENU( optMainStr[4], optsAudio );

/* main options menu definition */

static struct Option optsMain[] = {
#ifdef BETTERCAMERA
    DEF_OPT_SUBMENU( optMainStr[1], &menuCamera ),
#endif
#ifndef TARGET_N64
    DEF_OPT_SUBMENU( optMainStr[2], &menuControls ),
    DEF_OPT_SUBMENU( optMainStr[3], &menuVideo ),
    DEF_OPT_BUTTON ( optMainStr[5], optmenu_act_exit ),
#endif

    DEF_OPT_SUBMENU( optMainStr[4], &menuAudio ),

#ifdef CHEATS_ACTIONS
    // NOTE: always keep cheats the last option here because of the half-assed way I toggle them
    DEF_OPT_SUBMENU( optMainStr[6], &menuCheats )
#endif
};

static struct SubMenu menuMain = DEF_SUBMENU( optMainStr[0], optsMain );

/* implementation */

static s32 optmenu_option_timer = 0;
static u8 optmenu_hold_count = 0;
static s32 optmenu_sin_timer = 0;

static struct SubMenu *currentMenu = &menuMain;

static inline s32 wrap_add(s32 a, const s32 b, const s32 min, const s32 max) {
    a += b;
    if (a < min) a = max - (min - a) + 1;
    else if (a > max) a = min + (a - max) - 1;
    return a;
}

static void uint_to_hex(u32 num, char *dst) {
    sprintf(dst, "%04X", num);
}

//Displays a box.
static void optmenu_draw_box(s16 x1, s16 y1, s16 x2, s16 y2, u8 r, u8 g, u8 b) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);
    gDPSetFillColor(gDisplayListHead++, GPACK_RGBA5551(r, g, b, 255));
    gDPFillRectangle(gDisplayListHead++, x1, y1, x2 - 1, y2 - 1);
    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
}

static void optmenu_draw_text(s16 x, s16 y, const char *str, u8 col) {
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    print_generic_string_aligned(x+1, y-1, str, TEXT_ALIGN_CENTER);
    if (col == 0) {
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    } else {
        gDPSetEnvColor(gDisplayListHead++, 255, 32, 32, 255);
    }
    print_generic_string_aligned(x, y, str, TEXT_ALIGN_CENTER);
}

static void optmenu_draw_opt(const struct Option *opt, s16 x, s16 y, u8 sel) {
    char buf[32] = { 0 };

    if (opt->type == OPT_SUBMENU || opt->type == OPT_BUTTON)
        y -= 6;

    optmenu_draw_text(x, y, opt->label, sel);

    switch (opt->type) {
        case OPT_TOGGLE:
            optmenu_draw_text(x, y-13, toggleStr[(int)*opt->bval], sel);
            break;

        case OPT_CHOICE:
            optmenu_draw_text(x, y-13, opt->choices[*opt->uval], sel);
            break;

        case OPT_SCROLL:
            sprintf(buf, "%d", *opt->uval);
            optmenu_draw_text(x, y-13, buf, sel);
            break;

        default: break;
    };
}

static void optmenu_opt_change(struct Option *opt, s32 val) {
    switch (opt->type) {
        case OPT_TOGGLE:
            *opt->bval = !*opt->bval;
            break;

        case OPT_CHOICE:
            *opt->uval = wrap_add(*opt->uval, val, 0, opt->numChoices - 1);
            break;

        case OPT_SCROLL:
            *opt->uval = wrap_add(*opt->uval, opt->scrStep * val, opt->scrMin, opt->scrMax);
            break;

        case OPT_SUBMENU:
            opt->nextMenu->prev = currentMenu;
            currentMenu = opt->nextMenu;
            break;

        case OPT_BUTTON:
            if (opt->actionFn)
                opt->actionFn(opt, val);
            break;

        default: break;
    }
}

static inline s16 get_hudstr_centered_x(const s16 sx, const char *str) {
    const char *chr = str;
    s16 len = 0;
    while (*chr != '\0') ++chr, ++len;
    return sx - len * 6; // stride is 12
}

//Options menu
void optmenu_draw(void) {
    u8 i;
    s16 scroll;
    s16 scrollpos;
    f32 sinpos;

    const s16 labelX = get_hudstr_centered_x(160, currentMenu->label);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    print_hud_lut_string(labelX, 40, currentMenu->label);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    if (currentMenu->numOpts > 4) {
        optmenu_draw_box(272, 90, 280, 208, 0x80, 0x80, 0x80);
        scrollpos = 54 * ((f32)currentMenu->scroll / (currentMenu->numOpts-4));
        optmenu_draw_box(272, 90+scrollpos, 280, 154+scrollpos, 0xFF, 0xFF, 0xFF);
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 80, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    for (i = 0; i < currentMenu->numOpts; i++) {
        scroll = 140 - 32 * i + currentMenu->scroll * 32;
        // FIXME: just start from the first visible option bruh
        if (scroll <= 140 && scroll > 32)
            optmenu_draw_opt(&currentMenu->opts[i], 160, scroll, (currentMenu->select == i));
    }

    sinpos = sins(optmenu_sin_timer*5000)*4;
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    print_generic_string(80 - sinpos, 132 - (32 * (currentMenu->select - currentMenu->scroll)), optSmallStr[2]);
    print_generic_string(224 + sinpos, 132 - (32 * (currentMenu->select - currentMenu->scroll)), optSmallStr[3]);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

//This has been separated for interesting reasons. Don't question it.
void optmenu_draw_prompt(void) {
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    optmenu_draw_text(264, 212, optSmallStr[optmenu_open], 0);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

void optmenu_toggle(void) {
    if (optmenu_open == 0) {
        #ifndef nosound
        play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
        #endif

        currentMenu = &menuMain;
        optmenu_open = 1;
    } else {
        #ifndef nosound
        play_sound(SOUND_MENU_MARIO_CASTLE_WARP, gGlobalSoundSource);
        #endif
        optmenu_open = 0;
#ifndef TARGET_N64
        controller_reconfigure(); // rebind using new config values
        configfile_save(configfile_name());
#endif
    }
}

void optmenu_check_buttons(void) {
    if (gPlayer1Controller->buttonPressed & R_TRIG) {
        optmenu_toggle();
    }
    
    optmenu_sin_timer++;

    if (!optmenu_open) return;

    u8 allowInput = 0;

    optmenu_option_timer--;
    if (optmenu_option_timer <= 0) {
        if (optmenu_hold_count == 0) {
            optmenu_hold_count++;
            optmenu_option_timer = 10;
        } else {
            optmenu_option_timer = 3;
        }
        allowInput = 1;
    }

    if (ABS(gPlayer1Controller->stickY) > 60) {
        if (allowInput) {
            #ifndef nosound
            play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
            #endif

            if (gPlayer1Controller->stickY >= 60) {
                currentMenu->select--;
                if (currentMenu->select < 0)
                    currentMenu->select = currentMenu->numOpts-1;
            } else {
                currentMenu->select++;
                if (currentMenu->select >= currentMenu->numOpts)
                    currentMenu->select = 0;
            }

            if (currentMenu->select < currentMenu->scroll)
                currentMenu->scroll = currentMenu->select;
            else if (currentMenu->select > currentMenu->scroll + 3)
                currentMenu->scroll = currentMenu->select - 3;
        }
    } else if (ABS(gPlayer1Controller->stickX) > 60) {
        if (allowInput) {
            #ifndef nosound
            play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
            #endif
            if (gPlayer1Controller->stickX >= 60)
                optmenu_opt_change(&currentMenu->opts[currentMenu->select], 1);
            else
                optmenu_opt_change(&currentMenu->opts[currentMenu->select], -1);
        }
    } else if (gPlayer1Controller->buttonPressed & A_BUTTON) {
        if (allowInput) {
            #ifndef nosound
            play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
            #endif
            optmenu_opt_change(&currentMenu->opts[currentMenu->select], 0);
        }
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        if (allowInput) {
            if (currentMenu->prev) {
                #ifndef nosound
                play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
                #endif
                currentMenu = currentMenu->prev;
            } else {
                // can't go back, exit the menu altogether
                optmenu_toggle();
            }
        }
    } else if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        if (allowInput) optmenu_toggle();
    } else {
        optmenu_hold_count = 0;
        optmenu_option_timer = 0;
    }
}
