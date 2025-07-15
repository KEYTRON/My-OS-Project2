; boot.asm
BITS 16
ORG 0x7C00

start:
    mov ax, 0x07C0       ; Настройка стека
    mov ss, ax
    mov sp, 0x7C00

    ; Загрузка ядра
    call load_kernel

    ; Переключение в защищённый режим
    cli                  ; Отключаем прерывания
    lgdt [gdt_descriptor] ; Загружаем GDT
    mov eax, cr0         ; Читаем CR0
    or eax, 1            ; Включаем защищённый режим
    mov cr0, eax         ; Записываем CR0
    jmp 0x08:protected_mode ; Переход в защищённый режим

gdt_descriptor:
    dw gdt_end - gdt - 1  ; Длина GDT
    dd gdt                ; Адрес GDT

gdt:
    dq 0x0000000000000000  ; Нулевой сегмент
    dq 0x00CF9A000000FFFF  ; Кодовый сегмент
    dq 0x00CF92000000FFFF  ; Сегмент данных

gdt_end:

load_kernel:
    ; Чтение ядра (предполагаем, что оно начинается с 2-го сектора)
    mov bx, 0x1000        ; Адрес в памяти для ядра
    mov dh, 15            ; Количество секторов (подправь под размер ядра)
    call read_sectors     ; Чтение секторов с диска

    ret                   ; Возвращаемся

read_sectors:
    mov ah, 0x02          ; BIOS функция для чтения секторов
    mov al, dh            ; Количество секторов
    mov ch, 0x00          ; Дорожка
    mov dh, 0x00          ; Головка
    mov dl, 0x80          ; Диск (0x80 — первый жёсткий диск)
    int 0x13              ; Вызов прерывания BIOS
    jc disk_error         ; Если ошибка, переход на обработку ошибки
    ret                   ; Возвращаемся, если всё нормально

disk_error:
    ; Обработка ошибки (например, можно вывести сообщение)
    mov si, error_msg
    call print_string
    hlt                   ; Остановка системы

[bits 32]
protected_mode:
    mov ax, 0x10          ; Дескриптор сегмента данных
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Передача управления ядру
    jmp 0x100000          ; Переход к ядру

hang:
    jmp hang              ; Ожидание

print_string:
    mov ah, 0x0E
.next_char:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .next_char
.done:
    ret

error_msg db "Disk read error!", 0

times 510-($-$$) db 0     ; Дополняем до 512 байт
dw 0xAA55                 ; Сигнатура загрузочного сектора

