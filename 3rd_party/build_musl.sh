#!/usr/bin/env bash

MUSL_VERSION=v1.2.1
MUSL_DIST=musl-${MUSL_VERSION}-dist

if [ ! -d ${MUSL_DIST} ]; then
    if [ ! -d "musl" ]; then
        git clone --depth=1 -b ${MUSL_VERSION} git://git.musl-libc.org/musl
    fi

    cd musl
    ./configure

    mkdir ../${MUSL_DIST}
    DESTDIR=../${MUSL_DIST} make -j2 install
fi

