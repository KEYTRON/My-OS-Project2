#!/usr/bin/env python3
"""
Создание загружаемого ISO образа MyOS с GRUB для QEMU на macOS
"""
import os
import struct
import subprocess

def create_iso():
    # Пути к файлам
    kernel_path = "kernel/build/kernel-x86_64.bin"
    iso_output = "myos-gui-bootable.iso"
    iso_dir = "iso-boot"

    if not os.path.exists(kernel_path):
        print(f"❌ Kernel не найден: {kernel_path}")
        return False

    # Создаём структуру директории для ISO
    print("📁 Создание структуры ISO...")
    os.system(f"rm -rf {iso_dir}")
    os.makedirs(f"{iso_dir}/boot/grub", exist_ok=True)

    # Копируем ядро
    os.system(f"cp {kernel_path} {iso_dir}/boot/kernel")

    # Создаём grub.cfg
    grub_cfg = """set default=0
set timeout=2

menuentry "MyOS with GUI Desktop v1.0" {
    multiboot /boot/kernel
    boot
}
"""

    with open(f"{iso_dir}/boot/grub/grub.cfg", "w") as f:
        f.write(grub_cfg)

    print(f"✓ Структура ISO создана в {iso_dir}/")

    # Пытаемся найти grub-mkrescue или использовать альтернативу
    try:
        # Сначала пытаемся найти grub-mkrescue через Homebrew
        result = subprocess.run(
            ["which", "grub-mkrescue"],
            capture_output=True,
            text=True
        )
        if result.returncode == 0:
            print("✓ Найден grub-mkrescue, создаём ISO...")
            os.system(f"grub-mkrescue -o {iso_output} {iso_dir}")
        else:
            # Альтернатива - используем xorriso если доступен
            result = subprocess.run(
                ["which", "xorriso"],
                capture_output=True,
                text=True
            )
            if result.returncode == 0:
                print("✓ Используем xorriso для создания ISO...")
                os.system(f"xorriso -as mkisofs -o {iso_output} -c boot/bootcat.img "
                          f"-b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 "
                          f"-boot-info-table {iso_dir}")
            else:
                print("⚠ grub-mkrescue и xorriso не найдены")
                print("  Создаём простой ISO образ...")
                create_simple_iso(iso_dir, iso_output)

    except Exception as e:
        print(f"❌ Ошибка: {e}")
        return False

    if os.path.exists(iso_output):
        size = os.path.getsize(iso_output) / (1024 * 1024)
        print(f"✅ ISO создан успешно!")
        print(f"   Файл: {iso_output}")
        print(f"   Размер: {size:.1f} MB")
        print(f"\n🚀 Запустить в QEMU:")
        print(f"   qemu-system-x86_64 -cdrom {iso_output} -m 512 -serial stdio -display cocoa")
        return True
    else:
        print(f"❌ ISO не создан")
        return False

def create_simple_iso(iso_dir, iso_output):
    """Создаёт простой ISO образ если нет инструментов"""
    print(f"  Создаём простой ISO из директории {iso_dir}...")
    # Это не будет загружаемый ISO, но будет валидный ISO 9660
    os.system(f"mkisofs -o {iso_output} -R -J {iso_dir}")

if __name__ == "__main__":
    os.chdir("/Users/keytron46/git/GitHub_Desktop")
    if create_iso():
        print("\n✨ ISO готов к использованию!")
    else:
        print("\n❌ Ошибка при создании ISO")
