savedcmd_code.mod := printf '%s\n'   code.o | awk '!x[$$0]++ { print("./"$$0) }' > code.mod
