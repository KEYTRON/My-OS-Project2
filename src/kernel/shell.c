#include "shell.h"
#include "keyboard.h"
#include "vga.h"
#include "fs.h"

static int str_eq(const char* a, const char* b)
{
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static void shell_help() {
    print_string("Available commands:\n");
    print_string("help - show this message\n");
    print_string("ls   - list files\n");
    print_string("cat <name> - display file\n");
    print_string("clear - clear screen\n");
    print_string("draw  - draw pattern\n");
}

static void draw_demo() {
    clear_graphics(0);
    for (int y = 0; y < 200; y++) {
        for (int x = 0; x < 320; x++) {
            if ((x + y) % 20 == 0) {
                put_pixel(x, y, 12);
            }
        }
    }
}

void shell() {
    char buf[128];
    while (1) {
        print_string("\n$ ");
        read_line(buf, sizeof(buf));

        if (buf[0] == '\0') continue;
        if (str_eq(buf, "help")) {
            shell_help();
        } else if (str_eq(buf, "ls")) {
            fs_list();
        } else if (str_eq(buf, "clear")) {
            clear_screen();
        } else if (str_eq(buf, "draw")) {
            draw_demo();
        } else if (str_eq(buf, "exit")) {
            break;
        } else if (buf[0]=='c' && buf[1]=='a' && buf[2]=='t' && buf[3]==' ') {
            const char *data = fs_read(buf + 4);
            if (data) {
                print_string(data);
                print_string("\n");
            } else {
                print_string("Not found\n");
            }
        } else {
            print_string("Unknown command. Type 'help'\n");
        }
    }
}
