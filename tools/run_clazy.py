#!/usr/bin/env python3
"""
Run clazy-standalone over Diesel / DXShell sources and write a report.

Designed for Windows + Qt Creator's bundled clazy. System headers (Windows
Kits / MSVC) are passed as -isystem and also matched by --ignore-dirs so
diagnostics from those trees are suppressed.

Usage (from repo root):
  python tools/run_clazy.py
  python tools/run_clazy.py --limit 20
  python tools/run_clazy.py --checks level0,ifndef-define-typo
"""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

DEFAULT_CLAZY_CANDIDATES = [
    Path(r"C:\Qt\qtcreator-20.0.1\bin\clang\bin\clazy-standalone.exe"),
    Path(r"C:\Qt\Tools\QtCreator\bin\clang\bin\clazy-standalone.exe"),
]

# Non-Qt-ish checks that still apply to this codebase, plus level0.
# Qt-specific checks are harmless (they simply won't match).
DEFAULT_CHECKS = ",".join(
    [
        "level0",
        "ifndef-define-typo",
        "raw-environment-function",
        "rule-of-two-soft",
        "returning-data-from-temporary",
        "reserve-candidates",
        "unneeded-cast",
        "virtual-call-ctor",
        "skipped-base-method",
        "copyable-polymorphic",
    ]
)

IGNORE_DIRS = r"Windows Kits|MSVC|Microsoft Visual Studio|ucrt|shared\\um|\\um\\|Windows SDK|WinSDK"
HEADER_FILTER = r"[\\/](diesel|dxshell)[\\/]"

SOURCE_GLOBS = [
    "diesel/**/*.cpp",
    "dxshell/include/**/*.cpp",
]


def find_clazy(explicit: str | None) -> Path:
    if explicit:
        p = Path(explicit)
        if not p.is_file():
            sys.exit(f"clazy not found: {p}")
        return p
    env = os.environ.get("CLAZY_STANDALONE")
    if env and Path(env).is_file():
        return Path(env)
    for c in DEFAULT_CLAZY_CANDIDATES:
        if c.is_file():
            return c
    # PATH
    from shutil import which

    w = which("clazy-standalone")
    if w:
        return Path(w)
    sys.exit(
        "clazy-standalone not found. Set CLAZY_STANDALONE or pass --clazy PATH"
    )


def newest_subdir(parent: Path) -> Path | None:
    if not parent.is_dir():
        return None
    kids = [p for p in parent.iterdir() if p.is_dir()]
    if not kids:
        return None
    return sorted(kids, key=lambda p: p.name)[-1]


def detect_msvc_include() -> Path | None:
    base = Path(r"C:\Program Files\Microsoft Visual Studio")
    if not base.is_dir():
        return None
    # .../18/Community/VC/Tools/MSVC/<ver>/include
    for edition in ("Community", "Professional", "Enterprise", "BuildTools"):
        for ver in sorted((base.iterdir()), key=lambda p: p.name, reverse=True):
            msvc = ver / edition / "VC" / "Tools" / "MSVC"
            newest = newest_subdir(msvc)
            if newest and (newest / "include").is_dir():
                return newest / "include"
    return None


def detect_winsdk_includes() -> list[Path]:
    kits = Path(r"C:\Program Files (x86)\Windows Kits\10\Include")
    ver = newest_subdir(kits)
    if not ver:
        return []
    out = []
    for name in ("ucrt", "shared", "um", "winrt"):
        p = ver / name
        if p.is_dir():
            out.append(p)
    return out


def project_include_dirs() -> list[Path]:
    dirs = [
        ROOT / "dxshell" / "include",
        ROOT / "diesel",
    ]
    for arch in ("_install-x64", "_install", "_install-x86"):
        p = ROOT / "3rdParty" / arch / "include"
        if p.is_dir():
            dirs.append(p)
            break
    return dirs


def collect_sources() -> list[Path]:
    files: list[Path] = []
    for pattern in SOURCE_GLOBS:
        files.extend(ROOT.glob(pattern))
    # Skip vendored GL extension headers' companions if any .cpp under gl/
    files = [f for f in files if "dxshell/include/gl" not in f.as_posix()]
    return sorted(set(files), key=lambda p: p.as_posix().lower())


def build_compile_args() -> list[str]:
    args: list[str] = [
        "-std=c++14",
        "-fms-extensions",
        "-fms-compatibility",
        "-fms-compatibility-version=19.30",
        "-DWIN32",
        "-D_WIN32",
        "-DUNICODE",
        "-D_UNICODE",
        "-DNOMINMAX",
        "-D_CRT_SECURE_NO_WARNINGS",
        "-Wno-microsoft-extra-qualification",
        "-Wno-invalid-friend",
        "-Wnonportable-include-path",
        "-Wno-error",
    ]
    for d in project_include_dirs():
        args.append(f"-I{d}")
    # System headers: -isystem so clang/clazy treat them as system
    msvc = detect_msvc_include()
    if msvc:
        args.append(f"-isystem{msvc}")
    for d in detect_winsdk_includes():
        args.append(f"-isystem{d}")
    return args


def run_one(
    clazy: Path,
    source: Path,
    checks: str,
    compile_args: list[str],
    timeout: int,
) -> tuple[int, str]:
    cmd = [
        str(clazy),
        f"--checks={checks}",
        f"--header-filter={HEADER_FILTER}",
        f"--ignore-dirs={IGNORE_DIRS}",
        str(source),
        "--",
        *compile_args,
    ]
    try:
        proc = subprocess.run(
            cmd,
            cwd=ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=timeout,
        )
    except subprocess.TimeoutExpired as e:
        out = (e.stdout or "") + (e.stderr or "")
        return 124, out + f"\n[timeout after {timeout}s]\n"
    out = (proc.stdout or "") + (proc.stderr or "")
    return proc.returncode, out


def filter_noise(text: str) -> str:
    """Drop lines that still escaped from system headers / fatal noise we don't fix here."""
    keep: list[str] = []
    skip_block = False
    for line in text.splitlines():
        low = line.lower().replace("/", "\\")
        if any(
            s in low
            for s in (
                "windows kits",
                "\\msvc\\",
                "\\ucrt\\",
                "\\shared\\",
                "program files\\microsoft visual studio",
            )
        ):
            # keep the note but mark — usually we drop
            continue
        keep.append(line)
    return "\n".join(keep).strip()


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--clazy", help="Path to clazy-standalone")
    ap.add_argument("--checks", default=DEFAULT_CHECKS)
    ap.add_argument(
        "--log",
        type=Path,
        default=ROOT / "tools" / "clazy_report.log",
    )
    ap.add_argument("--limit", type=int, default=0, help="Max files (0=all)")
    ap.add_argument("--timeout", type=int, default=120, help="Per-file seconds")
    ap.add_argument(
        "--keep-fatal",
        action="store_true",
        help="Also keep full fatal-error output (default: still logged)",
    )
    args = ap.parse_args()

    clazy = find_clazy(args.clazy)
    sources = collect_sources()
    if args.limit > 0:
        sources = sources[: args.limit]

    compile_args = build_compile_args()
    args.log.parent.mkdir(parents=True, exist_ok=True)

    print(f"clazy:   {clazy}")
    print(f"sources: {len(sources)}")
    print(f"checks:  {args.checks}")
    print(f"log:     {args.log}")
    print(f"includes:{project_include_dirs()}")
    print(f"msvc:    {detect_msvc_include()}")
    print(f"winsdk:  {detect_winsdk_includes()}")

    summary_hits: list[str] = []
    ok = fail = 0
    t0 = time.time()

    with args.log.open("w", encoding="utf-8") as log:
        log.write(f"# clazy report generated {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        log.write(f"# clazy={clazy}\n")
        log.write(f"# checks={args.checks}\n")
        log.write(f"# header-filter={HEADER_FILTER}\n")
        log.write(f"# ignore-dirs={IGNORE_DIRS}\n")
        log.write(f"# compile_args={' '.join(compile_args)}\n\n")

        for i, src in enumerate(sources, 1):
            rel = src.relative_to(ROOT).as_posix()
            print(f"[{i}/{len(sources)}] {rel} ...", flush=True)
            code, out = run_one(clazy, src, args.checks, compile_args, args.timeout)
            cleaned = filter_noise(out)
            log.write("=" * 78 + "\n")
            log.write(f"FILE: {rel}\n")
            log.write(f"EXIT: {code}\n")
            log.write("-" * 78 + "\n")
            if cleaned:
                log.write(cleaned + "\n")
                # Collect diagnostic lines for a summary appendix
                for line in cleaned.splitlines():
                    if re.search(r"warning:|error:|note:", line):
                        summary_hits.append(f"{rel}: {line}")
            else:
                log.write("(no project diagnostics)\n")
            log.write("\n")
            log.flush()
            if code == 0 and not re.search(r"\berror:", cleaned):
                ok += 1
            else:
                fail += 1

        log.write("=" * 78 + "\n")
        log.write("SUMMARY DIAGNOSTICS\n")
        log.write("=" * 78 + "\n")
        for h in summary_hits:
            log.write(h + "\n")
        log.write(
            f"\n# done files={len(sources)} okish={ok} with_errors={fail} "
            f"diag_lines={len(summary_hits)} elapsed={time.time()-t0:.1f}s\n"
        )

    print(
        f"Finished. okish={ok} with_errors={fail} "
        f"diag_lines={len(summary_hits)} -> {args.log}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
