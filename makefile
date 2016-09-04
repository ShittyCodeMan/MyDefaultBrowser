mdb.exe: mdb.obj
	link mdb.obj /nologo /FIXED kernel32.lib user32.lib shell32.lib
mdb.obj: mdb.c
	cl mdb.c /c /nologo /O1 /GS- /Oi-
