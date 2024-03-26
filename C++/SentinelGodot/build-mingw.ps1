clear
scons dev_build=yes target=template_debug disable_exceptions=false arch=x86_64 platform=windows use_mingw=yes
del src\*.o* | Out-Null
del ..\SentinelLib\*.o* | Out-Null
copy-item "demo\bin\*.*" "..\..\godot\SentinelChess\bin"
