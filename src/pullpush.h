#ifndef LIBWALLY_CORE_PULLPUSH_H
#define LIBWALLY_CORE_PULLPUSH_H 1
/**
 * Safely copy @len bytes from @src into @cursor.
 *
 * If @cursor is NULL or *@cursor is NULL, then nothing will be
 * written to it (obv), but *@max will be increased @len.
 *
 * Otherwise, if @len > *@max, *@max bytes will be copied from @src to
 * *@cursor if @src is not NULL, then *@cursor will be set to NULL and
 * @max will be set to the remainder.
 *
 * Otherwise, @len bytes will be copied from @src to *@cursor if @src
 * is not NULL, *@max decreased by @len and @cursor increased by @len.
 *
 * This means you can call it repeatedly, and if it fails, *@cursor will be
 * NULL, and *@max will indicate how many additional bytes you need.
 *
 * On success, this returns *@cursor before it was updated, otherwise NULL.
 */
unsigned char *wally_push_bytes(unsigned char **cursor, size_t *max,
                                const void *src, size_t len);

/**
 * Safely copy @len bytes from @cursor into @dst.
 *
 * If @len > *@max, *@max bytes will be copied from *@cursor, the
 * remainder of @dst will be zeroed, then *@cursor will be set to NULL
 * and @max will be set to 0.
 * Otherwise, @len bytes will be copied from @cursor, *@max decreased
 * by @len and @cursor increased by @len.
 */
void wally_pull_bytes(void *dst, size_t len,
                      const unsigned char **cursor, size_t *max);


/**
 * Return a pointer to (and skip over) some bytes.
 *
 * Returns NULL (and maybe calls wally_pull_failed()) if there are not enough left.
 */
const unsigned char *wally_pull_skip(const unsigned char **cursor, size_t *max,
                                     size_t len);


/**
 * Convenience function to indicate a wally_pull failed (eg. type decode failed)
 *
 * Sets *cursor to NULL, *max to 0.
 */
void wally_pull_failed(const unsigned char **cursor, size_t *max);

/**
 * Convenience functions.
 */
void wally_push_le64(unsigned char **cursor, size_t *max, uint64_t v);
uint64_t wally_pull_le64(const unsigned char **cursor, size_t *max);
void wally_push_le32(unsigned char **cursor, size_t *max, uint32_t v);
uint32_t wally_pull_le32(const unsigned char **cursor, size_t *max);
void wally_push_le16(unsigned char **cursor, size_t *max, uint16_t v);
uint16_t wally_pull_le16(const unsigned char **cursor, size_t *max);
void wally_push_u8(unsigned char **cursor, size_t *max, uint8_t v);
uint8_t wally_pull_u8(const unsigned char **cursor, size_t *max);

void wally_push_varint(unsigned char **cursor, size_t *max, uint64_t v);
uint64_t wally_pull_varint(const unsigned char **cursor, size_t *max);

void wally_push_varbuff(unsigned char **cursor, size_t *max,
                        const unsigned char *bytes, size_t bytes_len);

/* Calls wally_pull_failed() (and returns 0) if length would exceed remaining *max */
size_t wally_pull_varlength(const unsigned char **cursor, size_t *max);

/**
 * Functions to parse a subfield within an area.
 *
 * wally_pull_subfield_start: initializes subcursor/submax to subfield_len at
 * cursor.  These can then be used with wally_pull_ methods like normal.
 *
 * end_subfield_wally_pull: updates *cursor and *max to point after the subfield has
 * been parsed.  If *subcursor is NULL, it's equivalent to wally_pull_failed.
 */
void wally_pull_subfield_start(const unsigned char *const *cursor, const size_t *max,
                               size_t subfield_len,
                               const unsigned char **subcursor, size_t *submax);

void wally_pull_subfield_end(const unsigned char **cursor, size_t *max,
                             const unsigned char *subcursor,
                             const size_t submax);

#endif /* LIBWALLY_CORE_PULLPUSH_H */
