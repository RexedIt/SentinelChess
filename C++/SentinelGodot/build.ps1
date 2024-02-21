clear
scons dev_build=yes target=template_debug arch=x86_64 platform=windows
#scons target=template_release arch=x86_64 platform=windows
del src\*.obj | Out-Null
del ..\SentinelLib\*.obj | Out-Null
copy-item "demo\bin\*.*" "..\..\godot\SentinelChess\bin"
