/* RVV 内核：mpg123 的 DCT64 第一级蝶形（逐元素加/减/乘，位精确）。
 * 对应 dct64.c 开头两个 16 长度循环：
 *   bufs[i]     = samples[i] + samples[47-i]                     (i=0..15)
 *   bufs[16+j]  = (samples[15-j] - samples[16+j]) * costab[15-j] (j=0..15)
 * 每个输出元素只做一次加/减/乘，向量化不改变任何单个元素的运算顺序。
 */
#ifndef MPG_DCT64_RVV_H
#define MPG_DCT64_RVV_H
#include <riscv_vector.h>

static inline void mpg_dct64_first_rvv(float *bufs, const float *samples, const float *costab)
{
    size_t vlmax = __riscv_vsetvlmax_e32m1();
    vuint32m1_t vid = __riscv_vid_v_u32m1(vlmax);
    vuint32m1_t rev = __riscv_vrsub_vx_u32m1(vid, (unsigned)(vlmax - 1), vlmax);

    for (int i = 0; i < 16; ) {
        size_t vl = (size_t)(16 - i) < vlmax ? (size_t)(16 - i) : vlmax;
        vfloat32m1_t a = __riscv_vle32_v_f32m1(samples + i, vl);
        vfloat32m1_t b = __riscv_vle32_v_f32m1(samples + (32 - i - (int)vl), vl);
        b = __riscv_vrgather_vv_f32m1(b, rev, vl);
        __riscv_vse32_v_f32m1(bufs + i, __riscv_vfadd_vv_f32m1(a, b, vl), vl);
        i += (int)vl;
    }
    for (int j = 0; j < 16; ) {
        size_t vl = (size_t)(16 - j) < vlmax ? (size_t)(16 - j) : vlmax;
        vfloat32m1_t a  = __riscv_vle32_v_f32m1(samples + 16 + j, vl);
        vfloat32m1_t sb = __riscv_vle32_v_f32m1(samples + (16 - j - (int)vl), vl);
        vfloat32m1_t c  = __riscv_vle32_v_f32m1(costab + (16 - j - (int)vl), vl);
        sb = __riscv_vrgather_vv_f32m1(sb, rev, vl);
        c  = __riscv_vrgather_vv_f32m1(c, rev, vl);
        __riscv_vse32_v_f32m1(bufs + 16 + j,
                              __riscv_vfmul_vv_f32m1(__riscv_vfsub_vv_f32m1(sb, a, vl), c, vl), vl);
        j += (int)vl;
    }
}
#endif
