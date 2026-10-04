#include "kc_image_layout.h"
static void kc_print_label(const char *label) {
    for (size_t index = 0; index < 64 && label[index]; ++index) {
        unsigned char byte = (unsigned char)label[index];
        if (byte < 32 || byte > 126 || byte == '\\') printf("\\x%02x", byte);
        else putchar(byte);
    }
}
static bool kc_address_offset(const kc_image *image, uint64_t base, uint64_t address, uint64_t length, uint64_t *offset) {
    address = KC_NORMALIZE_ADDRESS(address);
    if (address < base) return false;
    *offset = address - base;
    return kc_contains(image, *offset, length);
}
static bool kc_record_at(const kc_image *image, uint64_t base, uint64_t address, const kc_module_record **record) {
    uint64_t offset;
    if (!kc_address_offset(image, base, address, sizeof(**record), &offset) || offset % 4) return false;
    *record = (const void *)(image->bytes + offset);
    return memchr((*record)->name, 0, sizeof((*record)->name)) && memchr((*record)->version, 0, sizeof((*record)->version));
}
static bool kc_extension_at(const kc_image *image, uint64_t base, uint64_t address,
                            const struct segment_command_64 **segment, uint64_t *header_offset, uint64_t *data_offset,
                            uint64_t *remaining_commands) {
    if (!kc_address_offset(image, base, address, sizeof(struct mach_header_64), header_offset)) return false;
    *segment = kc_locate_region_limited(image, *header_offset, "__TEXT_EXEC", remaining_commands);
    if (!*segment || (*segment)->fileoff > image->size - *header_offset) return false;
    *data_offset = *header_offset + (*segment)->fileoff;
    return kc_contains(image, *data_offset, (*segment)->filesize);
}
static int kc_write_new_file(const char *name, const unsigned char *bytes, size_t length) {
    int output = open(name, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (output < 0) { perror("output (must not already exist)"); return 1; }
    size_t position = 0;
    while (position < length) {
        ssize_t count = write(output, bytes + position, length - position);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) { perror("output write (incomplete file retained)"); close(output); return 1; }
        position += (size_t)count;
    }
    if (close(output)) { perror("output close"); return 1; }
    printf("Wrote raw __TEXT_EXEC bytes to: %s\n", name);
    return 0;
}
int kc_export_extensions(const kc_image *image) {
    uint64_t remaining_commands = KC_MAX_AGGREGATE_COMMANDS;
    const struct segment_command_64 *text = kc_locate_region_limited(image, 0, "__TEXT", &remaining_commands);
    const struct segment_command_64 *info = kc_locate_region_limited(image, 0, "__PRELINK_INFO", &remaining_commands);
    if (!text || !info || text->fileoff != 0) goto invalid;
    const struct section_64 *starts = kc_locate_section(info, "__kmod_start");
    const struct section_64 *records = kc_locate_section(info, "__kmod_info");
    if (!starts || !records || starts->size != records->size || !starts->size || starts->size % sizeof(uint64_t) ||
        starts->offset % 8 || records->offset % 8 || !kc_contains(image, starts->offset, starts->size) ||
        !kc_contains(image, records->offset, records->size)) goto invalid;
    const uint64_t *start_values = (const void *)(image->bytes + starts->offset);
    const uint64_t *record_values = (const void *)(image->bytes + records->offset);
    size_t count = starts->size / sizeof(uint64_t);
    if (count > 4096) goto invalid;
    for (size_t index = 0; index < count; ++index) {
        const kc_module_record *record;
        const struct segment_command_64 *segment;
        uint64_t header_offset, data_offset;
        if (!kc_record_at(image, text->vmaddr, record_values[index], &record) ||
            !kc_extension_at(image, text->vmaddr, start_values[index], &segment, &header_offset, &data_offset,
                             &remaining_commands)) goto invalid;
        printf("index: %zu, name: ", index);
        kc_print_label(record->name);
        printf(", version: ");
        kc_print_label(record->version);
        printf(", vmaddr: 0x%016" PRIx64 "\n", KC_NORMALIZE_ADDRESS(segment->vmaddr));
    }
    printf("Select index to extract: ");
    char line[80];
    size_t line_length = 0;
    int next;
    while ((next = getchar()) != '\n' && next != EOF) {
        if (next == '\0' || line_length >= sizeof(line) - 1) goto invalid;
        line[line_length++] = (char)next;
    }
    if (ferror(stdin)) goto invalid;
    line[line_length] = '\0';
    char *start = line + strspn(line, " \t");
    if (*start < '0' || *start > '9') goto invalid;
    char *end;
    errno = 0;
    uintmax_t chosen = strtoumax(start, &end, 10);
    end += strspn(end, " \t\r\n");
    if (errno || *end || chosen >= count) goto invalid;
    size_t selection = (size_t)chosen;
    const struct segment_command_64 *segment;
    uint64_t header_offset, data_offset;
    if (!kc_extension_at(image, text->vmaddr, start_values[selection], &segment, &header_offset, &data_offset,
                         &remaining_commands)) goto invalid;
    char filename[80];
    snprintf(filename, sizeof(filename), "kernelcabinet-index-%zu.bin", selection);
    return kc_write_new_file(filename, image->bytes + data_offset, (size_t)segment->filesize);
invalid:
    fprintf(stderr, "Unsupported or malformed kernel image, metadata, or selection.\n");
    return 2;
}
