#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef enum {
	PROTO_HELLO,
} proto_type_e;

typedef struct {
	proto_type_e type;
	unsigned int length;
} proto_hdr_t;

#endif
