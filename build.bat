@echo off

if not exist bin mkdir bin
pushd bin

:: Debug
set compile_options=-nologo -std:c++14 -D DEBUG=1 /diagnostics:caret -EHa- -FC -Zi -Od -W4 -WX -wd4201 -wd4090 -wd4100 -wd4127 -external:W0 -I..\inc -Feparse
set link_options=-incremental:no -subsystem:console

cl.exe %compile_options% ..\main.cpp -link %link_options% user32.lib

popd
