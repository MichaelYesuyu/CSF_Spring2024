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
 *   %edi     - the value to square
 * 
 * Returns (in %rax):
 *  the square of the input value
 */
	.globl square
square:
	imull %edi, %edi           /* multiply the value in rdi by itself*/
	movl %edi, %eax			   /* move the multiplied value to rax*/
	ret

/*
 * Compute the square of the geomoetrical distance between 
 * (x1, y1) and (x2, y2)
 *
 * Parameters:
 *   %edi     - x1
 *   %esi     - y1
 *   %edx     - x2
 *   %ecx     - y2
 * 
 * Returns (in %eax):
 *  the square of the geomoetrical distance between (x1, y1)
 *  and (x2, y2)
 */
	.globl square_dist
square_dist:
	
	/* save the value originally in r12 to the memory */
	pushq %r12

	/* compute (x1 - x2)^2 */
	subl %edx, %edi					/* store x1 - x2 into %rdi */
	call square						/* compute (x1 - x2)^2, store in %rax */
	movl %eax, %r12d				/* move (x1 - x2)^2 to %r12 */

	/* compute (y1 - y2)^2 */
	subl %ecx, %esi			 		/* store y1 - y2 into %rsi */
	movl %esi, %edi					/* move y1 - y2 to %rdi */
	call square						/* compute (y1 - y2)^2, store in rax */

	/* add (y1 - y2)^2 and (x1 - x2)^2 */
	addl %r12d, %eax

	/* restore the value originally in r12 */
	popq %r12
	ret

/*
 * check if a point is within the range. if not, return -1. Otherwise, return 1
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate of the target point
 *   %edx     - y coordinate of the target point
 * 
 * Returns (in %rax):
 *  whether the point is in the bound or not
 */
	.globl in_bounds
in_bounds:

    pushq %r12                              /* save the value in %r12 to memory */
    
    movl IMAGE_WIDTH_OFFSET(%rdi), %r12d     /* Load width value from struct Image */
    cmpl $0, %esi                           /* compare x with 0 */
    jl .LoutOfBound                         /* jump to out of bound flag is x < 0 */
    cmpl %r12d, %esi                         /* compare x to width */
    jge .LoutOfBound                        /* jump to out of bound flag is x >= width */
    
    movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d    /* Load height value from struct Image */
    cmpl $0, %edx                           /* compare y with 0 */
    jl .LoutOfBound                         /* jump to out of bound flag is y < 0 */
    cmpl %r12d, %edx                         /* compare y to height */
    jge .LoutOfBound                        /* jump to out of bound flag is y >= height */
    
    movq $1, %rax                           /* 1 means the input is in bound */
    popq %r12                               /* restore the value originally in %r12 */
    ret

.LoutOfBound:
    movq $-1, %rax                          /* -1 means the input is not in bound */
    popq %r12                               /* restore the value originally in %r12 */
    ret
	
/*
 * Compute the index at a given point (x,y)
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate of the target point
 *   %edx     - y coordinate of the target point
 * 
 * Returns (in %eax):
 *  the index of the target pixel
 */
	.globl compute_index
compute_index:

	pushq %r12 								/* save the value in %r12 to memory */
	movl IMAGE_WIDTH_OFFSET(%rdi), %r12d 	/* get the width of the given image */
	imulq %r12, %rdx						/* multiply the y coordinate with width */ /*special note: index is 64 bit. x and y coordinates are 32 bit.*/
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
 * Returns (in %eax):
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
 * 	 %edx     - the color to be write into the image

 * Returns (in %rax):
 *  no return
 */
	.globl set_pixel
set_pixel:

	movq IMAGE_DATA_OFFSET(%rdi), %r10 			/* get the pointer that points to the data (pixel array) */
	movl %edx, (%r10, %rsi, 4) 					/* move target color to the destination pixel */
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

	pushq %r12 
	pushq %r13
	pushq %r14

	/* check if the given point is within the image limit */
	call in_bounds
	cmpq $0, %rax
	jl .Lending

	/* get the index of the given point */
	call compute_index
	movl %esi, %r12d						/* save the x coordinate to r12 */
	movq %rax, %rsi							/* put the new input value (index) to call register*/
	
	/* get the pixel of the given index (background color)*/
	call get_pixel
	movq %rdi, %r13							/* save the struct pointer color to r13 */
	movl %ecx, %edi							/* put the foreground color to input register */
	movq %rsi, %r14							/* put the index to r14d */
	movl %eax, %esi							/* put the background color to input register */
	/* blend the foreground and background color */
	call blend_colors
	movq %r13, %rdi							/* put the struct pointer back to rdi */
	movq %r14, %rsi							/* put the index back to rsi */
	movl %eax, %edx							/* put the color to edx */

	/* put the blended color back to the image */
	call set_pixel

.Lending:
	popq %r14								/* restore the value originally in %r12 */
	popq %r13								/* restore the value originally in %r13 */
	popq %r12								/* restore the value originally in %r14 */
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
 *
 * Register Use:
 *	%r12 - Stores the x value of rect as the loop iterates
 *	%r13 - Stores the y value of rect as the loop iterates
 *	%r14 - Stores the ending x value of rect as the loop iterates
 *	%r15 - Stores the ending y value of rect as the loop iterates
 *	%ebx - Stores the color value for the duration of the function
 *	%rbp - Stores the pointer to the struct Image for the duration of the function
 */
	.globl draw_rect
draw_rect:
	pushq %r12 /* Preserve value of %r12 */
	pushq %r13 /* Preserve value of %r13 */
	pushq %r14 /* Preserve value of %r14 */
	pushq %r15 /* Preserve value of %r15 */
	pushq %rbx /* Preserve value of %rbx */
	pushq %rbp /* Preserve value of %rbp */

	movl %edx, %ebx 							/* Store the color value inside %ebx */
	movq %rdi, %rbp 							/* Store the pointer to the struct Image for the duration of the function */
	movl RECT_X_OFFSET(%rsi), %r12d 			/* Store the value of the x starting point on the rectangle in %r12 */
	movl RECT_Y_OFFSET(%rsi), %r13d 			/* Store the value of the y starting point on the rectangle in %r13 */
	
	movl RECT_X_OFFSET(%rsi), %r14d 			/* Store the value of the x starting point in %r14 */
	addl RECT_WIDTH_OFFSET(%rsi), %r14d 		/* Add the width to get the ending x coordinate */
	movl RECT_Y_OFFSET(%rsi), %r15d 			/* Store the value of the y starting point in %r15 */
	addl RECT_HEIGHT_OFFSET(%rsi), %r15d 		/* Add the height to get the ending y coordinate */

	/* Nested for loop to loop through the rectangle */
		jmp .LcheckInXLoop
	
	.LloopBody:
		/* first argument Image pointer is already inside %rdi register */
		movl %r12d, %esi
		movl %r13d, %edx
		movq %rdi, %rbp
		call draw_pixel

	.LcheckInYLoop:
		cmpq %r15, %r13 						/* check if current y coord is less than y ending coord */
		inc %r13 								/* Increment y coord by 1 */
		jl .LloopBody

	.LcheckInXLoop:
		cmpq %r14, %r12 						/* check if current x coord is less than x ending coord */
		inc %r12 								/* Increment x coord by 1 */
		jl .LcheckInYLoop


	popq %rbp 									/* Restore value of %rbp */
	popq %rbx 									/* Restore value of %rbx */
	popq %r15 									/* Restore value of %r15 */
	popq %r14 									/* Restore value of %r14 */
	popq %r13 									/* Restore value of %r12 */
	popq %r12 									/* Restore value of %r13 */
	ret


/*
 * check if a point is within the circle. if not, return -1. Otherwise, return 1
 *
 * Parameters:
 *   %edi     - x coordinate of the center
 *   %esi     - y coordinate of the center
 *   %edx     - r -- the radius of the circle
 *   %ecx     - x coordinate of the target point
 *   %r8d     - y coordinate of the target point
 * 
 * Returns (in %rax):
 *  whether the point is in the circle or not
 */
	.globl in_circle
in_circle:

    pushq %r12                              /* save the value in %r12 to memory */
    pushq %r13                              /* save the value in %r13 to memory */
    pushq %r14                              /* save the value in %r14 to memory */
    
    movl %edx, %r12d						/* move the r value to r12d */
	movl %ecx, %edx							/* move the x of the target point to input register 3 */
	movl %r8d, %ecx							/* move the y of the target point to input register 4 */
    call square_dist						/* compute the square distance of the point and the circle center */		
	movl %eax, %r13d						/* move the computed square distance to r13d */
	
	movl %r12d, %edi							/* move r to edi */
	call square								/* compute the square of r */
	cmpl %r13d, %eax						/* compare the r square and the square distance */
	jl .LoutOfCircle						/* if r square is smaller than square distance, jump */


    movq $1, %rax                           /* 1 means the input is in bound */
    popq %r14                               /* restore the value originally in %r12 */
    popq %r13                               /* restore the value originally in %r12 */
	popq %r12                               /* restore the value originally in %r12 */
    ret

.LoutOfCircle:
    movq $-1, %rax                          /* -1 means the input is not in bound */
    popq %r14                               /* restore the value originally in %r12 */
    popq %r13                               /* restore the value originally in %r12 */
	popq %r12                               /* restore the value originally in %r12 */
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
    shrl $24, %edi 		/* Perform right shift by 24 to set the lower 8 bits of rdi to r component value */
    movb %dil, %al 		/* Move the lower 8 bits of edi into al (the 8 bit sub-register of rax) */
	ret 				/* returns the r component value */


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
    shrl $16, %edi 		/* Perform right shift by 16 to set the lower 8 bits of rdi to g component value */
    movb %dil, %al 		/* Move the lower 8 bits of edi into al (the 8 bit sub-register of rax) */
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
	shrl $8, %edi 		/* Perform right shift by 8 to set the lower 8 bits of rdi to b component value */
    movb %dil, %al 		/* Move the lower 8 bits of edi into al (the 8 bit sub-register of rax) */
	ret 				/* returns the b component value */

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
    movb %dil, %al 		/* Move the lower 8 bits of edi into al (the 8 bit sub-register of rax) */
	ret 				/* returns the a component value */


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
    imull %edx, %edi 		/* computes alpha * fg and stores it in %edi */
    movl %edx, %r10d 		/* stores the value of alpha inside %r10d */
	movl $255, %edx 		/* stores 255 inside %edx to be subtracted by alpha later */
    subl %r10d, %edx 		/* computes (255 - alpha) and stores it inside %edx */
    imull %esi, %edx 		/* computes (255 - alpha) * bg and stores it inside %edx */
    addl %edi, %edx 		/* computes ((alpha * fg) + (255 - alpha) * (bg)) and stores it inside %edx */
    movl %edx, %eax 		/* moves the dividend into %eax */
	xor %edx, %edx 			/* clear all the bits of edx as it is unused in division */
	movl $255, %r10d 		/* moves the constant 255 into %r10d to divide the whole equation in the next step */
    divl %r10d 				/* computes ((alpha * fg) + (255 - alpha) * (bg)) / 255  and stores it inside %eax */
    ret 					/* Only the lower 8 bits (al) should have bits set to 1 */

/*
 * Computes the new blended color of the foreground and background colors
 * 
 * Parameters: 
 *  %edi - 32 bit integer storing the foreground color
 *  %esi - 32 bit integer storing the background color
 *
 * Returns (in %eax):
 *  32 bit integer storing the blended color
 *
 * Register use:
 *  %rbp - base pointer, for allocating space for local variables
 *  %rsp - stack pointer, for tracking local variables
 *	%r12 - stores the value of the input foreground color for the duration of the function
 *	%r13 - stores the value of the input background color for the duration of the function
*/
	.globl blend_colors
blend_colors:
	pushq %r12 				/* Push register onto stack to store value of foreground color */
	pushq %r13 				/* Push register onto stack to store value of background color */
    pushq %rbp 				/* push base pointer onto stack before allocating stack memory */
    movq %rsp, %rbp 		/* move stack pointer to the address pointed by the base pointer */
    subq $48, %rsp 			/* allocate 32 bytes of memory for the local variables */

	movl %edi, %r12d 		/* Moves value of foreground color into %r12 */
	movl %esi, %r13d 		/* Moves value of background color into %r13 */

    xor %eax, %eax 			/* clear out eax register before calling helper function */
	movl %r12d, %edi 		/* Moves foreground color into 1st argument register */
    call get_r 				/* Call get_r function with foreground color, result is stored in %al, subregister of %eax */
    movl %eax, -4(%rbp) 	/* Store foreground r component (as 32 bit value) inside the stack */

	movl %r12d, %edi 		/* Moves foreground color into 1st argument register */
    call get_g 				/* Call get_g function with foreground color */
    movl %eax, -8(%rbp) 	/* Store foreground g component inside the stack */

	movl %r12d, %edi 		/* Moves foreground color into 1st argument register */
    call get_b 				/* Call get_b function with foreground color */
    movl %eax, -12(%rbp) 	/* Store foreground b component inside the stack */

	movl %r12d, %edi 		/* Moves foreground color into 1st argument register */
    call get_a 				/* Call get_a function with foreground color */
    movl %eax, -16(%rbp) 	/* Store foreground alpha value inside the stack */

    movl %r13d, %edi 		/* Moves background color into 1st argument register */
    call get_r 				/* Call get_r function with background color */
    movl %eax, -20(%rbp) 	/* Store background r component inside the stack */

	movl %r13d, %edi 		/* Moves background color into 1st argument register */
    call get_g 				/* Call get_g function with background color */
    movl %eax, -24(%rbp) 	/* Store background g component inside the stack */

	movl %r13d, %edi 		/* Moves background color into 1st argument register */
    call get_b 				/* Call get_b function with background color */
    movl %eax, -28(%rbp) 	/* Store background b component inside the stack */

    movl -4(%rbp), %edi 	/* Move fg_r into the first argument register */
    movl -20(%rbp), %esi 	/* Move bg_r into the second argument register */
    movl -16(%rbp), %edx 	/* Move alpha into the third argument register */
    xor %eax, %eax 			/* Clear %eax register before calling blend_components */
    call blend_components 	/* Call function with fg_r, bg_r, alpha */
    movl %eax, -32(%rbp) 	/* Store the new r component inside the stack */

    movl -8(%rbp), %edi 	/* Move fg_g into the first argument register */
    movl -24(%rbp), %esi 	/* Move bg_g into the second argument register */
	movl -16(%rbp), %edx 	/* Move alpha into the third argument register */
    call blend_components 	/* Call function with fg_g, bg_g, alpha */
    movl %eax, -36(%rbp) 	/* Store the new g component inside the stack */

    movl -12(%rbp), %edi 	/* Move fg_b into the first argument register */
    movl -28(%rbp), %esi 	/* Move bg_b into the second argument register */
	movl -16(%rbp), %edx 	/* Move alpha into the third argument register */
    call blend_components 	/* Call function with fg_b, bg_b, alpha */
    movl %eax, -40(%rbp) 	/* Store the new b component inside the stack */

    xor %eax, %eax 			/* Set the %eax register to 0 before adding the color inside */
    orl -32(%rbp), %eax 	/* Sets eax color to r */
    shll $8, %eax 			/* Left shift %eax by 8 bits */ 
    orl -36(%rbp), %eax 	/* Sets the lowest 8 bits of color to g */
    shll $8, %eax 			/* Left shift %eax by 8 bits */
    orl -40(%rbp), %eax 	/* Sets the lowest 8 bits of color to b */
    shll $8, %eax 			/* Left shift %eax by 8 bits */
    orl $255, %eax 			/* Sets the lowest 8 bits of color to alpha (default 255) */

    addq $48, %rsp 			/* reset stack pointer after function is done, local variables no longer needed */
    popq %rbp  				/* pop the base pointer from the stack after function is over */
	popq %r13 				/* restore value of %r13 */
	popq %r12 				/* restore value of %r12 */

	ret 					/* Returns the new color (in %eax) */



/*
vim:ft=gas:
*/
