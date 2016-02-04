
export SWTRIGGER_INCDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
export SWTRIGGER_BUILDDIR=$SWTRIGGER_INCDIR/build
export SWTRIGGER_LIBDIR=$SWTRIGGER_BUILDDIR/lib
export LD_LIBRARY_PATH=$SWTRIGGER_LIBDIR:$LD_LIBRARY_PATH

if [ `command -v rootcling` ]; then
    export SWTRIGGER_ROOT6=1
else 
    if [ `command -v rootcint` ]; then
	export SWTRIGGER_ROOT5=1
    fi
fi

