@echo off
cloc . --by-file --exclude-dir=lib,inc,cmake-build-debug,cmake-build-release,.idea,.git,highlighting,benchmark,json --not-match-f="CMakeLists.txt|build.sh|scc" --quiet --report-file=scc.txt