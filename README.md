# PCM Encoder

Генератор PCM видео из звуковых файлов.


# Features

* Открывает любые поддерживаемые FFmpeg звуковые файлы (также видо со звуковой дорожкой)
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
cmake .. -GCMAKE_BUILD_TYPE=Release
```

* Сборка

```
make
```

* Исполняемый файл находится в подкаталоге src


### Windows

