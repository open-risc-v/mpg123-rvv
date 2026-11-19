#!/usr/bin/env bash
set -euo pipefail
SRC=$1
OUT=$2
mkdir -p "$OUT/pkg"
rm -rf "$OUT/pkg/src"
cp -R "$SRC" "$OUT/pkg/src"
cd "$OUT/pkg/src"
# mpg123 的 aux 文件（ltmain.sh/compile/missing/install-sh）不在发行包里，先补
autoreconf -fi > "$OUT/pkg/bootstrap.log" 2>&1 || { tail -25 "$OUT/pkg/bootstrap.log"; exit 1; }
./configure --prefix="$OUT/pkg/prefix" --disable-shared --enable-static \
  --disable-lfs --with-cpu=generic \
  > "$OUT/pkg/configure.log" 2>&1 || { tail -25 "$OUT/pkg/configure.log"; exit 1; }
make -j"$(nproc)" > "$OUT/pkg/build.log" 2>&1 || { tail -25 "$OUT/pkg/build.log"; exit 1; }
make install > "$OUT/pkg/install.log" 2>&1 || { tail -10 "$OUT/pkg/install.log"; exit 1; }
