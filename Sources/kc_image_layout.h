#ifndef KC_IMAGE_LAYOUT_H
#define KC_IMAGE_LAYOUT_H
/* Format derives from kextract; see ORIGIN.md and LICENSE.
 * Bounded parsing and output policy rewritten in October 2026. */
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <mach-o/loader.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define KC_MAX_IMAGE_BYTES (UINT64_C(1024) * 1024 * 1024)
#ifndef KC_MAX_AGGREGATE_COMMANDS
#define KC_MAX_AGGREGATE_COMMANDS UINT64_C(1000000)
#endif
#define KC_NORMALIZE_ADDRESS(value) ((value) | UINT64_C(0xffff000000000000))
#pragma pack(push, 4)
typedef struct {
    uint64_t next;
    int32_t info_version;
    uint32_t id;
    char name[64];
    char version[64];
} kc_module_record;
#pragma pack(pop)
typedef struct { const unsigned char *bytes; size_t size; } kc_image;
bool kc_contains(const kc_image *image, uint64_t offset, uint64_t length);
const struct segment_command_64 *kc_locate_region(const kc_image *image, uint64_t header_offset, const char *name);
const struct segment_command_64 *kc_locate_region_limited(const kc_image *image, uint64_t header_offset,
                                                          const char *name, uint64_t *remaining_commands);
const struct section_64 *kc_locate_section(const struct segment_command_64 *segment, const char *name);
int kc_export_extensions(const kc_image *image);
#endif
