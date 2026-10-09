#!/bin/dash
#
# test driver to run $1 in the appropriate environment
#

# $1 = test executable to run
exe=$1

export PATH="$runtime_root:${PATH}"

if [ "$1" = "./mingw/cygload" ]
then
    windows_runtime_root=$(cygpath -m $runtime_root)
    MSYS_NO_PATHCONV=1 $cygrun "$exe -v -cygwin $windows_runtime_root/msys-2.0.dll"
else
    cygdrop $cygrun $exe
fi
