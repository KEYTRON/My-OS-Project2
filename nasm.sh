#!/bin/bash
# NASM wrapper to run Windows nasm.exe from WSL, converting WSL paths to Windows paths.

args=()
i=1
while [ $i -le $# ]; do
    eval a=\"\${$i}\"
    case "$a" in
        -o)
            i=$((i + 1))
            eval out=\"\${$i}\"
            if [ -n "$out" ]; then
                if [ -e "$out" ] || [ -e "$(dirname "$out")" ]; then
                    out=$(wslpath -w "$out")
                fi
                args+=("-o" "$out")
            else
                args+=("-o")
            fi
            ;;
        -*)
            args+=("$a")
            ;;
        *)
            if [ -e "$a" ]; then
                args+=("$(wslpath -w "$a")")
            else
                args+=("$a")
            fi
            ;;
    esac
    i=$((i + 1))
done

"/mnt/c/Program Files/NASM/nasm.exe" "${args[@]}"
