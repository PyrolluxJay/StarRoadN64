// metal_box.inc.c
#include "game/options_menu.h"

struct ObjectHitbox sMetalBoxHitbox = {
    /* interactType:      */ INTERACT_NONE,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 1,
    /* numLootCoins:      */ 0,
    /* radius:            */ 220,
    /* height:            */ 300,
    /* hurtboxRadius:     */ 220,
    /* hurtboxHeight:     */ 300,
};

s32 check_if_moving_over_floor(f32 maxDist, f32 offset) {
    struct Surface *floor;
    f32 xPos = o->oPosX + sins(o->oMoveAngleYaw) * offset;
    f32 zPos = o->oPosZ + coss(o->oMoveAngleYaw) * offset;

    f32 floorHeight = find_floor(xPos, o->oPosY, zPos, &floor);

    return (absf(floorHeight - o->oPosY) < maxDist);
}

void bhv_pushable_loop(void) {
    obj_set_hitbox(o, &sMetalBoxHitbox);
    o->oForwardVel = 0.0f;

    if (obj_check_if_collided_with_object(o, gMarioObject) && gMarioStates[0].flags & MARIO_PUSHING) {
        s16 angleToMario = obj_angle_to_object(o, gMarioObject);
        if (abs_angle_diff(angleToMario, gMarioObject->oMoveAngleYaw) > 0x4000) {
            o->oMoveAngleYaw = (s16)((gMarioObject->oMoveAngleYaw + 0x2000) & 0xc000);
            const f32 vel = configFasterObjects ? 10.f : 4.f;
            if (check_if_moving_over_floor(vel + 4.0f, 150.0f)) {
                o->oForwardVel = vel;
                if (configFasterObjects)
                {
                    gMarioStates->pos[0] += o->oForwardVel * sins(o->oMoveAngleYaw);
                    gMarioStates->pos[2] += o->oForwardVel * coss(o->oMoveAngleYaw);
                }
                cur_obj_play_sound_1(SOUND_ENV_METAL_BOX_PUSH);
            }
        }
    }

    cur_obj_move_using_fvel_and_gravity();
}
