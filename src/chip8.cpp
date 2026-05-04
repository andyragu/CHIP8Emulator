#include <cstdint>
#include <cstring>
#include <fstream>

constexpr unsigned int START_ADDRESS { 0x200 };
constexpr unsigned int FONT_ADDRESS { 0x50 };

constexpr int FONT_SIZE { 80 };
unsigned char font[FONT_SIZE]
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

class Chip8 {
    public:
        uint8_t memory[4096]{};
        uint8_t PC{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint8_t sp{};
        uint8_t registers[16]{};
        uint16_t stack[16]{};
        uint16_t index{};
        uint16_t opcode{};
        uint32_t display[64 * 32]{};

        Chip8() {
            PC = START_ADDRESS;

            for (int i = 0; i < FONT_SIZE; i++) {
                memory[FONT_ADDRESS + i] = font[i];
            }
        }

        void LoadROM(char const* filename);

        // Instructions
        void OP_00E0();
        void OP_1NNN();
        void OP_2NNN();

        void OP_00EE();

        void OP_3XKK();

        void OP_4XKK();

        void OP_5XY0();

        void OP_6XKK();

        void OP_7XKK();

        void OP_8XY0();
};

void Chip8::LoadROM(char const* filename) {

    std::fstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        const int size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; i++) {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
}

// Clear display
void Chip8::OP_00E0() {
    memset(display, 0, sizeof(display));
}

// Jump
void Chip8::OP_1NNN() {
    uint16_t address = opcode & 0x0FFFu;

    PC = address;
}

// Calls subroutine
void Chip8::OP_2NNN() {
    uint16_t address = opcode & 0x0FFFu;

    stack[sp] = PC;
    sp++;

    PC = address;
}

// Return from subroutine
void Chip8::OP_00EE() {
    sp--;
    PC = stack[sp];
}

// Skip next instruction if Vx = kk.
void Chip8::OP_3XKK() {
    // kk or byte - An 8-bit value, the lowest 8 bits of the instruction
    // Chip-8 has 16 general purpose 8-bit registers, usually referred to as Vx, where x is a hexadecimal digit (0 through F)
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte) {
        PC += 2;
    }
}

void Chip8::OP_4XKK() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte) {
        PC += 2;
    }
}

void Chip8::OP_5XY0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy]) {
        PC += 2;
    }
}

void Chip8::OP_6XKK() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

void Chip8::OP_7XKK() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte + Vx;
}

void Chip8::OP_8XY0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}