here="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
source /grid/fermiapp/products/uboone/setup_uboone.sh
setup gcc v4_9_3
setup git
setup cmake v3_3_2
source $here/configure.sh
