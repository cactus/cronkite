#!/bin/bash
export CRONKITE_AURURL=http://aur.test
VG="${1}"
shift
ARG="${*}"
${VG} --tool=memcheck --leak-check=yes \
     --show-reachable=yes --num-callers=30 \
     --leak-check=summary --leak-resolution=high \
     ${ARG} > /dev/null
exit $?
