#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
#
# link ut_zebu
#

# Error out on error
set -e

# Nice output in kbuild format
# Will be supressed by "make -s"
info()
{
	if [ "${quiet}" != "silent_" ]; then
		printf "  %-7s %s\n" ${1} ${2}
	fi
}

# Thin archive build here makes a final archive with symbol table and indexes
# from ut_zebu objects INIT and MAIN, which can be used as input to linker.
# KBUILD_$(PROJECT)_LIBS archives should already have symbol table and indexes
# added.
#
# Traditional incremental style of link does not require this step
#
# built-in.a output file
#
archive_builtin()
{
	info AR built-in.a
		rm -f built-in.a;
	${AR} rcsTP${KBUILD_ARFLAGS} built-in.a			\
		${KBUILD_A1000_INIT}		\
		${KBUILD_A1000_MAIN}
}

# Link of ut_zebu
# ${1} - optional extra .o files
# ${2} - output file
ut_zebu_link()
{
	local lds="${objtree}/${KBUILD_LDS}"
	local objects

	objects="--whole-archive			\
	built-in.a				\
	--no-whole-archive			\
	--start-group				\
	${KBUILD_A1000_LIBS}			\
	--end-group				\
	${1}"

	${LD} ${KBUILD_LDFLAGS} ${LDFLAGS_ut_zebu} -o ${2}	\
	-T ${lds} ${KBUILD_A1000_HEAD} ${objects} ${PLATFORM_LIBS}
}

# Create map file with all symbols from ${1}
# See mksymap for additional details
mksysmap()
{
	${CONFIG_SHELL} "${srctree}/scripts/mksysmap" ${1} ${2}
}

# Delete output files in case of error
cleanup()
{
	rm -f .tmp_System.map
	rm -f .tmp_kallsyms*
	rm -f .tmp_vmlinux*
	rm -f built-in.a
	rm -f System.map
	rm -f ut_zebu
}

on_exit()
{
	if [ $? -ne 0 ]; then
		cleanup
	fi
}
trap on_exit EXIT

on_signals()
{
	exit 1
}
trap on_signals HUP INT QUIT TERM

#
#
# Use "make V=1" to debug this script
case "${KBUILD_VERBOSE}" in
*1*)
	set -x
;;
esac

if [ "$1" = "clean" ]; then
	cleanup
	exit 0
fi

# We need access to CONFIG_ symbols
case "${KCONFIG_CONFIG}" in
*/*)
	. "${KCONFIG_CONFIG}"
	;;
*)
	# Force using a file from the current directory
	. "./${KCONFIG_CONFIG}"
esac


# Update version
info GEN .version
if [ -r .version ]; then
	VERSION=$(expr 0$(cat .version) + 1)
	echo $VERSION > .version
else
	rm -f .version
	echo 1 > .version
fi;

archive_builtin

info LD ut_zebu
ut_zebu_link "" ut_zebu

info SYSMAP System.map
mksysmap ut_zebu System.map

