#include "kc_image_layout.h"

void
kc_export_extensions(struct mach_header_64 *kc_binding_018) {
	struct segment_command_64 *kc_binding_019, *kc_binding_020, *kc_binding_021;
	struct section_64 *kc_binding_022, *kc_binding_023, *kc_binding_024;
	uint64_t *kc_binding_025, *kc_binding_026;
	kc_binding_006 *kc_binding_027;
	struct mach_header_64 *kc_binding_028;
	size_t kc_binding_029, kc_binding_030;
	int kc_binding_031;
	
	if((kc_binding_019 = kc_locate_region(kc_binding_018, "__TEXT"))) {
		if((kc_binding_020 = kc_locate_region(kc_binding_018, "__PRELINK_INFO"))) {
			if((kc_binding_022 = kc_locate_section(kc_binding_020, "__kmod_start"))) {
				if((kc_binding_023 = kc_locate_section(kc_binding_020, "__kmod_info"))) {
					kc_binding_025 = (uint64_t *)((uintptr_t)kc_binding_018 + kc_binding_022->offset);
					kc_binding_026 = (uint64_t *)((uintptr_t)kc_binding_018 + kc_binding_023->offset);
					kc_binding_030 = KC_RULE_01(kc_binding_023->size, kc_binding_022->size) / sizeof(uint64_t);
					for(kc_binding_029 = 0; kc_binding_029 < kc_binding_030; ++kc_binding_029) {
						kc_binding_028 = (struct mach_header_64 *)((uintptr_t)kc_binding_018 + KC_RULE_02(kc_binding_025[kc_binding_029]) - kc_binding_019->vmaddr);
						if((kc_binding_021 = kc_locate_region(kc_binding_028, "__TEXT_EXEC"))) {
							kc_binding_027 = (kc_binding_006 *)((uintptr_t)kc_binding_018 + KC_RULE_02(kc_binding_026[kc_binding_029]) - kc_binding_019->vmaddr);
							printf("index: %zu, name: %s, version: %s, vmaddr: 0x%016" PRIx64 "\n", kc_binding_029, kc_binding_027->kc_binding_004, kc_binding_027->kc_binding_005, KC_RULE_02(kc_binding_021->vmaddr));
						}
					}
					printf("Select index to extract: ");
					if(scanf("%zu", &kc_binding_029) == 1 && kc_binding_029 < kc_binding_030) {
						kc_binding_027 = (kc_binding_006 *)((uintptr_t)kc_binding_018 + KC_RULE_02(kc_binding_026[kc_binding_029]) - kc_binding_019->vmaddr);
						if((kc_binding_031 = open(kc_binding_027->kc_binding_004, O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) != -1) {
							kc_binding_028 = (struct mach_header_64 *)((uintptr_t)kc_binding_018 + KC_RULE_02(kc_binding_025[kc_binding_029]) - kc_binding_019->vmaddr);
							if((kc_binding_021 = kc_locate_region(kc_binding_028, "__TEXT_EXEC"))) {
								if((kc_binding_024 = kc_locate_section(kc_binding_021, "__text"))) {
									kc_binding_021->vmaddr = KC_RULE_02(kc_binding_021->vmaddr);
									kc_binding_024->addr = KC_RULE_02(kc_binding_024->addr);
									if(write(kc_binding_031, (const void *)((uintptr_t)kc_binding_028 + kc_binding_021->fileoff), kc_binding_021->filesize) != -1) {
										printf("Wrote kext to file: %s\n", kc_binding_027->kc_binding_004);
									}
								}
							}
							close(kc_binding_031);
						}
					} else {
						puts("Invalid index");
					}
				}
			}
		}
	}
}
