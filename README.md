# SPMT

**spmt** - **S**imple **P**ackage **M**anagement **T**ool

------

## Зависимости
- tar (с поддержкой ztsd)
- nlohmann/json

## Пример
Для примера работы пакетного менеджера создадим hello пакет.
Для этого запустите эту коман
```shell
tar --zstd -C hellopkg -cf hello.sppkg root scripts info.json
```
Вы получите файл `hello.cpio`, его можно установить с помощью:
```shell
sudo spmt -S hello.sppkg
```

## Устройство пакета
Пакет это tar архив сжатый с помощью zstd.
В пакете есть папки: `root` и `scripts`, а так же файл `info.json`.
В `scripts` есть скрипты `preinst` и `postinst`, заупскающиеся до и после копирования файлов.
В `root` содержится катологов которое сопируется в root.
`info.json` содержит информацию о пакете:
- `name` имя пакета 
- `require` требуемые пакеты
- `conflicts` конфликтуещие пакеты
- `version` версия, например `[0, 0, 0]`