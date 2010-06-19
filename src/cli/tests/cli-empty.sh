#!/bin/sh
OUTPUT=$(${1} -search example 2>&1)
rval=$?
# echo it out once just for ctest -V output
echo $OUTPUT
echo $OUTPUT | egrep -qi 'no content'
nval=$?
if [ $rval -eq 1 ] && [ $nval -eq 0 ]; then
    exit 0;
else
    exit 1;
fi
