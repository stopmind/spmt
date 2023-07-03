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

## Компиляция и установка

Что бы сгенерировать make файл, выполните:
```shell
cmake .
```
Так же вы можете указать некоторые пути (не указывать в конце пути `/`) опциями:
- `-DCONF_DIR` путь до директории с настройками, по умолчанию `/etc`.
- `-DTEMP_DIR` путь до temp дериктории, по умолчанию `/tmp`.
- `-DDATA_DIR` путь до директории в котрой можно хранить информацию, по умолчанию `/var`.
- `-DINSTALL_PREFIX` путь до дериктории для установки, по умолчанию `/`.

Для компиляции:
```shell
make
```

### Установка
#### Не рекомендуемый способ:
```shell
make install
```
Так же CMake поддерживает DESTDIR, для изменения пути установки, пример:
```shell
DESTDIR=/usr make install
```
#### Рекомендуемый способ:
Создайте пакет `spmt`:  
```shell
DESTDIR=$(pwd)/spmtpkg/root make install
tar --zstd -C spmtpkg -cf spmt.sppkg root scripts info.json
```

Установите его:
```shell
sudo ./spmtpkg/root/bin/spmt -S spmt.sppkg
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