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
 * check if a point is within the range. if not, return -1. Otherwise, return 1
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - x coordinate of the target point
 *   %rdx     - y coordinate of the target point
 * 
 * Returns (in %rax):
 *  whether the point is in the bound or not
 */
	.globl in_bounds
in_bounds:

	cmpq %rsi

/*
 * Compute the index at a given point (x,y)
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - x coordinate of the target point
 *   %rdx     - y coordinate of the target point
 * 
 * Returns (in %rax):
 *  the index of the target pixel
 */
	.globl compute_index
compute_index:

	pushq %r12 								/* save the value in %r12 to memory */
	movq IMAGE_WIDTH_OFFSET(%rdi), %r12 	/* get the width of the given image */
	imulq %r12, %rdx						/* multiply the y coordinate with width */
	addq %rdx, %rsi							/* compute x + y * width, store in esi */
	movq %rsi, %rax							/* move x + y * width to rax */
	popq %r12								/* restore the value originally in %r12 */
	ret

/*
 * Return the color stored in the given pixel index
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - index of the pixel to retrieve
 * 
 * Returns (in %rax):
 *  the uint32_t pixel color value
 */
	.globl get_pixel
get_pixel:

	movq IMAGE_DATA_OFFSET(%rdi), %r10  		/* get the pointer that points to the data (pixel array) */
	movl (%r10, %rsi, 4), %eax                  /* retrieve the element value */
	ret											/* return value */

/*
 * Set a pixel with a given index to a certain color (without blending)
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - index of the pixel to rewrite
 * 	 %rdx     - the color to be write into the image

 * Returns (in %rax):
 *  no return
 */
	.globl set_pixel
set_pixel:

	movq IMAGE_DATA_OFFSET(%rdi), %r10 			/* get the pointer that points to the data (pixel array) */
	movl %rdx, (%r10, %rsi, 4) 					/* move target color to the destination pixel */
	ret											/* return */




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

	/* get the index of the given point */

	/* get the pixel of the given index (background color)*/

	/* blend the foreground and background color */

	/* put the blended color back to the image */


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

/*
 * Gets the 8 bit integer representing the r component of the 32 bit integer color
 * r is stored in bits 24-31
 *
 * Parameters:
 *  %edi - 32 bit integer storing an RBG color and alpha value
 *
 * Returns (in %al):
 *  8 bit integer representing the r component
*/

    .globl get_r
get_r:
    shrl $24, %edi /* Perform right shift by 24 to set the lower 8 bits of rdi to r component value */
    movb %dil, %al /* Move the lower 8 bits of edi into al (the 8 bit sub-register of rax) */
	ret /* returns the r component value */


/*
 * Gets the 8 bit integer representing the g component of the 32 bit integer color
 * g is stored in bits 16-23
 *
 * Parameters:
 *  %edi - 32 bit integer storing an RBG color and alpha value
 *
 * Returns (in %al):
 *  8 bit integer representing the g component
*/
	.globl get_g
get_g:
    shrl $16, %edi /* Perform right shift by 16 to set the lower 8 bits of rdi to g component value */
    movb %dil, %al /* Move the lower 8 bits of edi into al (the 8 bit sub-register of rax) */
	ret /* returns the G component value */

/*
 * Gets the 8 bit integer representing the b component of the 32 bit integer color
 * b is stored in bits 8-15
 *
 * Parameters:
 *  %edi - 32 bit integer storing an RBG color and alpha value
 *
 * Returns (in %al):
 *  8 bit integer representing the b component
*/
	.globl get_b
get_b:
	shrl $8, %edi /* Perform right shift by 8 to set the lower 8 bits of rdi to b component value */
    movb %dil, %al /* Move the lower 8 bits of edi into al (the 8 bit sub-register of rax) */
	ret /* returns the b component value */

/*
 * Gets the 8 bit integer representing the a component of the 32 bit integer color
 * a is stored in bits 0-7
 *
 * Parameters:
 *  %edi - 32 bit integer storing an RBG color and alpha value
 *
 * Returns (in %al):
 *  8 bit integer representing the a component
*/
	.globl get_a
get_a:
    movb %dil, %al /* Move the lower 8 bits of edi into al (the 8 bit sub-register of rax) */
	ret /* returns the a component value */


/*
 * Computes the blended component given the foreground component, background component and alpha value
 * Follows the formula: blended_component = ((alpha * fg) + (255 - alpha) * (bg)) / 255
 *
 * Parameters: 
 *  %edi - 32 bit integer storing the foreground component to be blended (fg)
 *  %esi - 32 bit integer storing the background component to be blended (bg)
 *  %edx - 32 bit integer storing the alpha value
 *
 * Returns (in %al):
 *  8 bit integer representing the blended component
 *
 * Register use: 
 *  %r10d - for temporary storage during computation
 *  %eax - used to store ((alpha * fg) + (255 - alpha) * (bg)) to be divided by 255
 *
*/
    .globl blend_components
blend_components:
    mull %edx, %edi /* computes alpha * fg and stores it in %edi */
    movl $255, %r10d /* stores the constant value 255 inside %r10d */
    subl %r10d, %edx /* computes 255 - alpha and stores it inside %edx */
    mull %esi, %edx /* computes (255 - alpha) * bg and stores it inside %edx */
    addl %edi, %edx /* computes ((alpha * fg) + (255 - alpha) * (bg)) and stores it inside %edx */
    movl %edx, %eax /* moves the dividend into %eax */
    divl %r10d /* computes ((alpha * fg) + (255 - alpha) * (bg)) / 255  and stores it inside %eax */
    ret /* Only the lower 8 bits (al) should have bits set to 1 */


	.globl blend_colors
blend_colors:
	ret



/*
vim:ft=gas:
*/
