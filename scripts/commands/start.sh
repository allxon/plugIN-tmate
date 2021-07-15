#!/bin/bash

OUTPUT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]%.*}").output"
PASSWORD=
OPTIONS=$(getopt -l "password:" -o "p:" -a -- "$@")
eval set -- "$OPTIONS"

while true; do
case $1 in
-p|--password) 
    shift
    export PASSWORD=$1
    ;;
--)
    shift
    break;;
esac
shift
done

if [ $UID -eq 0 ]; then
    exec > "${OUTPUT}"
    which tmate > /dev/null || id tmate > /dev/null 2>&1 || { echo "Not installed"; exit 0; }
    chown tmate:sudo "${OUTPUT}"
    echo -e "${PASSWORD:=password}\n${PASSWORD:=password}" | passwd -q tmate 2>&1
    exec su -c "$0" tmate "$@"
    exit $?
fi

exec >> "${OUTPUT}"

test -f ~/.ssh/id_rsa || ssh-keygen -q -t rsa -N '' -f ~/.ssh/id_rsa <<<y 2>&1 >/dev/null

test -e /tmp/tmate.sock > /dev/null || { cd ~ && tmate -S /tmp/tmate.sock new-session -d && tmate -S /tmp/tmate.sock wait tmate-ready; }

tmate -S /tmp/tmate.sock display -p '#{tmate_ssh}'

tmate -S /tmp/tmate.sock display -p '#{tmate_web}'
