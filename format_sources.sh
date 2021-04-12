#!/bin/bash

find lib dst tests \( -name "*.h" -or -name "*.cc" \) ! -name "Linkdef.h" -exec clang-format --style=file -i {} \;
