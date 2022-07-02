call D:\software\vs_2019\Community\VC\Auxiliary\Build\vcvars32.bat
rem call cl.exe /ifcOutput ".\Debug\" /GS /analyze- /W3 /Gy- /Zc:wchar_t /I".." /Zi /Gm /Od /Fd".\Debug\" /Zc:inline /fp:precise /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_VC80_UPGRADE=0x0600" /D "_MBCS" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /MDd /FC /Fa".\Debug\" /EHsc /nologo /Fo".\Debug\" /Fp".\Debug\Test.pch" /diagnostics:column  /Gh  main.cpp

rem /OUT:".\..\Debug\Test.exe" /MANIFEST /NXCOMPAT /PDB:".\..\Debug\test.pdb" /DYNAMICBASE "odbc32.lib" "odbccp32.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" /DEBUG /MACHINE:X86 /SAFESEH /INCREMENTAL /PGD:".\..\Debug\test.pgd" /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:".\..\Debug\test.exe.intermediate.manifest" /LTCGOUT:".\..\Debug\test.iobj" /ERRORREPORT:PROMPT /ILK:".\..\Debug\test.ilk" /NOLOGO /TLBID:1 

rem pause
rem call cl.exe /Gh /DYNAMICBASE "CallMon.lib"  main.cpp
rem call main.exe

:loop1
cls

rm main.exe
rem call cl.exe /D "WIN32" /Gh /DYNAMICBASE "CallMon.lib"  main.cpp
rem call cl.exe /GS /analyze- /W3 /Gy- /Zc:wchar_t /Zi /Gm /Od Zc:inline /fp:precise /D "WIN32" main.cpp

cl.exe /ifcOutput ".\Debug\" /GS /analyze- /W3 /Gy- /Zc:wchar_t /I".." /Zi /Gm /Od /Fd".\Debug\" /Zc:inline /fp:precise /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_VC80_UPGRADE=0x0600" /D "_MBCS" /errorReport:prompt /WX- /Zc:forScope /Gd /Oy- /MDd /FC /Fa".\Debug\" /EHsc /nologo /Fo".\Debug\" /Fp".\Debug\Test.pch" /diagnostics:column  /Gh  main.cpp

main.exe

pause
goto loop1