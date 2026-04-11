add_executable(CHIP8Emulator
        chip8.cpp)
target_sources(CHIP8Emulator PRIVATE main.cpp)
target_link_libraries(CHIP8Emulator PRIVATE vendor)