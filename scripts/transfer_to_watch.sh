#!/usr/bin/env bash
set -e

SOURCE_UAPP="/Users/scottcowie/repos/FreshBreath/Output/FreshBreath_1.0.0.uapp"
WATCH_VOL="/Volumes/UNA WATCH"
TARGET_DIR="${WATCH_VOL}/Apps/FreshBreath"

if [ ! -f "${SOURCE_UAPP}" ]; then
    echo "Error: Source binary ${SOURCE_UAPP} not found. Please compile first."
    exit 1
fi

echo "Waiting for UNA WATCH to be connected via USB..."
TIMEOUT=30
ELAPSED=0

while [ ! -d "${WATCH_VOL}" ]; do
    sleep 1
    ELAPSED=$((ELAPSED + 1))
    if [ ${ELAPSED} -ge ${TIMEOUT} ]; then
        echo "Timed out waiting for '${WATCH_VOL}' to mount."
        echo "Please connect the watch via USB cable and ensure Mass Storage is active."
        exit 1
    fi
done

echo "Found watch drive at ${WATCH_VOL}!"
mkdir -p "${TARGET_DIR}"
echo "Copying FreshBreath_1.0.0.uapp to ${TARGET_DIR}..."
cp "${SOURCE_UAPP}" "${TARGET_DIR}/"

# Clean Apple Double resource files
dot_clean "${TARGET_DIR}" 2>/dev/null || true
rm -f "${TARGET_DIR}/._"* 2>/dev/null || true

sync

SRC_MD5=$(md5 -q "${SOURCE_UAPP}")
DST_MD5=$(md5 -q "${TARGET_DIR}/FreshBreath_1.0.0.uapp")

echo "Source MD5: ${SRC_MD5}"
echo "Watch  MD5: ${DST_MD5}"

if [ "${SRC_MD5}" = "${DST_MD5}" ]; then
    echo "SUCCESS: FreshBreath_1.0.0.uapp transferred and verified successfully!"
    echo "You may now safely eject the watch drive and power-cycle the watch."
else
    echo "ERROR: MD5 mismatch!"
    exit 1
fi
