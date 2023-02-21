del *.obj Buildlog.htm *.manifest *.manifest.res *.pch /s
del *.pdb *.dep *.idb *.res *.user /s
del *.ncb *.sbr *.ilk *.exp *.bsc /s
del *.sdf *.aps *.ipch *.suo *.vcproj *.cd *.scc /s
del /s /q /f /a *.bak *.suo

rmdir /s /q x64
rmdir /s /q Debug
rmdir /s /q Release
rmdir /s /q ipch
