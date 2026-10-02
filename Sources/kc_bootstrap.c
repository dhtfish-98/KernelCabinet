#include "kc_image_layout.h"
int main(int argc, const char **argv) {
    if (argc != 2) { fprintf(stderr, "Usage: %s owned-kernel-image\n", argv[0]); return 2; }
    int input = open(argv[1], O_RDONLY | O_NOFOLLOW | O_NONBLOCK | O_CLOEXEC);
    if (input < 0) { perror("input"); return 1; }
    struct stat metadata;
    if (fstat(input, &metadata) || !S_ISREG(metadata.st_mode) || metadata.st_size < (off_t)sizeof(struct mach_header_64) ||
        (uint64_t)metadata.st_size > KC_MAX_IMAGE_BYTES) {
        fprintf(stderr, "Input must be a regular Mach-O file of 32 bytes to 1 GiB.\n"); close(input); return 2;
    }
    size_t length = (size_t)metadata.st_size;
    unsigned char *bytes = malloc(length);
    if (!bytes) { close(input); perror("allocation"); return 1; }
    size_t position = 0;
    while (position < length) {
        ssize_t count = read(input, bytes + position, length - position);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) { fprintf(stderr, "Input changed or could not be read.\n"); free(bytes); close(input); return 1; }
        position += (size_t)count;
    }
    unsigned char extra;
    ssize_t count;
    do { count = read(input, &extra, 1); } while (count < 0 && errno == EINTR);
    close(input);
    if (count != 0) { fprintf(stderr, "Input grew or could not be read.\n"); free(bytes); return 1; }
    const kc_image image = {bytes, length};
    int result = kc_export_extensions(&image);
    free(bytes);
    return result;
}
