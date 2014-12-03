#!/usr/bin/env bash
# Bundle QT and adjust the library paths

CONTENTS="./bin/KeePassX.app/Contents"
FRAMEWORKS="${CONTENTS}/Frameworks"
BINARY="${CONTENTS}/MacOS/KeePassX"
error=""

if $(command -v brew --version >/dev/null 2>&1); then
	qtversion=$(brew ls --versions qt)
	if [[ "${qtversion}" == "" ]]; then
		error="Homebrew doesn't have QT. Please install it."
	else
		echo "Bundling ${qtversion}"
	fi
else
	error="This script requires Homebrew (http://brew.sh)."
fi

if [[ "${error}" != "" ]]; then
	echo "Error: ${error}"
	exit
fi

function changeLibraryPaths {
	echo "Changing library paths for ${1}"
	otool -l "${1}" | sed 1d | while read line; do
		if [[ "${line}" =~ Qt[A-Za-z]+\.framework ]]; then
			fullLibPath=$(echo "${line}" | perl -pe 's|[^/]+(/.*?) \(offset.*|\1|')
			shortLibPath=$(echo "${fullLibPath}" | perl -pe 's|.*?(Qt\w+\.framework)|\1|')
			install_name_tool -change "${fullLibPath}" "@executable_path/../Frameworks/${shortLibPath}" "${1}"
		fi
	done
}

rm -rf "${FRAMEWORKS}" && mkdir -p "${FRAMEWORKS}"

# Use readlink since Homebrew symlinks to the real location of the frameworks
locallib="$(brew --prefix)/lib"
cp -aX "${locallib}"/$(readlink "${locallib}/QtCore.framework") "${FRAMEWORKS}"
cp -aX "${locallib}"/$(readlink "${locallib}/QtGui.framework") "${FRAMEWORKS}"
cp -aX "${locallib}"/$(readlink "${locallib}/QtXml.framework") "${FRAMEWORKS}"

for f in ${FRAMEWORKS}/Qt*.framework/Versions/4/Qt* ; do
	chmod 644 "${f}" # Necessary since the libs aren't writable by the current user
	changeLibraryPaths "${f}"
done

strip "${BINARY}"
changeLibraryPaths "${BINARY}"
