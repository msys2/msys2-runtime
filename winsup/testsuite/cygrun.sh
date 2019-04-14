#!/bin/dash
#
# test driver to run $1 in the appropriate environment
#

# $1 = test executable to run
exe=$1

export PATH="$runtime_root:${PATH}"

if [ "$1" = "./mingw/cygload" ]
then
    $cygrun "$exe -v -cygwin ./testinst/usr/bin/msys-2.0.dll"
else
    cygdrop $cygrun $exe
fi
