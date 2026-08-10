#!/usr/bin/env bash
# Refresh this project's .cursor/rules mirror from a shared upstream tree.
#
# Upstream is chosen by the first existing "$HOME/<candidate>/.cursor/rules":
#   1. dev/synesissoftware/forks/freelibs
#   2. dev/synesissoftware/freelibs
#   3. dev/sis/SISTrS/SISTrS
#   4. dev/sis/SISTrS
#
# Local policy file upstream-source-of-truth.mdc is preserved.
# Local elaborations named local-*.mdc are preserved.
# Other *.mdc entries are replaced with symlinks to absolute upstream paths
# (not copies; safe if this project directory moves).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEST="${ROOT}/.cursor/rules"
META_RULE="upstream-source-of-truth.mdc"
STAMP="${DEST}/.upstream-source"

is_preserved_local_rule() {
  local base="$1"
  [[ "${base}" == "${META_RULE}" ]] && return 0
  [[ "${base}" == local-*.mdc ]] && return 0
  return 1
}

CANDIDATES=(
  "dev/synesissoftware/forks/freelibs"
  "dev/synesissoftware/freelibs"
  "dev/sis/SISTrS/SISTrS"
  "dev/sis/SISTrS"
)

source_rules=""
source_home_rel=""
for rel in "${CANDIDATES[@]}"; do
  candidate="${HOME}/${rel}/.cursor/rules"
  if [[ -d "${candidate}" ]]; then
    source_rules="${candidate}"
    source_home_rel="${rel}"
    break
  fi
done

if [[ -z "${source_rules}" ]]; then
  echo "error: no upstream .cursor/rules found under \$HOME among:" >&2
  for rel in "${CANDIDATES[@]}"; do
    echo "  \$HOME/${rel}/.cursor/rules" >&2
  done
  exit 1
fi

# Replace a whole-directory symlink from older setups with a real directory.
if [[ -L "${DEST}" ]]; then
  rm "${DEST}"
fi
mkdir -p "${DEST}"

# Drop previous mirror links/files except preserved local policy/elaborations.
shopt -s nullglob
for path in "${DEST}"/* "${DEST}"/.*; do
  base="$(basename "${path}")"
  case "${base}" in
    .|..|.upstream-source) continue ;;
  esac
  if is_preserved_local_rule "${base}"; then
    if [[ "${base}" == local-*.mdc ]]; then
      preserved_locals+=("${base}")
    fi
    continue
  fi
  # Only remove symlinks or stale plain mirrors we created; refuse unknowns.
  if [[ -L "${path}" ]]; then
    rm "${path}"
  elif [[ -f "${path}" && "${base}" == *.mdc ]]; then
    echo "error: refusing to delete non-symlink rule: ${path}" >&2
    echo "rename to local-*.mdc, move it aside, or convert the mirror manually, then re-run." >&2
    exit 1
  fi
done
shopt -u nullglob

linked=0
for src in "${source_rules}"/*.mdc; do
  [[ -e "${src}" ]] || continue
  base="$(basename "${src}")"
  if is_preserved_local_rule "${base}"; then
    echo "note: skipping upstream file that would shadow local ${base}"
    continue
  fi
  # Absolute target so the project can move without breaking the link.
  abs_src="$(cd "$(dirname "${src}")" && pwd)/$(basename "${src}")"
  ln -s "${abs_src}" "${DEST}/${base}"
  linked=$((linked + 1))
done

{
  echo "upstream_home_relative=${source_home_rel}"
  echo "upstream_rules=${source_rules}"
  echo "synced_at_utc=$(date -u +"%Y-%m-%dT%H:%M:%SZ")"
  echo "linked_mdc_count=${linked}"
} > "${STAMP}"

echo "Synced ${linked} rule symlink(s) from \$HOME/${source_home_rel}"
echo "  ${source_rules}"
echo "  -> ${DEST}"
echo "Preserved local policy: ${DEST}/${META_RULE}"
if ((${#preserved_locals[@]})); then
  echo "Preserved local elaboration(s):"
  for base in "${preserved_locals[@]}"; do
    echo "  ${DEST}/${base}"
  done
fi
