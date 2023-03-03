#!/bin/bash

OUTPUT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").output"
CURRENT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").current"

exec > "${CURRENT}"

while :
do
	test -e /tmp/tmate.sock > /dev/null || { echo "N/A"; break; }

	URL=$(tmate -S /tmp/tmate.sock display -p '#{tmate_ssh}')

	echo ${URL}

	break
done

if ! test -f ${OUTPUT} > /dev/null || ! diff ${OUTPUT} ${CURRENT} > /dev/null; then
        mv ${CURRENT} ${OUTPUT}
fi
