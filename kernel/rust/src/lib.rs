//! myos_rust — Rust-часть ядра MyOS: интерактивная командная оболочка.
//!
//! Собирается как staticlib для таргета x86_64-unknown-none и линкуется
//! в ядро. Использует C-интерфейсы ядра (консоль, клавиатура, таймер)
//! и кучу kmalloc/kfree, реализованную в C++ (lib/cxx/kheap.cpp).

#![no_std]

extern crate alloc;

use alloc::string::String;
use alloc::vec::Vec;
use core::alloc::{GlobalAlloc, Layout};
use core::fmt::{self, Write};

// ---- Интерфейсы, предоставляемые C/C++ частью ядра ----
extern "C" {
    fn kputchar(c: u8);
    fn keyboard_getchar() -> i32;
    fn timer_ticks() -> u64;
    fn kheap_stats(total: *mut u64, used: *mut u64, free_bytes: *mut u64);
    fn kmalloc_aligned(size: usize, align: usize) -> *mut u8;
    fn kfree(ptr: *mut u8);
}

// ---- Глобальный аллокатор поверх кучи ядра (C++) ----

struct KernelAllocator;

unsafe impl GlobalAlloc for KernelAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        kmalloc_aligned(layout.size().max(1), layout.align())
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: Layout) {
        kfree(ptr);
    }
}

#[global_allocator]
static ALLOCATOR: KernelAllocator = KernelAllocator;

#[panic_handler]
fn panic(info: &core::panic::PanicInfo) -> ! {
    let _ = write!(Console, "\n[rust] KERNEL PANIC: {}\n", info);
    loop {
        halt();
    }
}

// ---- Простейшая консоль поверх kputchar ----

struct Console;

impl Write for Console {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for b in s.bytes() {
            unsafe { kputchar(b) };
        }
        Ok(())
    }
}

macro_rules! kprint {
    ($($arg:tt)*) => {{ let _ = write!(Console, $($arg)*); }};
}

macro_rules! kprintln {
    () => {{ kprint!("\n"); }};
    ($($arg:tt)*) => {{ kprint!($($arg)*); kprint!("\n"); }};
}

fn halt() {
    unsafe { core::arch::asm!("hlt", options(nomem, nostack, preserves_flags)) };
}

fn outb(port: u16, value: u8) {
    unsafe {
        core::arch::asm!("out dx, al", in("dx") port, in("al") value,
                         options(nomem, nostack, preserves_flags));
    }
}

/// Блокирующее чтение символа: ждём прерывания клавиатуры в hlt
fn read_char() -> u8 {
    loop {
        let c = unsafe { keyboard_getchar() };
        if c >= 0 {
            return c as u8;
        }
        halt();
    }
}

/// Читает строку с эхом и редактированием backspace
fn read_line() -> String {
    let mut line = String::new();
    loop {
        let c = read_char();
        match c {
            b'\n' => {
                kprintln!();
                return line;
            }
            0x08 | 0x7f => {
                if line.pop().is_some() {
                    kprint!("\x08");
                }
            }
            0x03 => {
                // Ctrl+C — отменяем ввод
                kprintln!("^C");
                line.clear();
                return line;
            }
            0x20..=0x7e => {
                line.push(c as char);
                unsafe { kputchar(c) };
            }
            _ => {}
        }
    }
}

// ---- Команды оболочки ----

fn cmd_help() {
    kprintln!("Available commands:");
    kprintln!("  help          - show this help");
    kprintln!("  about         - about MyOS");
    kprintln!("  clear         - clear the screen");
    kprintln!("  echo <text>   - print text");
    kprintln!("  uptime        - time since boot");
    kprintln!("  mem           - kernel heap statistics");
    kprintln!("  alloc <n>     - test: allocate n bytes on the heap");
    kprintln!("  langs         - which language runs what");
    kprintln!("  reboot        - reboot the machine");
    kprintln!("  halt          - stop the CPU");
}

fn cmd_about() {
    kprintln!("MyOS v1.0 - hobby operating system (not a UNIX)");
    kprintln!("Kernel languages: ASM (boot/interrupts), C (drivers),");
    kprintln!("C++ (heap allocator), Rust (this shell).");
}

fn cmd_langs() {
    kprintln!("ASM  : boot, long mode switch, ISR stubs (entry.S, isr_stubs.S)");
    kprintln!("C    : VGA/serial/keyboard/timer drivers, GDT/IDT (drivers/, arch/)");
    kprintln!("C++  : kernel heap allocator kmalloc/kfree (lib/cxx/kheap.cpp)");
    kprintln!("Rust : interactive shell + global allocator (rust/src/lib.rs)");
}

fn cmd_clear() {
    // ANSI недоступен; экран чистится C-функцией через "магический" символ
    unsafe { kputchar(0x0c) }; // form feed обрабатывается в kmain как clear
}

fn cmd_uptime() {
    let ticks = unsafe { timer_ticks() };
    let secs = ticks / 100;
    kprintln!(
        "uptime: {}m {}s ({} ticks @ 100 Hz)",
        secs / 60,
        secs % 60,
        ticks
    );
}

fn cmd_mem() {
    let (mut total, mut used, mut free) = (0u64, 0u64, 0u64);
    unsafe { kheap_stats(&mut total, &mut used, &mut free) };
    kprintln!("kernel heap (C++ allocator):");
    kprintln!("  total: {} KiB", total / 1024);
    kprintln!("  used : {} KiB", used / 1024);
    kprintln!("  free : {} KiB", free / 1024);
}

fn cmd_alloc(arg: &str) {
    let n: usize = match arg.trim().parse() {
        Ok(v) => v,
        Err(_) => {
            kprintln!("usage: alloc <bytes>");
            return;
        }
    };
    let mut v: Vec<u8> = Vec::new();
    if v.try_reserve_exact(n).is_err() {
        kprintln!("allocation of {} bytes FAILED", n);
        return;
    }
    v.resize(n, 0xA5);
    // Проверяем, что память реально доступна на запись/чтение
    let ok = v.iter().all(|&b| b == 0xA5);
    kprintln!(
        "allocated {} bytes at {:p}, verify: {}",
        n,
        v.as_ptr(),
        if ok { "OK" } else { "CORRUPT" }
    );
    // v освобождается здесь — память возвращается в кучу
}

fn cmd_reboot() {
    kprintln!("Rebooting...");
    // Импульс reset через контроллер клавиатуры 8042
    outb(0x64, 0xFE);
    loop {
        halt();
    }
}

fn cmd_halt() {
    kprintln!("CPU halted. Bye!");
    loop {
        halt();
    }
}

fn dispatch(line: &str) {
    let line = line.trim();
    if line.is_empty() {
        return;
    }
    let (cmd, arg) = match line.find(' ') {
        Some(i) => (&line[..i], &line[i + 1..]),
        None => (line, ""),
    };
    match cmd {
        "help" | "?" => cmd_help(),
        "about" => cmd_about(),
        "langs" => cmd_langs(),
        "clear" => cmd_clear(),
        "echo" => kprintln!("{}", arg),
        "uptime" => cmd_uptime(),
        "mem" => cmd_mem(),
        "alloc" => cmd_alloc(arg),
        "reboot" => cmd_reboot(),
        "halt" => cmd_halt(),
        _ => kprintln!("unknown command: '{}' (try 'help')", cmd),
    }
}

/// Точка входа оболочки; вызывается из kmain.c после инициализации ядра.
/// Не возвращается.
#[no_mangle]
pub extern "C" fn rust_shell_run() -> ! {
    kprintln!();
    kprintln!("MyOS shell (written in Rust). Type 'help' for commands.");
    loop {
        kprint!("myos> ");
        let line = read_line();
        dispatch(&line);
    }
}

/// Быстрая самопроверка Rust-части (вызывается при старте ядра):
/// проверяет работу аллокатора через C++ кучу.
#[no_mangle]
pub extern "C" fn rust_selftest() -> i32 {
    let mut v: Vec<u32> = Vec::new();
    for i in 0..1024 {
        v.push(i);
    }
    let sum: u32 = v.iter().sum();
    let s = String::from("rust+alloc ok");
    if sum == 523776 && s.len() == 13 {
        0
    } else {
        1
    }
}
