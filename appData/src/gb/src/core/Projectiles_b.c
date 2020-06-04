// clang-format off
#pragma bank=1
// clang-format on

#include "Projectiles.h"

#include "GameTime.h"
#include "Scroll.h"
#include "Sprite.h"

#define SCREENWIDTH_PLUS_64 224   // 160 + 64
#define SCREENHEIGHT_PLUS_64 208  // 144 + 64

Projectile projectiles[MAX_PROJECTILES];
UBYTE current_projectile = 0;

void ProjectilesInit_b() {
  UBYTE i;
  for (i = 0; i != MAX_PROJECTILES; i++) {
    projectiles[i].pos.x = 32 + (32 * i);
    projectiles[i].pos.y = 32 + (32 * i);
    projectiles[i].sprite_index = SpritePoolNext();
    LOG("MADE PROJECTILE %u WITH sprite_index=%u\n", i, projectiles[i].sprite_index);
  }
}

void ProjectileLaunch_b(UBYTE sprite, WORD x, WORD y, BYTE dir_x, BYTE dir_y, UBYTE moving,
                        UBYTE move_speed, UBYTE life_time, UBYTE col_mask, UWORD col_script) {

  if (projectiles[current_projectile].life_time == 0) {
    projectiles[current_projectile].life_time = 0;
    projectiles[current_projectile].moving = 0;
    projectiles[current_projectile].pos.x = 0;
    projectiles[current_projectile].pos.y = 0;
    projectiles[current_projectile].dir.x = 0;
    projectiles[current_projectile].dir.y = 0;
    projectiles[current_projectile].move_speed = 0;
    projectiles[current_projectile].life_time = 0;
    projectiles[current_projectile].col_mask = 0;

    projectiles[current_projectile].moving = moving;
    projectiles[current_projectile].pos.x = x;
    projectiles[current_projectile].pos.y = y;
    projectiles[current_projectile].dir.x = dir_x;
    projectiles[current_projectile].dir.y = dir_y;
    projectiles[current_projectile].move_speed = move_speed;
    projectiles[current_projectile].life_time = life_time;
    projectiles[current_projectile].col_mask = col_mask;
  }

  current_projectile = (current_projectile + 1) % MAX_PROJECTILES;
}

void UpdateProjectiles_b() {
  UBYTE i, k;
  UINT16 screen_x;
  UINT16 screen_y;

  for (i = 0; i != MAX_PROJECTILES; i++) {
    if (projectiles[i].life_time != 0) {
      if (projectiles[i].moving) {
        if (projectiles[i].move_speed == 0) {
          // Half speed only move every other frame
          if (IS_FRAME_2) {
            projectiles[i].pos.x += projectiles[i].dir.x;
            projectiles[i].pos.y += projectiles[i].dir.y;
          }
        } else {
          projectiles[i].pos.x += projectiles[i].dir.x * projectiles[i].move_speed;
          projectiles[i].pos.y += projectiles[i].dir.y * projectiles[i].move_speed;
        }
      }

      k = projectiles[i].sprite_index;
      screen_x = 8u + projectiles[i].pos.x - scroll_x;
      screen_y = 8u + projectiles[i].pos.y - scroll_y;

      move_sprite(k, screen_x, screen_y);
      move_sprite(k + 1, screen_x + 8, screen_y);

      // Check if actor is off screen
      if (IS_FRAME_4) {
        if (((UINT16)(screen_x + 32u) >= SCREENWIDTH_PLUS_64) ||
            ((UINT16)(screen_y + 32u) >= SCREENHEIGHT_PLUS_64)) {
          // Mark off screen actor for removal
          LOG("PROJECTILE OFFSCREEN %u p_x=%d s_x=%u s_y=%u s1_x=%u s1_y=%u max_x=%u max_y=%u\n", i,
              projectiles[i].pos.x, screen_x, screen_y, (UINT16)(screen_x + 32u),
              (UINT16)(screen_y + 32u), SCREENWIDTH_PLUS_64, SCREENHEIGHT_PLUS_64);
          projectiles[i].life_time = 0;
        } else {
          projectiles[i].life_time--;
        }
      }

    } else {
      LOG("HIDE PROJECTILE %u\n", i);
      k = projectiles[i].sprite_index;
      move_sprite(k, 0, 0);
      move_sprite(k + 1, 0, 0);
    }
  }
}