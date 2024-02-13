#ifndef DRAWING_FUNCS_H
#define DRAWING_FUNCS_H

#include <stdint.h>
#include "image.h"

struct Rect {
  int32_t x, y, width, height;
};

void draw_pixel(struct Image *img, int32_t x, int32_t y, uint32_t color);

void draw_rect(struct Image *img,
               const struct Rect *rect,
               uint32_t color);

void draw_circle(struct Image *img,
                 int32_t x, int32_t y, int32_t r,
                 uint32_t color);

void draw_tile(struct Image *img,
               int32_t x, int32_t y,
               struct Image *tilemap,
               const struct Rect *tile);

void draw_sprite(struct Image *img,
                 int32_t x, int32_t y,
                 struct Image *spritemap,
                 const struct Rect *sprite);

//checks to see if the provided x and y coordinates are in bounds of the image
int32_t in_bounds(struct Image *img, int32_t x, int32_t y); 

//Computes the index of the data array given an x and y value
uint32_t compute_index(struct Image *img, int32_t x, int32_t y);

//Restricts a value between min and max
int32_t clamp(int32_t val, int32_t min, int32_t max);

//Checks to see if a (j,i) coordinate point is inside the circle, returns 1 if it is in the circle, -1 otherwise
int32_t in_circle(int32_t x_center, int32_t y_center, int32_t r, int32_t j, int32_t i);

//Get R component of RGB
uint8_t get_r(uint32_t color);

//Get G component of RGB
uint8_t get_g(uint32_t color);

//Get B component of RGB
uint8_t get_b(uint32_t color);

//Get A component of RGB
uint8_t get_a(uint32_t color);

//Calculates the value of blended individual components (RGB)
uint8_t blend_components(uint32_t fg, uint32_t bg, uint32_t alpha);

//Helper function to blend the colors
uint32_t blend_colors(uint32_t fg, uint32_t bg);

//Helper function to set a pixel to a color (without blending)
void set_pixel(struct Image *img, uint32_t index, uint32_t color);

//Helper function that returns the square of x
int64_t square(int64_t x);

//Helper function that calculates the square dist of 2 points (x1, y1) and (x2, y2)
int64_t square_dist(int64_t x1, int64_t y1, int64_t x2, int64_t y2);

#endif // DRAWING_FUNCS_H
