#ifndef __STRUCT_SIGNATURE_H__
#define __STRUCT_SIGNATURE_H__

#ifndef DEFINE_STRUCTURE
#define DEFINE_STRUCTURE(STRUCT_NAME, ...) \
    typedef struct { \
        __VA_ARGS__ \
    } STRUCT_NAME; \
    static inline const char *generate_##STRUCT_NAME##_signature() { \
        static const char signature[] = #STRUCT_NAME "{"#__VA_ARGS__"}"; \
        return signature; \
    }
#endif // DEFINE_STRUCTURE
#endif // __STRUCT_SIGNATURE_H__