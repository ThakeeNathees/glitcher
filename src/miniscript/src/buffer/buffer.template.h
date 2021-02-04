/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

/** A template header to emulate C++ template and every occurence of 
 * $name$ will be replaced by the name of the buffer and $type$ will be
 * replaced by the element type of the buffer (by a pre compile script) */

#ifndef BUFFER_TEMPLATE_H
#define BUFFER_TEMPLATE_H

#include "../common.h"
#include "../vm.h"

 /** The factor by which the buffer will grow when it's capacity reached */
#define GROW_FACTOR 2

/** The initial capacity of the buffer */
#define MIN_CAPACITY 16

/** A place holder typedef to prevent IDE syntax errors. Remove this line 
 * when generating the source. */
typedef uint8_t $type$;

typedef struct {
	$type$* data;
	size_t count;
	size_t capacity;
} $name$Buffer;

/** Initialize a new buffer int instance. */
void ms_$name$BufferInit($name$Buffer* self);

/** Clears the allocated elementes from the VM's realloc function. */
void ms_$name$BufferClear($name$Buffer* self, VM* vm);

/** Fill the buffer at the end of it with provided data if the capacity isn't
 * enough using VM's realloc function. */
void ms_$name$BufferFill($name$Buffer* self, VM* vm, $type$ data, int count);

/** Write to the buffer with provided data at the end of the buffer. */
void ms_$name$BufferWrite($name$Buffer* self, VM* vm, $type$ data);

#endif // BUFFER_TEMPLATE_H
