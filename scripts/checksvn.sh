#!/bin/sh

ufind . -type f -name \*.java    \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.cs      \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.txt     \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.c       \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.h       \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.py      \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.xml     \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.etch    \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.etchinc \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.vm      \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.kwd     \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.sh      \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.bash    \! -exec sh scripts/checkeol.sh \{} \; -print
ufind . -type f -name \*.bat     \! -exec sh scripts/checkeol.sh \{} \; -print
