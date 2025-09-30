@REM TODO: ALlow config to be set
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=install
cmake --build build --config Release
cmake --install build --config Release