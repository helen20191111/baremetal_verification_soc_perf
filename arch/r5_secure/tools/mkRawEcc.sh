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
cp ${srctree}/arch/r5_secure/tools/mkRawEcc ${objtree}/arch/r5_secure/boot/r5_secure_mkRawEcc
cd ${objtree}/arch/${ARCH}/boot
rm -f ${ecc_obj}
./r5_secure_mkRawEcc ${ecc_src}
chmod 777 Ecc_Secure.raw
cd ../../..

