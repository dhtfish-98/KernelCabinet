#include "kc_image_layout.h"

struct segment_command_64 *
kc_locate_region(struct mach_header_64 *kc_binding_008, const char *kc_binding_009) {
	struct segment_command_64 *kc_binding_010 = (struct segment_command_64 *)((uintptr_t)kc_binding_008 + sizeof(*kc_binding_008));
	uint32_t kc_binding_011;
	
	for(kc_binding_011 = 0; kc_binding_011 < kc_binding_008->ncmds; ++kc_binding_011) {
		if(kc_binding_010->cmd == LC_SEGMENT_64 && !strncmp(kc_binding_010->segname, kc_binding_009, sizeof(kc_binding_010->segname))) {
			return kc_binding_010;
		}
		kc_binding_010 = (struct segment_command_64 *)((uintptr_t)kc_binding_010 + kc_binding_010->cmdsize);
	}
	return NULL;
}

struct section_64 *
kc_locate_section(struct segment_command_64 *kc_binding_013, const char *kc_binding_014) {
	struct section_64 *kc_binding_015 = (struct section_64 *)((uintptr_t)kc_binding_013 + sizeof(*kc_binding_013));
	uint32_t kc_binding_016;
	
	for(kc_binding_016 = 0; kc_binding_016 < kc_binding_013->nsects; ++kc_binding_016) {
		if(!strncmp(kc_binding_015->segname, kc_binding_013->segname, sizeof(kc_binding_015->segname)) && !strncmp(kc_binding_015->sectname, kc_binding_014, sizeof(kc_binding_015->sectname))) {
			return kc_binding_015;
		}
		++kc_binding_015;
	}
	return NULL;
}
