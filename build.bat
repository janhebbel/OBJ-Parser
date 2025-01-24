@echo off

if not exist bin mkdir bin
pushd bin

:: Debug
set compile_options=-c -nologo -std:c++14 -D DEBUG=1 /diagnostics:caret -EHa- -FC -Zi -Od
set compile_options=%compile_options% -W4 -WX -wd4201 -wd4090 -wd4100 -wd4127 -external:W0
set compile_options=%compile_options% -I..\inc
set link_options=-nologo -debug:full -incremental:no -subsystem:console -OUT:parse.exe

cl.exe %compile_options% ..\main.cpp
link.exe main.obj %link_options% user32.lib
:: radlink.exe main.obj %link_options% user32.lib

popd
