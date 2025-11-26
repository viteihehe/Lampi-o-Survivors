#include "jogador.h"
#include <allegro5/allegro_font.h>

#define QUANT_POWERS 4

typedef enum {
    AUMENTO_DANO,
    AUMENTO_VDA,
    AUMENTO_VDM,
    RESTAURAR_VIDA,
} EPowerUps;

void desenhar_powerups(EPowerUps powers[3], ALLEGRO_FONT *fonte);

void aplicar_power(Jogador *canga, EPowerUps power);

void aleatorizar_powers(EPowerUps *powers);