clear
scons dev_build=yes platform=windows
#scons target=template_release arch=x86_64
del src\*.obj | Out-Null
del ..\SentinelLib\*.obj | Out-Null
copy-item "demo\bin\*.*" "..\..\godot\SentinelChess\bin"
