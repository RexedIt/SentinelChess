clear
scons platform=windows
del src\*.obj | Out-Null
del ..\SentinelLib\*.obj | Out-Null
copy-item "demo\bin\*.*" "..\..\godot\SentinelChess\bin"
