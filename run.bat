del /s /q program.exe
clang++ main.cpp isctrl.cpp --target=x86_64-w64-mingw32 -fuse-ld=lld -o program.exe -std=c++26
if exist "program.exe" (
  program
) else (
  echo Error!
)
