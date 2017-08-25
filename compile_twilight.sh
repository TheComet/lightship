#!/bin/sh

./scripts/cross-compile.py \
    --build=1 \
    --compiler-root=/usr/bin/x86_64-pc-linux-gnu \
    --cmake="CMAKE_PREFIX_PATH=/usr/urho3d" \
    --cmake="CMAKE_BUILD_TYPE=Release" \
    --make="make -j7" \
    --install="make install" \
    --fetch-urho3d-data \
    --compress=xz
