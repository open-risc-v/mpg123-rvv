#!/usr/bin/env bash
# mpg123 真实包用例：解码仓库自带的确定性 MP3 语料，输出 PCM 逐字节比。
set -euo pipefail
: "${STAGE:=run}" "${SIDE:=A}" "${BIN:=build/native}" "${DATA:=build/ab-data}" "${OUT:=build/ab-data/out}"
mkdir -p "$DATA" "$OUT"
CASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
case "$STAGE" in
  gen)
    cp "$CASE_DIR/../../test/data/tiny.mp3" "$DATA/tiny.mp3"
    ;;
  run)
    H=""
    for c in "$BIN/mpg123_harness" "$BIN/tools/mpg123_harness"; do [ -x "$c" ] && H="$c" && break; done
    if [ -z "$H" ]; then echo "缺少 mpg123_harness（该侧未构建）" >&2; exit 1; fi
    ${RUNNER:-} "$H" "$DATA/tiny.mp3" "$OUT/decoded.pcm"
    ;;
esac
