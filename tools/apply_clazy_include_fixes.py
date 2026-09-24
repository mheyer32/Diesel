#!/usr/bin/env python3
"""Parse tools/clazy_report.log and apply -Wnonportable-include-path fixes."""

from __future__ import annotations

import re
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LOG = ROOT / "tools" / "clazy_report.log"


def extract_include_fixes(text: str) -> dict[str, str]:
    """
    Map wrong include token -> correct token (including quote/angle chars).

    Clang format:
      warning: non-portable path to file '"Image.h"'; ... on disk
         46 | #include "image.h"
            |          ^~~~~~~~~
            |          "Image.h"
    The warning names the on-disk spelling; the #include line is wrong.
    """
    fixes: dict[str, str] = {}
    lines = text.splitlines()
    for i, line in enumerate(lines):
        if "non-portable path to file" not in line:
            continue
        # Correct (on-disk) spelling from the warning text
        m = re.search(r"non-portable path to file '([\"<][^\">]+[\">])'", line)
        if not m:
            continue
        correct = m.group(1)
        wrong = None
        for j in range(i + 1, min(i + 6, len(lines))):
            im = re.search(r"\|\s*#include\s+([\"<][^\">]+[\">])", lines[j])
            if im:
                wrong = im.group(1)
                break
        if not wrong or wrong == correct:
            continue
        if wrong.strip("\"<>").lower() != correct.strip("\"<>").lower():
            continue
        fixes[wrong] = correct
    return fixes


def iter_source_files():
    for root in (ROOT / "diesel", ROOT / "dxshell" / "include"):
        for path in root.rglob("*"):
            if path.suffix.lower() in {".h", ".hh", ".hpp", ".c", ".cpp"}:
                yield path


def apply_fixes(fixes: dict[str, str]) -> int:
    edits = 0
    for wrong, correct in sorted(fixes.items()):
        old = "#include " + wrong
        new = "#include " + correct
        for path in iter_source_files():
            try:
                text = path.read_text(encoding="utf-8", errors="replace")
            except OSError:
                continue
            if old not in text:
                continue
            path.write_text(text.replace(old, new), encoding="utf-8")
            edits += 1
            print(f"OK {path.relative_to(ROOT).as_posix()}: {wrong} -> {correct}")
    return edits


def main() -> None:
    text = LOG.read_text(encoding="utf-8", errors="replace")
    fixes = extract_include_fixes(text)
    print(f"Unique include spelling fixes from log: {len(fixes)}")
    for o, n in sorted(fixes.items(), key=lambda kv: kv[0].lower()):
        print(f"  {o} -> {n}")

    types = Counter()
    for line in text.splitlines():
        m = re.search(r"\[-Wclazy-([^\]]+)\]", line)
        if m:
            types[m.group(1)] += 1
        if "non-portable path" in line:
            types["nonportable-include-path"] += 1
    print("\nWarning tallies:")
    for k, v in types.most_common():
        print(f"  {v:5} {k}")

    print("\nApplying include fixes...")
    n = apply_fixes(fixes)
    print(f"\nEdited {n} files")


if __name__ == "__main__":
    main()
