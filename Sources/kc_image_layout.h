#ifndef KC_IMAGE_LAYOUT_H
#define KC_IMAGE_LAYOUT_H

#include <fcntl.h>
#include <inttypes.h>
#include <mach-o/loader.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define KC_RULE_01(kc_macro_value_1, kc_macro_value_2) ((kc_macro_value_1) < (kc_macro_value_2) ? (kc_macro_value_1) : (kc_macro_value_2))
#define KC_RULE_02(kc_macro_value_1) ((kc_macro_value_1) | UINT64_C(0xffff000000000000))
#define KC_RULE_03 (64)

#pragma pack(4)
typedef struct {
	uint64_t kc_binding_001;
	int32_t  kc_binding_002;
	uint32_t kc_binding_003;
	char     kc_binding_004[KC_RULE_03];
	char     kc_binding_005[KC_RULE_03];
} kc_binding_006; /* From xnu/osfmk/mach/kmod.h */
#pragma pack()

struct segment_command_64 *
kc_locate_region(struct mach_header_64 *kc_binding_008, const char *kc_binding_009);
struct section_64 *
kc_locate_section(struct segment_command_64 *kc_binding_013, const char *kc_binding_014);
void
kc_export_extensions(struct mach_header_64 *kc_binding_018);

#endif
