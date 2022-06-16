set MSDevDir=C:\Program Files\DevStudio\SharedIDE
set MSVCDir=C:\Program Files\DevStudio\VC
path=c:\windows;c:\windows\command;C:\Progra~1\DevStu~1\VC\bin;C:\Progra~1\DevStu~1\Shared~1\bin
prep Debug\test_3d.exe
profile Debug\test_3d.exe
prep /M Debug\test_3d
plist Debug\test_3d >profileout.txt
