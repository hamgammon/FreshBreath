#!/usr/bin/env bash
# Package FreshBreath for the UNA Watch App Store per UNA SDK specification.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
UNA_SDK="${UNA_SDK:-/Users/scottcowie/repos/una-sdk}"
VERSION="1.0.1"
UAPP_NAME="FreshBreath_${VERSION}.uapp"
ZIP_NAME="FreshBreath_${VERSION}.zip"

echo "1. Validating Output/app-manifest.json..."
python3 "${UNA_SDK}/Utilities/Scripts/app_packer/min_kernel_version.py" --check "${REPO_ROOT}/Output/app-manifest.json"
python3 "${UNA_SDK}/Utilities/Scripts/app_packer/validate_app_config.py" --check "${REPO_ROOT}/Output/app-manifest.json"

echo "2. Staging App Store package..."
STAGE_DIR="$(mktemp -d)"
trap 'rm -rf "${STAGE_DIR}"' EXIT

cp "${REPO_ROOT}/Output/${UAPP_NAME}" "${STAGE_DIR}/${UAPP_NAME}"
cp "${REPO_ROOT}/Output/app-manifest.json" "${STAGE_DIR}/app-manifest.json"
cp "${REPO_ROOT}/Resources/icon.png" "${STAGE_DIR}/icon.png"

mkdir -p "${REPO_ROOT}/dist"
cp "${REPO_ROOT}/Output/${UAPP_NAME}" "${REPO_ROOT}/dist/${UAPP_NAME}"
rm -f "${REPO_ROOT}/dist/${ZIP_NAME}"

(
  cd "${STAGE_DIR}"
  zip -r "${REPO_ROOT}/dist/${ZIP_NAME}" "${UAPP_NAME}" app-manifest.json icon.png
)

echo "3. Verifying ${REPO_ROOT}/dist/${ZIP_NAME} contents:"
unzip -l "${REPO_ROOT}/dist/${ZIP_NAME}"
echo "SUCCESS: App Store package created at ${REPO_ROOT}/dist/${ZIP_NAME}"
