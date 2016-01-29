
export SWTRIGGER_INCDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
export SWTRIGGER_BUILDDIR=$SWTRIGGER_INCDIR/build
export SWTRIGGER_LIBDIR=$SWTRIGGER_BUILDDIR/lib
export LD_LIBRARY_PATH=$SWTRIGGER_LIBDIR:$LD_LIBRARY_PATH

export SWTRIGGER_ROOT=5;
if [ `command -v rootcling` ]; then
    export SWTRIGGER_ROOT=6
else 
    if [ `command -v rootcint` ]; then
	export SWTRIGGER_ROOT=5
    else
	unset SWTRIGGER_ROOT
    fi
fi

