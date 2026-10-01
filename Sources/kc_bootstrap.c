#include "kc_image_layout.h"

int
main(int kc_binding_032, const char **kc_binding_033)
{
	if(kc_binding_032 != 2) {
		printf("Usage: %s kernel\n", kc_binding_033[0]);
	} else {
		int kc_binding_034 = open(kc_binding_033[1], O_RDONLY);
		size_t kc_binding_035 = (size_t)lseek(kc_binding_034, 0, SEEK_END);
		struct mach_header_64 *kc_binding_036 = mmap(NULL, kc_binding_035, PROT_READ | PROT_WRITE, MAP_PRIVATE, kc_binding_034, 0);
		close(kc_binding_034);
		if(kc_binding_036 != MAP_FAILED) {
			if(kc_binding_036->magic == MH_MAGIC_64 && kc_binding_036->cputype == CPU_TYPE_ARM64) {
				kc_export_extensions(kc_binding_036);
			}
			munmap(kc_binding_036, kc_binding_035);
		}
	}
}
