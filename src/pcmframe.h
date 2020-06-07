#ifndef PCMFRAME_H
#define PCMFRAME_H

struct PCMFrame {
  // промежуточный кадр. Содержит в себе PCM-данные как uint16_t
  // [L R L R L R P Q CRC0 CRC1]
};

#endif // PCMFRAME_H
