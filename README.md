# PCM Encoder

Генератор PCM видео из звуковых файлов.


## Features

* Открывает любые поддерживаемые FFmpeg звуковые файлы (также видео со звуковой дорожкой)
* Кодирование в PCM видеопоток формата PAL и NTSC
* Режим просмотра: дисплей + звук
* Кодирование видео средствами FFmpeg: Возможность указать видеокодек и битрейт


## Сборка

* Клонировать репозиторий

```
git clone https://github.com/ololoshka2871/pcm_coder.git && cd pcm_coder
```

* Клонировать субмодули

```
git submodule update --init --recursive
```

* Создать каталог для сборки и перейти в него

```
mkdir build && cd build
```


### Linux

Необходимые зависимости:

* [SDL2](https://www.libsdl.org/download-2.0.php)
* [portaudio](http://www.portaudio.com/)
* [ffmpeg](http://ffmpeg.org/)
* [cmake](https://cmake.org/)
* Компилятор gcc/clang с поддержкой С++17


* Конфигурирование

```
cmake .. -DCMAKE_BUILD_TYPE=Release
```

* Сборка

```
make
```

* Исполняемый файл находится в подкаталоге src


### Windows

Необходимые зависимости:

* [ffmpeg](http://ffmpeg.org/)
* [cmake](https://cmake.org/)
* [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=BuildTools&rel=16) или
	Visual Studio 2019
* [Visual C Runtime 2019](https://aka.ms/vs/16/release/vc_redist.x64.exe) Только для клиентов


* Распаковать [библиотеки](https://ffmpeg.zeranoe.com/builds/win64/shared/ffmpeg-4.2.3-win64-shared.zip) и 
[хедеры](https://ffmpeg.zeranoe.com/builds/win64/dev/ffmpeg-4.2.3-win64-dev.zip) FFmpeg'а куда угодно, например в каталог ffmpeg

```
ffmpeg
├───bin
├───doc
├───include
│   ├───libavcodec
│   ├───libavdevice
│   ├───libavfilter
│   ├───libavformat
│   ├───libavutil
│   ├───libpostproc
│   ├───libswresample
│   └───libswscale
├───lib
└───presets
```

* Конфигурирование

**-A платформа**
- x64 - 64 битный вариант
- Win32 - 32 битный

```
cmake .. -DCMAKE_BUILD_TYPE=Release -A x64 -DLIBAVCODEC_INCLUDE_DIR=<путь к ffmpeg>/include -DLIBAVDEVICE_INCLUDE_DIR=<путь к ffmpeg>/include -DLIBAVFORMAT_INCLUDE_DIR=<путь к ffmpeg>/include -DLIBAVUTIL_INCLUDE_DIR=<путь к ffmpeg>/include -DLIBSWRESAMPLE_INCLUDE_DIR=<путь к ffmpeg>/include -DLIBSWSCALE_INCLUDE_DIR=<путь к ffmpeg>/include
```

Или то же самое через cmake-gui

* Сборка

```
cmake --build . --config Release
```

Исполняемый файл находится в подкаталоге src/Release

Для запуска потребуется **скопировать библотеки**

ffmpeg:
- avcodec-58.dll
- avdevice-58.dll
- avfilter-7.dll
- avformat-58.dll
- avutil-56.dll
- postproc-55.dll
- swresample-3.dll
- swscale-5.dll

SDL2:
- SDL2.dll (появится при сборке в каталоге `libs\dependencies\install\bin`)

В один каталог с исполняемым файлом


# Использование

## Воспроизведение

```
pcm_coder <входной файл>
```

## Вопроизведение на Raspberry PI
1. Используйте [инструкцию](https://www.raspberrypi.org/documentation/configuration/config-txt/video.md) Чтобы включить
    композитный выход
2. Убедитесь, что композит рботает

```
$ tvservice -s
state 0x80000 [PAL 4:3], 720x576 @ 50.00Hz, interlaced
```

3. Зпустите воспроизведение командой
```
$ pcm_coder -R --crop-top X <входной файл>
```

Где:
    `-R` - Активирует режим воспроизведения на Raspberry PI
    `--crop-top X` - Сдвигает изображение "вверх" на указанное количество строк. Экспериментируёте с этим значением
    начиная с 0, чтобы дабиться устойчивого воспроизведения на аппаратном декодере PCM

### EXPERT:
В состеве пакета также собирается утилита rpi-fb-shifter позволяющая аналогично утилете tvct из пакета [raspi-teletext](https://github.com/ali1234/raspi-teletext)
Сдвигать фреймбуфер относительно строк видеосигнала. (запус должен требует прав root)

```
# rpi-fb-shifter -s <сдвиг> <on|off>
```

`сдвиг` - задает смещеине фреймбуфера **+** - вверх, **-** - вниз.
Чтобы отключить режим сдвига используйте off, однако следует указать ранее установленное значение `-s`, иначе отменить
сдвиг невозможно без перезагрузки.

## Кодирование

```
pcm_coder -c <название кодека ffmpeg> -b <битрейт видео> {--pal|--ntsc} <входной файл> <выходной файл.avi>
```

Остальные опции кодирования смотри в справке `--help`

## TODO

* [v] Поддержка 16 бит (--16)
* [v] Поддержка обрезки (--Cut) - Заменено на произвольную обрезку сверху и снизу --crop-*
* [v] Поддержка вывода на Raspberry PI 1+
