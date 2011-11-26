#ifndef LIB_MEMORY_H
#define LIB_MEMORY_H

extern void *memory_clear(void *out, unsigned int count);
extern int memory_compare(const void *in1, const void *in2, unsigned int count);
extern void *memory_copy(void *out, const void *in, unsigned int count);
extern unsigned int memory_index(const void *in, char value, unsigned int count, unsigned int skip);
extern unsigned int memory_index_reversed(const void *in, char value, unsigned int count, unsigned int skip);
extern void *memory_replace(void *out, char value1, char value2, unsigned int count);

#endif

