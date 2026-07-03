// cxx_runtime.cpp — минимальный C++ рантайм для freestanding-ядра.
// Компилируемся с -fno-exceptions -fno-rtti, поэтому нужны только
// заглушки для чисто виртуальных вызовов и регистрации деструкторов.
#include <stdint.h>

extern "C" {

// Вызывается при обращении к чисто виртуальной функции
void __cxa_pure_virtual() {
    for (;;) {
        asm volatile("cli; hlt");
    }
}

// Регистрация деструкторов статических объектов — в ядре не выходим
// из "программы", так что просто игнорируем.
int __cxa_atexit(void (*)(void*), void*, void*) {
    return 0;
}

void* __dso_handle = nullptr;

// Guard-переменные для потокобезопасной инициализации локальных статиков.
// Ядро одноядерное без вытеснения на этом этапе — достаточно флага.
int __cxa_guard_acquire(uint64_t* guard) {
    return *reinterpret_cast<uint8_t*>(guard) == 0;
}

void __cxa_guard_release(uint64_t* guard) {
    *reinterpret_cast<uint8_t*>(guard) = 1;
}

void __cxa_guard_abort(uint64_t*) {}

} // extern "C"
