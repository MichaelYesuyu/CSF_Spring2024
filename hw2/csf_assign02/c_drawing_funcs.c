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

//check if a point is within the range. if not, return -1
int32_t in_bounds(struct Image *img, int32_t x, int32_t y){
    int32_t index;
    if((x >= (img->width))||(y >= (img->height))||(x < 0)||(y < 0)){
        return -1;
    }else{
        return compute_index(img, x, y);
    }
}

//compute the index at a given point, given that the point is within the range
uint32_t compute_index(struct Image *img, int32_t x, int32_t y){
    uint32_t new_x = clamp(x, 0, (img->width - 1));
    uint32_t new_y = clamp(y, 0, (img->height - 1));
    uint32_t index = new_x + new_y*(img->width);
    return index;
}

//restrains the value into a given range
int32_t clamp(int32_t val, int32_t min, int32_t max){
    int32_t result = val;
    if(result < min){
        result = min;
    }else if(result > max){
        result = max;
    }
    return result;
}

uint8_t get_r(uint32_t color){
  //Shift color right by 24 bits so the lower 8 bits stores bits for blue
  color = color >> 24;
  //Mask the lower 8 bits
  color = color & ~(~0U << 8);
  //Bitwise OR to store the lower 8 bits in a
  uint8_t r = 0;
  r |= color;
  return r;
}

uint8_t get_g(uint32_t color){
  //Shift color right by 16 bits so the lower 8 bits stores bits for blue
  color = color >> 16;
  //Mask the lower 8 bits
  color = color & ~(~0U << 8);
  //Bitwise OR to store the lower 8 bits in a
  uint8_t g = 0;
  g |= color;
  return g;
}

uint8_t get_b(uint32_t color){
  //Shift color right by 8 bits so the lower 8 bits stores bits for blue
  color = color >> 8;
  //Mask the lower 8 bits
  color = color & ~(~0U << 8);
  //Bitwise OR to store the lower 8 bits in a
  uint8_t b = 0;
  b |= color;
  return b;
}

uint8_t get_a(uint32_t color){
  //Mask the lower 8 bits
  color = color & ~(~0U << 8);
  //Bitwise OR to store the lower 8 bits in a
  uint8_t a = 0;
  a |= color;
  return a;
}

uint8_t blend_components(uint8_t fg, uint8_t bg, uint8_t alpha){
  uint8_t blended_component = ((alpha * fg) + (255 - alpha) * (bg)) / 255;
  return blended_component;
}

uint32_t blend_colors(uint32_t fg, uint32_t bg){
  //Get all the color components
  uint8_t fg_r = get_r(fg);
  uint8_t fg_g = get_g(fg);
  uint8_t fg_b = get_b(fg);
  uint8_t alpha = get_a(fg);
  uint8_t bg_r = get_r(bg);
  uint8_t bg_g = get_g(bg);
  uint8_t bg_b = get_b(bg);

  //Calculate the new color components
  uint8_t new_r = blend_components(fg_r, bg_r, alpha);
  uint8_t new_g = blend_components(fg_b, bg_b, alpha);
  uint8_t new_b = blend_components(fg_b, bg_b, alpha);
  uint8_t new_alpha = 255; //alpha for destination image is always 255

  //Create the new color with the new components
  uint32_t new_color = 0;
  //Bitwise OR and shift to store all the components in specified format
  new_color |= new_r;
  new_color = new_color << 8;
  new_color |= new_g;
  new_color = new_color << 8;
  new_color |= new_b;
  new_color = new_color << 8;
  new_color |= new_alpha;

  return new_color;
}

void set_pixel(struct Image *img, uint32_t index, uint32_t color){
    
}

//return the square of an int64_t
int64_t square(int64_t x){
    return (x*x);
}

//return the square distance of two points
int64_t square_dist(int64_t x1, int64_t y1, int64_t x2, int64_t y2){
    int64_t x_dist = x1 - x2;
    int64_t y_dist = y1 - y2;
    return (square(x_dist) + square(y_dist));
}