#!/bin/sh
OUTPUT=$(${1} -search a 2>&1)
rval=$?
# echo it out once just for ctest -V output
echo $OUTPUT
echo $OUTPUT | egrep -qi 'no results found.*'
nval=$?
if [ $rval -eq 2 ] && [ $nval -eq 0 ]; then
    exit 0;
else
    exit 1;
fi
