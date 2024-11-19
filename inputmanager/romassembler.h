#ifndef ROMASSEMBLER_H
#define ROMASSEMBLER_H

#include <QStringList>

class RomAssembler
{
public:
    RomAssembler();
    ~RomAssembler(); // Destructor for cleanup

private:
    inline static const QStringList romFiles = {
        ":/ROM/ROM/invaders.h",
        ":/ROM/ROM/invaders.g",
        ":/ROM/ROM/invaders.f",
        ":/ROM/ROM/invaders.e"
    };
};

#endif // ROMASSEMBLER_H
