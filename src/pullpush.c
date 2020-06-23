#include "internal.h"
#include "script_int.h"

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include "pullpush.h"

unsigned char *wally_push_bytes(unsigned char **cursor, size_t *max,
                                const void *src, size_t len)
{
    if (cursor == NULL || *cursor == NULL) {
        *max += len;
        return NULL;
    }
    if (len > *max) {
        if (src)
            memcpy(*cursor, src, *max);
        /* From now on, max records the room we *needed* */
        *max = len - *max;
        *cursor = NULL;
        return NULL;
    }
    if (src)
        memcpy(*cursor, src, len);

    *cursor += len;
    *max -= len;

    return *cursor - len;
}

void wally_pull_bytes(void *dst, size_t len,
                      const unsigned char **cursor, size_t *max)
{
    if (len > *max) {
        memcpy(dst, *cursor, *max);
        memset((char *)dst + *max, 0, len - *max);
        wally_pull_failed(cursor, max);
        return;
    }
    memcpy(dst, *cursor, len);
    *cursor += len;
    *max -= len;
}

const unsigned char *wally_pull_skip(const unsigned char **cursor, size_t *max,
                                     size_t len)
{
    const unsigned char *p;

    if (*cursor == NULL) {
        return NULL;
    }

    if (len > *max) {
        wally_pull_failed(cursor, max);
        return NULL;
    }

    p = *cursor;
    *cursor += len;
    *max -= len;
    return p;
}

void wally_pull_failed(const unsigned char **cursor, size_t *max)
{
    *cursor = NULL;
    *max = 0;
}

void wally_push_le64(unsigned char **cursor, size_t *max, uint64_t v)
{
    leint64_t lev = cpu_to_le64(v);
    wally_push_bytes(cursor, max, &lev, sizeof(lev));
}

uint64_t wally_pull_le64(const unsigned char **cursor, size_t *max)
{
    leint64_t lev;
    wally_pull_bytes(&lev, sizeof(lev), cursor, max);
    return le64_to_cpu(lev);
}

void wally_push_le32(unsigned char **cursor, size_t *max, uint32_t v)
{
    leint32_t lev = cpu_to_le32(v);
    wally_push_bytes(cursor, max, &lev, sizeof(lev));
}

uint32_t wally_pull_le32(const unsigned char **cursor, size_t *max)
{
    leint32_t lev;
    wally_pull_bytes(&lev, sizeof(lev), cursor, max);
    return le32_to_cpu(lev);
}

void wally_push_le16(unsigned char **cursor, size_t *max, uint16_t v)
{
    leint16_t lev = cpu_to_le16(v);
    wally_push_bytes(cursor, max, &lev, sizeof(lev));
}

uint16_t wally_pull_le16(const unsigned char **cursor, size_t *max)
{
    leint16_t lev;
    wally_pull_bytes(&lev, sizeof(lev), cursor, max);
    return le16_to_cpu(lev);
}

void wally_push_u8(unsigned char **cursor, size_t *max, uint8_t v)
{
    wally_push_bytes(cursor, max, &v, sizeof(uint8_t));
}

uint8_t wally_pull_u8(const unsigned char **cursor, size_t *max)
{
    uint8_t v;
    wally_pull_bytes(&v, sizeof(v), cursor, max);
    return v;
}

void wally_push_varint(unsigned char **cursor, size_t *max, uint64_t v)
{
    unsigned char buf[sizeof(uint8_t) + sizeof(uint64_t)];
    size_t len = varint_to_bytes(v, buf);

    wally_push_bytes(cursor, max, buf, len);
}

uint64_t wally_pull_varint(const unsigned char **cursor, size_t *max)
{
    unsigned char buf[sizeof(uint8_t) + sizeof(uint64_t)];
    uint64_t v;

    /* FIXME: Would be more efficient to opencode varint here! */
    wally_pull_bytes(buf, 1, cursor, max);
    wally_pull_bytes(buf + 1, varint_length_from_bytes(buf) - 1, cursor, max);
    varint_from_bytes(buf, &v);

    return v;
}

void wally_push_varbuff(unsigned char **cursor, size_t *max,
                        const unsigned char *bytes, size_t bytes_len)
{
    wally_push_varint(cursor, max, bytes_len);
    wally_push_bytes(cursor, max, bytes, bytes_len);
}

size_t wally_pull_varlength(const unsigned char **cursor, size_t *max)
{
    uint64_t len = wally_pull_varint(cursor, max);

    if (len > *max) {
        /* Impossible length. */
        wally_pull_failed(cursor, max);
        return 0;
    }
    return len;
}

void wally_pull_subfield_start(const unsigned char *const *cursor, const size_t *max,
                               size_t subfield_len,
                               const unsigned char **subcursor, size_t *submax)
{
    if (subfield_len > *max) {
        wally_pull_failed(subcursor, submax);
    } else {
        *subcursor = *cursor;
        *submax = subfield_len;
    }
}

void wally_pull_subfield_end(const unsigned char **cursor, size_t *max,
                             const unsigned char *subcursor, size_t submax)
{
    if (subcursor == NULL) {
        wally_pull_failed(cursor, max);
    } else if (*cursor != NULL) {
        const unsigned char *subend = subcursor + submax;
        assert(subcursor >= *cursor);
        assert(subend <= *cursor + *max);
        *max -= (subend - *cursor);
        *cursor = subend;
    }
}
