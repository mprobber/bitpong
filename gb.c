#include <gb/gb.h>
#include "./bitcoin_sprite.h"
#include "./paddle.h"
#include "./numbers.h"

#define GRAPHICS_WIDTH 160
#define GRAPHICS_HEIGHT 144
#define PADDLE_WIDTH 16
#define PADDLE_HEIGHT 32
#define BALL_HEIGHT 32

typedef enum {false, true} bool;

typedef enum {
  UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
} Direction;

typedef struct {
  int score_1;
  int score_2;
  int player_1;
  int player_2;
  int ball_x;
  int ball_y;
  Direction direction;
} Game;


void draw_score(Game * game) {
  int number_tile_1 = game->score_1 != 0 ? 26 + game->score_1 : 35;
  char tile_map_1[] = { number_tile_1 };
  int number_tile_2 = game->score_2 != 0 ? 26 + game->score_2 : 35;
  char tile_map_2[] = { number_tile_2 };

  set_bkg_data(26, 10, Numbers);
  set_bkg_tiles(4, 1, 1, 1, tile_map_1);
  set_bkg_tiles(16, 1, 1, 1, tile_map_2);
}

void position_ball_sprites(int x, int y) {
  move_sprite(0, x, y);
  move_sprite(1, x+8, y);
  move_sprite(2, x, y+16);
  move_sprite(3, x+8, y+16);
  move_sprite(4, x+16, y);
  move_sprite(5, x+24, y);
  move_sprite(6, x+16, y+16);
  move_sprite(7, x+24, y+16);
}

void position_left_paddle(int y) {
  move_sprite(8, 8, y+16);
  move_sprite(9, 8, y+32);
}

void position_right_paddle(int y) {
  move_sprite(10, 160, y+16);
  move_sprite(11, 160, y+32);
}

void reset_ball(Game * game) {
  game->ball_x = GRAPHICS_WIDTH / 2;
  game->ball_y = GRAPHICS_HEIGHT / 2;
  game->direction = DOWN_RIGHT;

  position_ball_sprites(game->ball_x - 8, game->ball_y);

}

void init_game(Game * game) {
  reset_ball(game);
  game->player_1 = (GRAPHICS_HEIGHT / 2)- (PADDLE_HEIGHT / 2);
  game->player_2 = (GRAPHICS_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
  game->score_1 = 0;
  game->score_2 = 0;

  position_left_paddle(game->player_1);
  position_right_paddle(game->player_2);
}

void move_player(Game * game) {
  int position = game->player_1;
  switch (joypad()) {
    case J_UP:
      if (position >= 0) position--;
      break;
    case J_DOWN:
      if (position <= GRAPHICS_HEIGHT - PADDLE_HEIGHT) position++;
      break;
  }
  position_left_paddle(position);
  game->player_1 = position;
}

void move_computer(Game * game) {
  int ball_position = game->ball_y;
  int player_position = game->player_2;
  bool move_up = player_position + 10 > ball_position;
  bool move_down = !move_up;

  if (move_up && player_position > 0) 
    player_position--;
  
  if (move_down && player_position < GRAPHICS_HEIGHT - PADDLE_HEIGHT)
    player_position++;

  position_right_paddle(game->player_2);
  game->player_2 = player_position;
}

void check_for_score(Game * game) {
  Direction direction = game->direction;
  int x = game->ball_x;
  int y = game->ball_y;
  int player_1 = game->player_1;
  int player_2 = game->player_2;


  if (x > GRAPHICS_WIDTH - (BALL_HEIGHT / 2) - PADDLE_WIDTH) {
    if (y > player_2 && y < player_2 + 32) {
      game->direction = direction == DOWN_RIGHT ? DOWN_LEFT : UP_LEFT;
      return;
    }
    game->score_1++;
    reset_ball(game);
    direction = DOWN_LEFT;
    draw_score(game);
  }

  if (x < (BALL_HEIGHT / 2)) {
    if (y > player_1 && y < player_1 + 32) {
      game->direction = direction == DOWN_LEFT ? DOWN_RIGHT : UP_RIGHT;
      return;
    }
    game->score_2++;
    reset_ball(game);
    direction = UP_RIGHT;
    draw_score(game);
  }
}

void move_ball(Game * game) {
  Direction direction = game->direction;
  int x = game->ball_x;
  int y = game->ball_y;

  if (y >= GRAPHICS_HEIGHT - (BALL_HEIGHT / 2)) {
    direction = direction == DOWN_RIGHT ? UP_RIGHT : UP_LEFT;
  }

  if (y <= BALL_HEIGHT / 2) {
    direction = direction == UP_RIGHT ? DOWN_RIGHT : DOWN_LEFT;
  }

  switch (direction) {
    case UP_LEFT:
      y--; x--;
      break;
    case UP_RIGHT:
      y--; x++;
      break;
    case DOWN_LEFT:
      y++; x--;
      break;
    case DOWN_RIGHT:
      y++; x++;
  }

  game->direction = direction;
  game->ball_x = x;
  game->ball_y = y;
  position_ball_sprites(x, y);
}

void setup_sprites() {
  int i;
  SPRITES_8x16;
  set_sprite_data(0, 16, BitcoinSprite);
  set_sprite_data(16, 2, Paddle);

  for (i = 0; i < 8; i++) {
    set_sprite_tile(i, i*2);
  }

  set_sprite_tile(8, 16);
  set_sprite_tile(9, 16);
  set_sprite_tile(10, 16);
  set_sprite_tile(11, 16);

  SHOW_SPRITES;
  SHOW_BKG;
  DISPLAY_ON;
}

void main() {
    Game game;
    setup_sprites();

    init_game(&game);
    draw_score(&game);

    while (true) {
      move_player(&game);
      move_computer(&game);
      move_ball(&game);
      check_for_score(&game);
      delay(15);
    }
}
