mdb.exe: mdb.c
	cl /O1 /GS- mdb.c kernel32.lib user32.lib shell32.lib
