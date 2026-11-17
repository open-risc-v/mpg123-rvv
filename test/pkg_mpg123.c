// mpg123 A/B 探针：喂一段固定 MP3（test/data/tiny.mp3）解码成 S16 PCM 并哈希。
// mpg123 是纯解码器，整数 MDCT/合成滤波器组，交叉下应当 bit-exact。
#include <mpg123.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTBLK 8192

static uint64_t fnv1a(const void *p, size_t n)
{
    const uint8_t *b = p;
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; i++) { h ^= b[i]; h *= 1099511628211ULL; }
    return h;
}

int main(int argc, char **argv)
{
    const char *inp = argc > 1 ? argv[1] : "test/data/tiny.mp3";
    const char *outp = argc > 2 ? argv[2] : NULL;

    mpg123_init();
    mpg123_handle *mh = mpg123_new(NULL, NULL);
    if (!mh) { fprintf(stderr, "mpg123_new 失败\n"); return 2; }

    mpg123_format_none(mh);
    /* 放开常见采样率与 16bit 输出；首次 read 会先返回 NEW_FORMAT */
    if (mpg123_format_all(mh) != MPG123_OK) {
        fprintf(stderr, "mpg123_format_all 失败\n");
        return 3;
    }
    if (mpg123_open(mh, inp) != MPG123_OK) {
        fprintf(stderr, "打不开 %s: %s\n", inp, mpg123_strerror(mh));
        return 3;
    }

    FILE *f = NULL;
    if (outp) {
        f = fopen(outp, "wb");
        if (!f) { fprintf(stderr, "打不开 %s\n", outp); return 5; }
    }

    unsigned char buf[OUTBLK];
    size_t done = 0, total = 0;
    uint64_t h = 0, hq8 = 1469598103934665603ULL;
    for (;;) {
        int rc = mpg123_read(mh, buf, sizeof buf, &done);
        if (done) {
            h ^= fnv1a(buf, done);
            h *= 1099511628211ULL;
            for (size_t k = 0; k + 1 < done; k += 2) {
                int16_t smp = (int16_t)(buf[k] | ((unsigned char)buf[k + 1] << 8));
                int q = (smp + 128) >> 8;
                hq8 ^= (uint64_t)(uint8_t)q;
                hq8 *= 1099511628211ULL;
            }
            total += done;
            if (f) fwrite(buf, 1, done, f);
        }
        if (rc == MPG123_DONE) break;
        if (rc != MPG123_OK && rc != MPG123_NEW_FORMAT) {
            fprintf(stderr, "解码错误 rc=%d: %s\n", rc, mpg123_strerror(mh));
            return 4;
        }
    }
    if (f) fclose(f);

    printf("decoded=%zu fnv_pcm=%016llx fnv_q8=%016llx\n", total,
           (unsigned long long)h, (unsigned long long)hq8);

    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    return 0;
}
