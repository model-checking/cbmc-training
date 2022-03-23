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

bool strbuf_is_valid(strbuf* str) {
  if (str == NULL) return false;
  if (str->buffer == NULL) return false;
  return true;
}

main() {
  size_t len;
  strbuf* str = strbuf_allocate(len);

  __CPROVER_assume(strbuf_is_valid(str));
  __CPROVER_assume(str->length > 4);

  cache_fifth_char(str);

  __CPROVER_assert(strbuf_is_valid(str), "String buffer remains valid");
  __CPROVER_assert(str->length > 4, "String buffer remains length >4");
}
