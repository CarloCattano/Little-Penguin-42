savedcmd_listmounts.mod := printf '%s\n'   listmounts.o | awk '!x[$$0]++ { print("./"$$0) }' > listmounts.mod
