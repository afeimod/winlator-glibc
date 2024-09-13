#!/bin/bash -e
#This is a modifed version of the script from https://github.com/Weab-chan/freedreno_turnip-CI
green='\033[0;32m'
red='\033[0;31m'
nocolor='\033[0m'

deps="meson ninja patchelf unzip curl pip3 flex bison zip git aarch64-linux-gnu-gcc-11 aarch64-linux-gnu-g++-11"
workdir="$(pwd)/turnip_workdir"
packagedir="$workdir/turnip_module"
sdkver="28"
mesasrc="https://gitlab.freedesktop.org/Pipetto-crypto/mesa.git"

#array of string => commit/branch;patch args
base_patches=(
	"Quest3;../../patches/quest3.patch;"
	"Meson;../../patches/meson.patch;"
)
failed_patches=()
commit=""
commit_short=""
mesa_version=""
vulkan_version=""
clear

# there are 4 functions here, simply comment to disable.
# you can insert your own function and make a pull request.
run_all(){
	check_deps
	if (( ${#base_patches[@]} )); then
		prep "patched"
	fi
}

prep () {
	prepare_workdir "$1"
	build_lib_for_android
	extract_lib "$1"
}

check_deps(){
	sudo apt remove meson
	pip3 install meson PyYAML

	echo "Checking system for required Dependencies ..."
	for deps_chk in $deps;
		do
			sleep 0.25
			if command -v "$deps_chk" >/dev/null 2>&1 ; then
				echo -e "$green - $deps_chk found $nocolor"
			else
				echo -e "$red - $deps_chk not found, can't countinue. $nocolor"
				deps_missing=1
			fi;
		done

		if [ "$deps_missing" == "1" ]
			then echo "Please install missing dependencies" && exit 1
		fi

	echo "Installing python Mako dependency (if missing) ..." $'\n'
	pip3 install mako &> /dev/null
}

prepare_workdir(){
	echo "Creating and entering to work directory ..." $'\n'
	mkdir -p "$workdir" && cd "$_"

	if [ -d mesa ]; then
		echo "Removing old mesa ..." $'\n'
		rm -rf mesa
	fi

	echo "Cloning mesa ..." $'\n'
	git clone --depth=1 "$mesasrc"

	cd mesa
	commit_short=$(git rev-parse --short HEAD)
	commit=$(git rev-parse HEAD)
	mesa_version=$(cat VERSION | xargs)
	version=$(awk -F'COMPLETE VK_MAKE_API_VERSION(|)' '{print $2}' <<< $(cat include/vulkan/vulkan_core.h) | xargs)
	major=$(echo $version | cut -d "," -f 2 | xargs)
	minor=$(echo $version | cut -d "," -f 3 | xargs)
	patch=$(awk -F'VK_HEADER_VERSION |\n#define' '{print $2}' <<< $(cat include/vulkan/vulkan_core.h) | xargs)
	vulkan_version="$major.$minor.$patch"

	if [ $1 == "patched" ]; then 
		apply_patches ${base_patches[@]}
	fi
}

apply_patches() {
	local arr=("$@")
	for patch in "${arr[@]}"; do
		echo "Applying patch $patch"
		patch_source="$(echo $patch | cut -d ";" -f 2 | xargs)"
		patch_args=$(echo $patch | cut -d ";" -f 3 | xargs)
		if [[ $patch_source == *"../.."* ]]; then
			if git apply $patch_args "$patch_source"; then
				echo "Patch applied successfully"
			else
				echo "Failed to apply $patch"
				failed_patches+=("$patch")

			fi
		else 
			patch_file="${patch_source#*\/}"
			curl --output "../$patch_file".patch -k --retry-delay 30 --retry 5 -f --retry-all-errors https://gitlab.freedesktop.org/mesa/mesa/-/"$patch_source".patch
			sleep 1

			if git apply $patch_args "../$patch_file".patch ; then
				echo "Patch applied successfully"
			else
				echo "Failed to apply $patch"
				failed_patches+=("$patch")
				
			fi
		fi
	done
}

patch_to_description() {
	local arr=("$@")
	for patch in "${arr[@]}"; do
		patch_name="$(echo $patch | cut -d ";" -f 1 | xargs)"
		patch_source="$(echo $patch | cut -d ";" -f 2 | xargs)"
		patch_args="$(echo $patch | cut -d ";" -f 3 | xargs)"
		if [[ $patch_source == *"../.."* ]]; then
			echo "- $patch_name, $patch_source, $patch_args" >> description
		else 
			echo "- $patch_name, [$patch_source](https://gitlab.freedesktop.org/mesa/mesa/-/$patch_source), $patch_args" >> description
		fi
	done
}

build_lib_for_android(){
	echo "Creating meson cross file ..." $'\n'
	cat <<EOF >"android-aarch64"
[binaries]
ar = 'aarch64-linux-gnu-ar'
c = ['aarch64-linux-gnu-gcc-11']
cpp = ['aarch64-linux-gnu-g++-11', '-fno-exceptions', '-fno-unwind-tables', '-fno-asynchronous-unwind-tables', '-static-libstdc++']
c_ld = 'gold'
cpp_ld = 'gold'
strip = 'aarch64-linux-gnu-strip'
pkgconfig = ['env', '/usr/bin/pkg-config']
[host_machine]
system = 'android'
cpu_family = 'aarch64'
cpu = 'armv8'
endian = 'little'
EOF

	echo "Generating build files ..." $'\n'
	meson build-android-aarch64 --cross-file "$workdir"/mesa/android-aarch64 -Dbuildtype=release -Dplatforms=android -Dplatform-sdk-version=$sdkver -Dandroid-stub=true -Dgallium-drivers= -Dvulkan-drivers=freedreno -Dvulkan-beta=true -Dfreedreno-kmds=kgsl -Db_lto=true

	echo "Compiling build files ..." $'\n'
	ninja -C build-android-aarch64
}

extract_lib(){
	echo "Using patchelf to match soname ..."  $'\n'
	rm libvulkan_freedreno.so
	cp "$workdir"/mesa/build-android-aarch64/src/freedreno/vulkan/libvulkan_freedreno.so "$workdir"
	cd "$workdir"
	patchelf --set-soname vulkan.adreno.so libvulkan_freedreno.so
}

run_all
