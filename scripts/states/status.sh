#!/bin/bash

OUTPUT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").output"
CURRENT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").current"

exec > "${CURRENT}" 

while :
do
	which tmate > /dev/null || { echo "Not running"; break; }

	test -e /tmp/tmate.sock > /dev/null || { echo "Closed"; break; }

	echo "Open"

	break
done

if ! test -f ${OUTPUT} > /dev/null || ! diff ${OUTPUT} ${CURRENT} > /dev/null; then
	mv -f ${CURRENT} ${OUTPUT}
fi
