#pragma once
#define RELEASE "0.0.1indev"
#define __OranGOS 1
#define PIC_OFFSET 0x20
#define KERNEL_BASE_OFFSET 0xC0000000
#define KERNEL_DEFAULT_PHYS 0x200000
#define KERNEL_SIZE 0x200000 // gonna set it to 2Mib. Because it's better to
//overshoot it so I can sleep at night not worrying about my kernel being to small.
