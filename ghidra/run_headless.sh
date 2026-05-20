#!/usr/bin/env bash
# Headless Ghidra import + analysis for bambu_networking.dll (and the
# Scylla-unpacked dump). This is the only entry point for Ghidra work.
#
# Usage:
#     ghidra/run_headless.sh                            # packed DLL + InitialAnalysis
#     ghidra/run_headless.sh ScriptName.java            # run a different post-script
#     ghidra/run_headless.sh ScriptName.java -arg1      # forward args to it
#
# Environment overrides (all optional):
#     TARGET_BINARY              - DLL/dump to import (default: ./unpacking/bambu_networking.dll)
#     PROJECT_NAME               - Ghidra project name (default: baltobu)
#     IMAGE_BASE                 - hex/decimal image base to rebase to before analysis
#                                  (default: empty - keep what the loader picked)
#     STRIP_PROTECTOR_SECTIONS=1 - drop .o1} / .2F; / .1M@ before auto-analysis
#                                  (use when importing the Scylla dump)
#
# Examples:
#     # Packed DLL (default)
#     ghidra/run_headless.sh
#
#     # Unpacked dump, forced to original image base, protector sections stripped
#     TARGET_BINARY=unpacking/dumps/bambu_networking_dump.dll \
#     PROJECT_NAME=baltobu-unpacked \
#     IMAGE_BASE=0x180000000 \
#     STRIP_PROTECTOR_SECTIONS=1 \
#       ghidra/run_headless.sh PostUnpackAnalysis.java
#
# The script is idempotent: subsequent runs reuse the existing project
# rather than re-importing.

set -euo pipefail

GHIDRA_HOME="${GHIDRA_HOME:-/home/fboisselier/ghidra_12.0.4_PUBLIC}"
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_DIR="${REPO_ROOT}/ghidra/projects"
PROJECT_NAME="${PROJECT_NAME:-baltobu}"
TARGET_BINARY="${TARGET_BINARY:-${REPO_ROOT}/unpacking/bambu_networking.dll}"
IMAGE_BASE="${IMAGE_BASE:-}"
SCRIPT_DIR="${REPO_ROOT}/ghidra/scripts"
OUTPUT_DIR="${REPO_ROOT}/output/ghidra"

# Resolve to absolute path (analyzeHeadless needs absolute or CWD-relative).
case "${TARGET_BINARY}" in
    /*) ;;
    *) TARGET_BINARY="${REPO_ROOT}/${TARGET_BINARY}" ;;
esac
TARGET_NAME="$(basename "${TARGET_BINARY}")"

SCRIPT_NAME="${1:-InitialAnalysis.java}"
shift || true

if [[ ! -x "${GHIDRA_HOME}/support/analyzeHeadless" ]]; then
    echo "ERROR: analyzeHeadless not found at ${GHIDRA_HOME}/support/" >&2
    echo "Set GHIDRA_HOME to point at your Ghidra install." >&2
    exit 1
fi
if [[ ! -f "${TARGET_BINARY}" ]]; then
    echo "ERROR: target binary not found: ${TARGET_BINARY}" >&2
    exit 1
fi

# Catch the silent-mismatch footgun: if PROJECT_NAME is non-default but
# TARGET_BINARY wasn't explicitly set, we'd reuse the wrong project's
# program name and analyzeHeadless would error with "Requested project
# program file(s) not found". Fail loudly instead.
if [[ "${PROJECT_NAME}" != "baltobu" ]] && [[ -z "${TARGET_BINARY_OVERRIDE:-}" ]] \
   && [[ "${TARGET_BINARY}" == "${REPO_ROOT}/unpacking/bambu_networking.dll" ]]; then
    echo "ERROR: PROJECT_NAME=${PROJECT_NAME} but TARGET_BINARY was not set." >&2
    echo "Set TARGET_BINARY so the reuse branch can locate the program." >&2
    exit 1
fi

mkdir -p "${PROJECT_DIR}" "${OUTPUT_DIR}"

# Detect whether the project already contains the program. Ghidra stores
# imported binaries inside <project>.rep/idata/, so the .gpr+.rep pair plus
# a non-empty idata is a reliable signal.
ALREADY_IMPORTED=0
if [[ -f "${PROJECT_DIR}/${PROJECT_NAME}.gpr" ]] \
   && [[ -d "${PROJECT_DIR}/${PROJECT_NAME}.rep/idata" ]] \
   && find "${PROJECT_DIR}/${PROJECT_NAME}.rep/idata" -name '*.db' -print -quit | grep -q .; then
    ALREADY_IMPORTED=1
fi

PRESCRIPT_ARGS=()
if [[ -n "${IMAGE_BASE}" ]]; then
    PRESCRIPT_ARGS+=(-preScript SetImageBase.java "${IMAGE_BASE}")
fi
# When STRIP_PROTECTOR_SECTIONS=1, drop .o1} / .2F; / .1M@ before
# auto-analysis. Use this when importing the Scylla dump, where those
# sections are the protector's scratch/compressed payload and feeding
# them to the auto-analyzer balloons the analysis time to hours.
if [[ "${STRIP_PROTECTOR_SECTIONS:-0}" -eq 1 ]]; then
    PRESCRIPT_ARGS+=(-preScript StripProtectorSections.java)
fi

LOG_FILE="${OUTPUT_DIR}/ghidra_headless_${PROJECT_NAME}.log"

if [[ "${ALREADY_IMPORTED}" -eq 0 ]]; then
    echo "[run_headless] importing ${TARGET_BINARY} into project ${PROJECT_NAME}"
    if [[ -n "${IMAGE_BASE}" ]]; then
        echo "[run_headless] will rebase to ${IMAGE_BASE} via SetImageBase pre-script"
    fi
    "${GHIDRA_HOME}/support/analyzeHeadless" \
        "${PROJECT_DIR}" "${PROJECT_NAME}" \
        -import "${TARGET_BINARY}" \
        -loader PeLoader \
        -scriptPath "${SCRIPT_DIR}" \
        "${PRESCRIPT_ARGS[@]}" \
        -postScript "${SCRIPT_NAME}" "$@" \
        -log "${LOG_FILE}"
else
    echo "[run_headless] reusing existing project ${PROJECT_NAME}; running ${SCRIPT_NAME}"
    "${GHIDRA_HOME}/support/analyzeHeadless" \
        "${PROJECT_DIR}" "${PROJECT_NAME}" \
        -process "${TARGET_NAME}" \
        -noanalysis \
        -scriptPath "${SCRIPT_DIR}" \
        -postScript "${SCRIPT_NAME}" "$@" \
        -log "${LOG_FILE}"
fi

echo "[run_headless] done. Log: ${LOG_FILE}"
