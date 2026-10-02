#include "kc_image_layout.h"
bool kc_contains(const kc_image *image, uint64_t offset, uint64_t length) {
    return offset <= image->size && length <= image->size - offset;
}
const struct segment_command_64 *kc_locate_region(const kc_image *image, uint64_t header_offset, const char *name) {
    if (header_offset % 8 || !kc_contains(image, header_offset, sizeof(struct mach_header_64))) return NULL;
    const struct mach_header_64 *header = (const void *)(image->bytes + header_offset);
    if (header->magic != MH_MAGIC_64 || header->cputype != CPU_TYPE_ARM64) return NULL;
    uint64_t position = header_offset + sizeof(*header);
    if (!kc_contains(image, position, header->sizeofcmds) || header->ncmds > header->sizeofcmds / sizeof(struct load_command)) return NULL;
    const uint64_t end = position + header->sizeofcmds;
    const struct segment_command_64 *found = NULL;
    for (uint32_t index = 0; index < header->ncmds; ++index) {
        if (position > end || end - position < sizeof(struct load_command)) return NULL;
        const struct load_command *command = (const void *)(image->bytes + position);
        if (command->cmdsize < sizeof(*command) || command->cmdsize % 8 || command->cmdsize > end - position) return NULL;
        if (command->cmd == LC_SEGMENT_64) {
            if (command->cmdsize < sizeof(struct segment_command_64)) return NULL;
            const struct segment_command_64 *segment = (const void *)command;
            if (segment->nsects > (command->cmdsize - sizeof(*segment)) / sizeof(struct section_64)) return NULL;
            if (!strncmp(segment->segname, name, sizeof(segment->segname))) found = segment;
        }
        position += command->cmdsize;
    }
    return position == end ? found : NULL;
}
/* Only segments returned by kc_locate_region may be passed here. */
const struct section_64 *kc_locate_section(const struct segment_command_64 *segment, const char *name) {
    const struct section_64 *sections = (const void *)(segment + 1);
    for (uint32_t index = 0; index < segment->nsects; ++index) {
        if (!strncmp(sections[index].segname, segment->segname, sizeof(segment->segname)) &&
            !strncmp(sections[index].sectname, name, sizeof(sections[index].sectname))) return sections + index;
    }
    return NULL;
}
