clear
scons dev_build=yes target=template_release disable_exceptions=false arch=x86_64 platform=windows
del src\*.o*j | Out-Null
del ..\SentinelLib\*.o* | Out-Null
copy-item "demo\bin\*.*" "..\..\godot\SentinelChess\bin"
