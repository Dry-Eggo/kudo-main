
cwd=$(pwd)
header_path="$cwd/runtime/kudo.h"

make || {
    echo "Build Failed. Aborting Setup"
    exit 1
}
echo "Adding Kudo Header to Env"
echo "Header Path: $header_path"

if ! grep -q 'export KUDO_HEADER=' ~/.bashrc; then
    echo "export KUDO_HEADER=\"$header_path\"" >> ~/.bashrc &&
	echo "Added Kudo Header to Env"
else
    echo "Kudo Header Already Set"
fi

source ~/.bashrc
