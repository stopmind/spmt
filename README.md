# SPMT

**spmt** - **S**imple **P**ackage **M**anagement **T**ool

## Зависимости
1. bsdtar

## Пример
Для примера работы пакетного менеджера создадим hello пакет из файлов hellopkg.
```shell
bsdtar -c -f hello.cpio --format cpio -C hellopkg root scripts info.json
```
Вы получите файл `hello.cpio`, его можно установить с помощью:
```shell
sudo spmt -S hello.cpio
```