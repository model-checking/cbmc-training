#include <stdlib.h>
#include <stdbool.h>

typedef struct { size_t length; char* buffer; } strbuf;

char cache;
void cache_fifth_char(strbuf* str) { cache = str->buffer[4]; }

strbuf* strbuf_allocate(size_t length) {
  strbuf* str = malloc(sizeof(strbuf));
  if (str == NULL) return NULL;
  str->length = length;
  str->buffer = malloc(length);
  return str;
}

main() {
  size_t len;
  strbuf* str = strbuf_allocate(len);
  cache_fifth_char(str);
}
