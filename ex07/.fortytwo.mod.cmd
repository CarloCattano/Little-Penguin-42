savedcmd_fortytwo.mod := printf '%s\n'   fortytwo.o | awk '!x[$$0]++ { print("./"$$0) }' > fortytwo.mod
