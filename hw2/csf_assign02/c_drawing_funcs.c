// C implementations of drawing functions (and helper functions)

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "drawing_funcs.h"

////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////

// TODO: implement helper functions

////////////////////////////////////////////////////////////////////////
// API functions
////////////////////////////////////////////////////////////////////////

//
// Draw a pixel.
//
// Parameters:
//   img   - pointer to struct Image
//   x     - x coordinate (pixel column)
//   y     - y coordinate (pixel row)
//   color - uint32_t color value
//
void draw_pixel(struct Image *img, int32_t x, int32_t y, uint32_t color) {
  // TODO: implement
  
}

//
// Draw a rectangle.
// The rectangle has rect->x,rect->y as its upper left corner,
// is rect->width pixels wide, and rect->height pixels high.
//
// Parameters:
//   img     - pointer to struct Image
//   rect    - pointer to struct Rect
//   color   - uint32_t color value
//
void draw_rect(struct Image *img,
               const struct Rect *rect,
               uint32_t color) {
  // TODO: implement
}

//
// Draw a circle.
// The circle has x,y as its center and has r as its radius.
//
// Parameters:
//   img     - pointer to struct Image
//   x       - x coordinate of circle's center
//   y       - y coordinate of circle's center
//   r       - radius of circle
//   color   - uint32_t color value
//
void draw_circle(struct Image *img,
                 int32_t x, int32_t y, int32_t r,
                 uint32_t color) {
  // TODO: implement
}

//
// Draw a tile by copying all pixels in the region
// enclosed by the tile parameter in the tilemap image
// to the specified x/y coordinates of the destination image.
// No blending of the tile pixel colors with the background
// colors should be done.
//
// Parameters:
//   img     - pointer to Image (dest image)
//   x       - x coordinate of location where tile should be copied
//   y       - y coordinate of location where tile should be copied
//   tilemap - pointer to Image (the tilemap)
//   tile    - pointer to Rect (the tile)
//
void draw_tile(struct Image *img,
               int32_t x, int32_t y,
               struct Image *tilemap,
               const struct Rect *tile) {
 // TODO: implement
}

//
// Draw a sprite by copying all pixels in the region
// enclosed by the sprite parameter in the spritemap image
// to the specified x/y coordinates of the destination image.
// The alpha values of the sprite pixels should be used to
// blend the sprite pixel colors with the background
// pixel colors.
//
// Parameters:
//   img       - pointer to Image (dest image)
//   x         - x coordinate of location where sprite should be copied
//   y         - y coordinate of location where sprite should be copied
//   spritemap - pointer to Image (the spritemap)
//   sprite    - pointer to Rect (the sprite)
//
void draw_sprite(struct Image *img,
                 int32_t x, int32_t y,
                 struct Image *spritemap,
                 const struct Rect *sprite) {
  // TODO: implement
}

uint32_t calculate_color(uint32_t background_color, uint32_t foreground_color){
  //Right shift 8 bits, get the new bottom 8 bits - blue component value
  background_color = background_color >> 8;
  uint32_t background_blue = background_color & ~(~0U << 8);
  //Right shift 8 bits, get the new bottom 8 bits - green component value
  background_color = background_color >> 8;
  uint32_t background_green = background_color & ~(~0U << 8);
  //Right shift 8 bits, get the new bottom 8 bits - red component value
  background_color = background_color >> 8;
  uint32_t background_red = background_color & ~(~0U << 8);

  //get bottom 8 bits of foreground_color - alpha value
  uint32_t new_alpha = foreground_color & ~(~0U << 8);
  //Right shift 8 bits, get the new bottom 8 bits - blue component value
  foreground_color = foreground_color >> 8;
  uint32_t new_blue = foreground_color & ~(~0U << 8);
  //Right shift 8 bits, get the new bottom 8 bits - green component value
  foreground_color = foreground_color >> 8;
  uint32_t new_green = foreground_color & ~(~0U << 8);
  //Right shift 8 bits, get the new bottom 8 bits - red component value
  foreground_color = foreground_color >> 8;
  uint32_t new_red = foreground_color & ~(~0U << 8);

  //Calculate color blend
  uint32_t blended_alpha = 255; //Alpha value of destination image is always 255
  uint32_t blended_blue = ((new_alpha * new_blue) + (255 - new_alpha) * (background_blue)) / 255;
  uint32_t blended_green = ((new_alpha * new_green) + (255 - new_alpha) * (background_green)) / 255;
  uint32_t blended_red = ((new_alpha * new_red) + (255 - new_alpha) * (background_red)) / 255;
  //Initialize the blended color
  uint32_t blended_color = 0;
  //Perform shifting so that the colors are stored in the specified bit format
  //Bitwise or used to change just the lower 8 bits (since all the values only have bits set to 1 in the lower 8 bits)
  blended_color |= blended_alpha; 
  blended_color = blended_color << 8;
  blended_color |= blended_blue;
  blended_color = blended_color << 8;
  blended_color |= blended_green;
  blended_color = blended_color << 8;
  blended_color |= blended_red;

  return blended_color;
}