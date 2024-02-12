#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "drawing_funcs.h"
#include "tctest.h"
// TODO: add prototypes for your helper functions

// an expected color identified by a (non-zero) character code
typedef struct
{
  char c;
  uint32_t color;
} ExpectedColor;

// struct representing a "picture" of an expected image
typedef struct
{
  ExpectedColor colors[20];
  const char *pic;
} Picture;

typedef struct
{
  struct Image small;
  struct Image large;
  struct Image tilemap;
  struct Image spritemap;
} TestObjs;

// dimensions and pixel index computation for "small" test image (objs->small)
#define SMALL_W 8
#define SMALL_H 6
#define SMALL_IDX(x, y) ((y) * SMALL_W + (x))

// dimensions of the "large" test image
#define LARGE_W 24
#define LARGE_H 20

// create test fixture data
TestObjs *setup(void)
{
  TestObjs *objs = (TestObjs *)malloc(sizeof(TestObjs));
  init_image(&objs->small, SMALL_W, SMALL_H);
  init_image(&objs->large, LARGE_W, LARGE_H);
  objs->tilemap.data = NULL;
  objs->spritemap.data = NULL;
  return objs;
}

// clean up test fixture data
void cleanup(TestObjs *objs)
{
  free(objs->small.data);
  free(objs->large.data);
  free(objs->tilemap.data);
  free(objs->spritemap.data);

  free(objs);
}

uint32_t lookup_color(char c, const ExpectedColor *colors)
{
  for (unsigned i = 0;; i++)
  {
    assert(colors[i].c != 0);
    if (colors[i].c == c)
    {
      return colors[i].color;
    }
  }
}

void check_picture(struct Image *img, Picture *p)
{
  unsigned num_pixels = img->width * img->height;
  assert(strlen(p->pic) == num_pixels);

  for (unsigned i = 0; i < num_pixels; i++)
  {
    char c = p->pic[i];
    uint32_t expected_color = lookup_color(c, p->colors);
    uint32_t actual_color = img->data[i];
    ASSERT(actual_color == expected_color);
  }
}

// prototypes of test functions
void test_draw_pixel(TestObjs *objs);
void test_draw_rect(TestObjs *objs);
void test_draw_circle(TestObjs *objs);
void test_draw_circle_clip(TestObjs *objs);
void test_draw_tile(TestObjs *objs);
void test_draw_sprite(TestObjs *objs);
void test_get_color_components(TestObjs *objs);
void test_color_blending(TestObjs *objs);
void test_square(TestObjs *objs);
void test_square_distance(TestObjs *objs);
void test_compute_index(TestObjs *objs);
void test_in_bound(TestObjs *objs);
void test_in_circle(TestObjs *objs);
void test_get_pixel(TestObjs *objs);
void test_set_pixel(TestObjs *objs);
void test_get_r(TestObjs *objs);
void test_get_g(TestObjs *objs);
void test_get_b(TestObjs *objs);
void test_get_a(TestObjs *objs);
void test_blend_components(TestObjs *objs);

int main(int argc, char **argv)
{
  if (argc > 1)
  {
    // user specified a specific test function to run
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  // TODO: add TEST() directives for your helper functions
  TEST(test_draw_pixel);
  TEST(test_draw_rect);
  TEST(test_draw_circle);
  // TEST(test_draw_circle_clip);
  TEST(test_draw_tile);
  // TEST(test_draw_sprite);
  TEST(test_get_color_components);
  TEST(test_color_blending);
  TEST(test_square);
  TEST(test_square_distance);
  TEST(test_compute_index);
  TEST(test_in_bound);
  TEST(test_in_circle);
  TEST(test_get_pixel);
  TEST(test_set_pixel);
  TEST(test_get_r);
  TEST(test_get_g);
  TEST(test_get_b);
  TEST(test_get_a);
  TEST(test_blend_components);

  TEST_FINI();
}

void test_draw_pixel(TestObjs *objs)
{
  // initially objs->small pixels are opaque black
  ASSERT(objs->small.data[SMALL_IDX(3, 2)] == 0x000000FFU);
  ASSERT(objs->small.data[SMALL_IDX(5, 4)] == 0x000000FFU);

  // test drawing completely opaque pixels
  draw_pixel(&objs->small, 3, 2, 0xFF0000FF); // opaque red
  ASSERT(objs->small.data[SMALL_IDX(3, 2)] == 0xFF0000FF);
  draw_pixel(&objs->small, 5, 4, 0x800080FF); // opaque magenta (half-intensity)
  ASSERT(objs->small.data[SMALL_IDX(5, 4)] == 0x800080FF);

  // test color blending
  draw_pixel(&objs->small, 3, 2, 0x00FF0080); // half-opaque full-intensity green
  ASSERT(objs->small.data[SMALL_IDX(3, 2)] == 0x7F8000FF);
  draw_pixel(&objs->small, 4, 2, 0x0000FF40); // 1/4-opaque full-intensity blue
  ASSERT(objs->small.data[SMALL_IDX(4, 2)] == 0x000040FF);
}

void test_draw_rect(TestObjs *objs)
{
  struct Rect red_rect = {.x = 2, .y = 2, .width = 3, .height = 3};
  struct Rect blue_rect = {.x = 3, .y = 3, .width = 3, .height = 3};
  draw_rect(&objs->small, &red_rect, 0xFF0000FF);  // red is full-intensity, full opacity
  draw_rect(&objs->small, &blue_rect, 0x0000FF80); // blue is full-intensity, half opacity

  const uint32_t red = 0xFF0000FF;   // expected full red color
  const uint32_t blue = 0x000080FF;  // expected full blue color
  const uint32_t blend = 0x7F0080FF; // expected red/blue blend color
  const uint32_t black = 0x000000FF; // expected black (background) color

  Picture expected = {
      {{'r', red}, {'b', blue}, {'n', blend}, {' ', black}},
      "        "
      "        "
      "  rrr   "
      "  rnnb  "
      "  rnnb  "
      "   bbb  "};

  check_picture(&objs->small, &expected);
}

void test_draw_circle(TestObjs *objs)
{
  Picture expected = {
      {{' ', 0x000000FF}, {'x', 0x00FF00FF}},
      "   x    "
      "  xxx   "
      " xxxxx  "
      "  xxx   "
      "   x    "
      "        "};

  draw_circle(&objs->small, 3, 2, 2, 0x00FF00FF);

  check_picture(&objs->small, &expected);
}

void test_draw_circle_clip(TestObjs *objs)
{
  Picture expected = {
      {{' ', 0x000000FF}, {'x', 0x00FF00FF}},
      " xxxxxxx"
      " xxxxxxx"
      "xxxxxxxx"
      " xxxxxxx"
      " xxxxxxx"
      "  xxxxx "};

  draw_circle(&objs->small, 4, 2, 4, 0x00FF00FF);

  check_picture(&objs->small, &expected);
}

void test_draw_tile(TestObjs *objs)
{
  ASSERT(read_image("img/PrtMimi.png", &objs->tilemap) == IMG_SUCCESS);

  struct Rect r = {.x = 4, .y = 2, .width = 16, .height = 18};
  draw_rect(&objs->large, &r, 0x1020D0FF);

  struct Rect grass = {.x = 0, .y = 16, .width = 16, .height = 16};
  draw_tile(&objs->large, 3, 2, &objs->tilemap, &grass);

  Picture pic = {
      {
          {' ', 0x000000ff},
          {'a', 0x52ad52ff},
          {'b', 0x1020d0ff},
          {'c', 0x257b4aff},
          {'d', 0x0c523aff},
      },
      "                        "
      "                        "
      "             a     b    "
      "            a      b    "
      "            a     ab    "
      "           ac      b    "
      "           ac a    b    "
      "      a a  ad  a   b    "
      "     a  a aad  aa ab    "
      "     a  a acd aaacab    "
      "    aa  cdacdaddaadb    "
      "     aa cdaddaaddadb    "
      "     da ccaddcaddadb    "
      "    adcaacdaddddcadb    "
      "   aaccacadcaddccaab    "
      "   aacdacddcaadcaaab    "
      "   aaaddddaddaccaacb    "
      "   aaacddcaadacaaadb    "
      "    bbbbbbbbbbbbbbbb    "
      "    bbbbbbbbbbbbbbbb    "};

  check_picture(&objs->large, &pic);
}

void test_draw_sprite(TestObjs *objs)
{
  ASSERT(read_image("img/NpcGuest.png", &objs->spritemap) == IMG_SUCCESS);

  struct Rect r = {.x = 1, .y = 1, .width = 14, .height = 14};
  draw_rect(&objs->large, &r, 0x800080FF);

  struct Rect sue = {.x = 128, .y = 136, .width = 16, .height = 15};
  draw_sprite(&objs->large, 4, 2, &objs->spritemap, &sue);

  Picture pic = {
      {
          {' ', 0x000000ff},
          {'a', 0x800080ff},
          {'b', 0x9cadc1ff},
          {'c', 0xefeae2ff},
          {'d', 0x100000ff},
          {'e', 0x264c80ff},
          {'f', 0x314e90ff},
      },
      "                        "
      " aaaaaaaaaaaaaa         "
      " aaaaaaaaaaaaaa         "
      " aaaaaaaaaaaaaa         "
      " aaaaaaabccccccbc       "
      " aaaaacccccccccccc      "
      " aaaacbddcccddcbccc     "
      " aaacbbbeccccedbccc     "
      " aaacbbceccccebbbccc    "
      " aaabbbccccccccbbccc    "
      " aaaabbbcccccccb ccb    "
      " aaaabaaaaabbaa  cb     "
      " aaaaaaaaafffea         "
      " aaaaaaaaaffeea         "
      " aaaaaaacffffcc         "
      "        cffffccb        "
      "         bbbbbbb        "
      "                        "
      "                        "
      "                        "};

  check_picture(&objs->large, &pic);
}

void test_get_color_components(TestObjs *objs)
{
  // This number represents R = 255, G = 255, B = 255, alpha = 255
  uint32_t test_color_1 = UINT32_MAX;
  // This number represents R = 0, G = 0, B = 0, alpha = 0
  uint32_t test_color_2 = 0;
  // This number represents R = 120, G = 120, B = 120, alpha = 255
  uint32_t test_color_3 = 2021161215;

  ASSERT(get_a(test_color_1) == 255);
  ASSERT(get_r(test_color_1) == 255);
  ASSERT(get_g(test_color_1) == 255);
  ASSERT(get_b(test_color_1) == 255);
  ASSERT(get_a(test_color_2) == 0);
  ASSERT(get_r(test_color_2) == 0);
  ASSERT(get_g(test_color_2) == 0);
  ASSERT(get_b(test_color_2) == 0);
  ASSERT(get_b(test_color_3) == 120);
  ASSERT(get_r(test_color_3) == 120);
  ASSERT(get_g(test_color_3) == 120);
  ASSERT(get_a(test_color_3) == 255);
}

void test_color_blending(TestObjs *objs)
{
  // Background
  uint8_t r_1 = 255;
  uint8_t g_1 = 255;
  uint8_t b_1 = 255;
  uint8_t alpha_1 = 255; // alpha doesn't matter but need to be stored anyway
  uint32_t bg = 0;
  bg |= r_1;
  bg = bg << 8;
  bg |= g_1;
  bg = bg << 8;
  bg |= b_1;
  bg = bg << 8;
  bg |= alpha_1;

  // Foreground
  uint8_t r_2 = 120;
  uint8_t g_2 = 120;
  uint8_t b_2 = 120;
  uint8_t alpha_2 = 255; // Should completely replace background
  uint32_t fg = 0;
  fg |= r_2;
  fg = fg << 8;
  fg |= g_2;
  fg = fg << 8;
  fg |= b_2;
  fg = fg << 8;
  fg |= alpha_2;

  uint32_t new_color = blend_colors(fg, bg);
  ASSERT(get_a(new_color) == 255);
  ASSERT(get_r(new_color) == 120);
  ASSERT(get_g(new_color) == 120);
  ASSERT(get_b(new_color) == 120);

  // Foreground 2
  uint8_t r_3 = 120;
  uint8_t g_3 = 120;
  uint8_t b_3 = 120;
  uint8_t alpha_3 = 200;
  uint32_t fg_2 = 0;
  fg_2 |= r_3;
  fg_2 = fg_2 << 8;
  fg_2 |= g_3;
  fg_2 = fg_2 << 8;
  fg_2 |= b_3;
  fg_2 = fg_2 << 8;
  fg_2 |= alpha_3;

  uint32_t new_color_2 = blend_colors(fg_2, bg);
  ASSERT(get_a(new_color_2) == 255);
  ASSERT(get_r(new_color_2) == 149);
  ASSERT(get_g(new_color_2) == 149);
  ASSERT(get_b(new_color_2) == 149);
}

void test_square(TestObjs *objs)
{
  ASSERT(0L == square(0L));
  ASSERT(4L == square(2L));
  ASSERT(9L == square(3L));
  ASSERT(4L == square(-2L));
  ASSERT(10000L == square(100L));
  ASSERT(10000L == square(-100L));
}

void test_square_distance(TestObjs *objs)
{
  ASSERT(0L == square_dist(0L, 0L, 0L, 0L));
  ASSERT(4L == square_dist(0L, 0L, 2L, 0L));
  ASSERT(13L == square_dist(0L, 0L, 2L, 3L));
  ASSERT(4L == square_dist(0L, 0L, -2L, 0L));
  ASSERT(10000L == square_dist(0L, 0L, 100L, 0L));
  ASSERT(10000L == square_dist(0L, 0L, -100L, 0L));
}

void test_compute_index(TestObjs *objs)
{
  int index = compute_index(&objs->small, 2, 2);
  ASSERT(18 == index);
  index = compute_index(&objs->small, 0, 0);
  ASSERT(0 == index);
  index = compute_index(&objs->small, 5, 0);
  ASSERT(5 == index);
  index = compute_index(&objs->small, 0, 5);
  ASSERT(40 == index);
}

void test_get_pixel(TestObjs *objs)
{
  uint32_t pixel = get_pixel(&objs->small, 5);
  ASSERT(pixel == 0x000000FFU);
  pixel = get_pixel(&objs->small, 20);
  ASSERT(pixel == 0x000000FFU);
  pixel = get_pixel(&objs->small, 0);
  ASSERT(pixel == 0x000000FFU);
  pixel = get_pixel(&objs->small, 7);
  ASSERT(pixel == 0x000000FFU);
  pixel = get_pixel(&objs->small, 20);
  ASSERT(pixel == 0x000000FFU);
}

void test_in_bound(TestObjs *objs)
{
  int inBoundCheck = in_bounds(&objs->small, 2, 2);
  ASSERT(inBoundCheck == 1);
  inBoundCheck = in_bounds(&objs->small, 8, 6);
  ASSERT(inBoundCheck == -1);
  inBoundCheck = in_bounds(&objs->small, 10, 10);
  ASSERT(inBoundCheck == -1);
  inBoundCheck = in_bounds(&objs->small, 0, 0);
  ASSERT(inBoundCheck == 1);
}

void test_set_pixel(TestObjs *objs)
{
  set_pixel(&objs->small, 5, 0x9cadc1ff);
  uint32_t pixel = get_pixel(&objs->small, 5);
  ASSERT(pixel == 0x9cadc1ff);
  set_pixel(&objs->small, 7, 0x100000ff);
  pixel = get_pixel(&objs->small, 7);
  ASSERT(pixel == 0x100000ff);
  set_pixel(&objs->small, 0, 0x264c80ff);
  pixel = get_pixel(&objs->small, 0);
  ASSERT(pixel == 0x264c80ff);

}

void test_in_circle(TestObjs *objs)
{
  int inCircleCheck = in_circle(0, 0, 5, 1, 2);
  ASSERT(inCircleCheck == 1);
  inCircleCheck = in_circle(1, 0, 5, 1, 5);
  ASSERT(inCircleCheck == 1);
  inCircleCheck = in_circle(1, 0, 5, 1, 6);
  ASSERT(inCircleCheck == -1);
  inCircleCheck = in_circle(1, 0, 3, 1, 6);
  ASSERT(inCircleCheck == -1);
}


void test_get_r(TestObjs *objs)
{
  uint8_t expected1 = 255;
  uint32_t input1 = UINT32_MAX;
  ASSERT(get_r(input1) == expected1);
  uint8_t expected2 = 0;
  uint32_t input2 = UINT32_MAX >> 8;
  ASSERT(get_r(input2) == expected2);
  uint8_t expected3 = 127;
  uint32_t input3 = UINT32_MAX >> 1;
  ASSERT(get_r(input3) == expected3);
}

void test_get_g(TestObjs *objs)
{
  uint8_t expected1 = 255;
  uint32_t input1 = UINT32_MAX;
  ASSERT(get_g(input1) == expected1);
  uint8_t expected2 = 0;
  uint32_t input2 = UINT32_MAX >> 16;
  ASSERT(get_g(input2) == expected2);
  uint8_t expected3 = 127;
  uint32_t input3 = UINT32_MAX >> 9;
  ASSERT(get_g(input3) == expected3);
}

void test_get_b(TestObjs *objs)
{
  uint8_t expected1 = 255;
  uint32_t input1 = UINT32_MAX;
  ASSERT(get_b(input1) == expected1);
  uint8_t expected2 = 0;
  uint32_t input2 = UINT32_MAX >> 24;
  ASSERT(get_b(input2) == expected2);
  uint8_t expected3 = 127;
  uint32_t input3 = UINT32_MAX >> 17;
  ASSERT(get_b(input3) == expected3);
}

void test_get_a(TestObjs *objs)
{
  uint8_t expected1 = 255;
  uint32_t input1 = UINT32_MAX;
  ASSERT(get_a(input1) == expected1);
  uint8_t expected2 = 1;
  uint32_t input2 = UINT32_MAX >> 31;
  ASSERT(get_a(input2) == expected2);
  uint8_t expected3 = 127;
  uint32_t input3 = UINT32_MAX >> 25;
  ASSERT(get_a(input3) == expected3);
}

void test_blend_components(TestObjs *objs)
{
  uint32_t fg_component_1 = 255;
  uint32_t bg_component_1 = 255;
  uint32_t alpha_1 = 255;
  ASSERT(blend_components(fg_component_1, bg_component_1, alpha_1) == 255);

  uint32_t fg_component_2 = 120;
  uint32_t bg_component_2 = 255;
  uint32_t alpha_2 = 200;
  ASSERT(blend_components(fg_component_2, bg_component_2, alpha_2) == 149);
}
