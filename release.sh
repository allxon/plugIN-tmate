#!/bin/bash
TARGET_VERSION=$1
REGEX="BUILD_VERSION := '(.+)'"
TEMP_FILE=$(mktemp)

# Find version part
cat Makefile | while read LINE; do
    if [[ $LINE =~ $REGEX ]]; then
        echo ${BASH_REMATCH[1]} > ${TEMP_FILE}
    fi
done

CURRENT_VERSION=$(cat ${TEMP_FILE})
if [[ $CURRENT_VERSION == "" ]]; then
    echo "Can't found version"
    rm ${TEMP_FILE}
    exit 1
fi

if [[ $CURRENT_VERSION == $TARGET_VERSION ]]; then
    echo "Version already matched, don't need to update"
    rm ${TEMP_FILE}
    exit 0
else
    echo "Current version: $CURRENT_VERSION replace to Target version: $TARGET_VERSION"
    sed -i -E "s/BUILD_VERSION := '(.+)'/BUILD_VERSION := '$TARGET_VERSION'/g" Makefile
fi

rm ${TEMP_FILE}