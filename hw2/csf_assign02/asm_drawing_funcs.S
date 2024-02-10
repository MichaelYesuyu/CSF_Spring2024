/*
 * Assembly-language implementations of the drawing functions
 */


/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/* Offsets of struct Rect fields */
#define RECT_X_OFFSET        0
#define RECT_Y_OFFSET        4
#define RECT_WIDTH_OFFSET    8
#define RECT_HEIGHT_OFFSET   12

	.section .text

/***********************************************************************
   Helper functions
   You should declare these as .globl so that
   they can be called from the unit test code.
 ***********************************************************************/

/* TODO: implement your helper functions here */

/***********************************************************************
   Public API functions
 ***********************************************************************/

/*
 * Return the square of an int64 value.
 *
 * Parameters:
 *   %rdi     - the value to square
 * 
 * Returns (in %rax):
 *  the square of the input value
 */
	.globl square
square:
	imulq %rdi, %rdi           /* multiply the value in rdi by itself*/
	movq %rdi, %rax			   /* move the multiplied value to rax*/
	ret

/*
 * Compute the square of the geomoetrical distance between 
 * (x1, y1) and (x2, y2)
 *
 * Parameters:
 *   %rdi     - x1
 *   %rsi     - y1
 *   %rdx     - x2
 *   %rcx     - y2
 * 
 * Returns (in %rax):
 *  the square of the geomoetrical distance between (x1, y1)
 *  and (x2, y2)
 */
	.globl square_dist
square_dist:
	
	/* save the value originally in r12 to the memory */
	pushq %r12

	/* compute (x1 - x2)^2 */
	subq %rdx, %rdi					/* store x1 - x2 into %rdi */
	call square						/* compute (x1 - x2)^2, store in %rax */
	movq %rax, %r12					/* move (x1 - x2)^2 to %r12 */

	/* compute (y1 - y2)^2 */
	subq %rcx, %rsi			 		/* store y1 - y2 into %rsi */
	movq %rsi, %rdi					/* move y1 - y2 to %rdi */
	call square						/* compute (y1 - y2)^2, store in rax */

	/* add (y1 - y2)^2 and (x1 - x2)^2 */
	addq %r12, %rax

	/* restore the value originally in r12 */
	popq %r12
	ret


/*
 * Draw a pixel.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 *   %ecx     - uint32_t color value
 */
	.globl draw_pixel
draw_pixel:
	/* TODO: implement */
	ret


/*
 * Draw a rectangle.
 * The rectangle has rect->x,rect->y as its upper left corner,
 * is rect->width pixels wide, and rect->height pixels high.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - pointer to struct Rect
 *   %edx     - uint32_t color value
 */
	.globl draw_rect
draw_rect:
	/* TODO: implement */
	ret

/*
 * Draw a circle.
 * The circle has x,y as its center and has r as its radius.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate of circle's center
 *   %edx     - y coordinate of circle's center
 *   %ecx     - radius of circle
 *   %r8d     - uint32_t color value
 */
	.globl draw_circle
draw_circle:
	/* TODO: implement */
	ret

/*
 * Draw a tile by copying all pixels in the region
 * enclosed by the tile parameter in the tilemap image
 * to the specified x/y coordinates of the destination image.
 * No blending of the tile pixel colors with the background
 * colors should be done.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where tile should be copied
 *   %edx - y coordinate of location where tile should be copied
 *   %rcx - pointer to Image (the tilemap)
 *   %r8  - pointer to Rect (the tile)
 */
	.globl draw_tile
draw_tile:
	/* TODO: implement */
	ret

/*
 * Draw a sprite by copying all pixels in the region
 * enclosed by the sprite parameter in the spritemap image
 * to the specified x/y coordinates of the destination image.
 * The alpha values of the sprite pixels should be used to
 * blend the sprite pixel colors with the background
 * pixel colors.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where sprite should be copied
 *   %edx - y coordinate of location where sprite should be copied
 *   %rcx - pointer to Image (the spritemap)
 *   %r8  - pointer to Rect (the sprite)
 */
	.globl draw_sprite
draw_sprite:
	/* TODO: implement */
	ret

	.globl get_r
get_r:
	ret

	.globl get_g
get_g:
	ret

	.globl get_b
get_b:
	ret

	.globl get_a
get_a:
	ret

	.globl blend_colors
blend_colors:
	ret



/*
vim:ft=gas:
*/
