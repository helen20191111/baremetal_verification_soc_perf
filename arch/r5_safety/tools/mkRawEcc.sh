#!/bin/sh
#
# mkEcc secure.raw
#
set -e

case "${KBUILD_VERBOSE}" in
*1*)
	set -x
	;;
esac

ecc_src=$(basename $1)
ecc_obj=$(basename $2)
cp ${srctree}/arch/r5_safety/tools/mkRawEcc ${objtree}/arch/r5_safety/boot/r5_safety_mkRawEcc
cd ${objtree}/arch/${ARCH}/boot
rm -f ${ecc_obj}
./r5_safety_mkRawEcc ${ecc_src}
chmod 777 Ecc_Safety.raw
cd ../../..

