#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <allegro5/display.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/keycodes.h>
#include <allegro5/mouse.h>
#include <allegro5/system.h>
#include <allegro5/timer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define VELOCIDADE 3
#define LARGURA 1280
#define ALTURA 720
#define FPS 60

typedef struct {
    bool cima;
    bool baixo;
    bool esq;
    bool dir;
} MapaDirecoes;

typedef struct {
    ALLEGRO_BITMAP *sprite;
    int x;
    int y;
    MapaDirecoes movimento;
    MapaDirecoes mira;
} Jogador;

typedef struct {
    ALLEGRO_BITMAP *sprite;
    int x;
    int y;
    MapaDirecoes direcoes;
} Bala;

void capturar_movimento(ALLEGRO_EVENT evento, MapaDirecoes *teclas) {
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_W:
            teclas->cima = true;
            break;

        case ALLEGRO_KEY_S:
            teclas->baixo = true;
            break;

        case ALLEGRO_KEY_A:
            teclas->esq = true;
            break;

        case ALLEGRO_KEY_D:
            teclas->dir = true;
            break;
        }
    }

    if (evento.type == ALLEGRO_EVENT_KEY_UP) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_W:
            teclas->cima = false;
            break;

        case ALLEGRO_KEY_S:
            teclas->baixo = false;
            break;

        case ALLEGRO_KEY_A:
            teclas->esq = false;
            break;

        case ALLEGRO_KEY_D:
            teclas->dir = false;
            break;
        }
    }
}

void capturar_mira(ALLEGRO_EVENT evento, MapaDirecoes *teclas) {
    if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_UP:
            teclas->cima = true;
            break;

        case ALLEGRO_KEY_DOWN:
            teclas->baixo = true;
            break;

        case ALLEGRO_KEY_LEFT:
            teclas->esq = true;
            break;

        case ALLEGRO_KEY_RIGHT:
            teclas->dir = true;
            break;
        }
    }

    if (evento.type == ALLEGRO_EVENT_KEY_UP) {
        switch (evento.keyboard.keycode) {
        case ALLEGRO_KEY_UP:
            teclas->cima = false;
            break;

        case ALLEGRO_KEY_DOWN:
            teclas->baixo = false;
            break;

        case ALLEGRO_KEY_LEFT:
            teclas->esq = false;
            break;

        case ALLEGRO_KEY_RIGHT:
            teclas->dir = false;
            break;
        }
    }
}

void efetuar_movimento(MapaDirecoes teclas, Jogador *jogador) {
    if (teclas.cima && jogador->y > 0) {
        jogador->y -= VELOCIDADE;
    }

    if (teclas.baixo && jogador->y < ALTURA) {
        jogador->y += VELOCIDADE;
    }

    if (teclas.esq && jogador->x > 0) {
        jogador->x -= VELOCIDADE;
    }

    if (teclas.dir && jogador->x < LARGURA) {
        jogador->x += VELOCIDADE;
    }
}

void gerar_bala(ALLEGRO_EVENT evento, Bala **balas, int *dest_quant,
                Jogador jogador) {
    MapaDirecoes direcoes;
    capturar_mira(evento, &direcoes);

    Bala bala_temp = {al_load_bitmap("./materiais/sprites/bala.png"), jogador.x,
                      jogador.y, direcoes};

    (*dest_quant)++;

    *balas = realloc(*balas, sizeof(Bala) * *dest_quant);
    (*balas)[*dest_quant - 1] = bala_temp;
}

#define VELOCIDADE_BALA 10;
void mover_balas(Bala *balas, int quant_balas) {
    for (int i = 0; i < quant_balas; i++) {
        if (balas[i].direcoes.cima) {
            balas[i].y -= VELOCIDADE_BALA;
        }

        if (balas[i].direcoes.baixo) {
            balas[i].y += VELOCIDADE_BALA;
        }

        if (balas[i].direcoes.esq) {
            balas[i].x -= VELOCIDADE_BALA;
        }

        if (balas[i].direcoes.dir) {
            balas[i].x += VELOCIDADE_BALA;
        }

        al_draw_bitmap(balas[i].sprite, balas[i].x - 8, balas[i].y - 8,
                       ALLEGRO_FLIP_HORIZONTAL);
    }
}

int main() {
    // ----------
    // Inicialização
    // ----------
    al_init();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_native_dialog_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY *tela = al_create_display(LARGURA, ALTURA);
    ALLEGRO_EVENT_QUEUE *fila = al_create_event_queue();
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_display_event_source(tela));

    ALLEGRO_TIMER *tick_timer = al_create_timer(1.0 / FPS);
    al_register_event_source(fila, al_get_timer_event_source(tick_timer));
    al_start_timer(tick_timer);

    // ----------
    // Entidades
    // ----------
    Jogador canga = {al_load_bitmap("./materiais/sprites/canga.png"),
                     50,
                     50,
                     {false, false, false, false}};

    int quant_balas = 0;
    Bala *balas = NULL;

    ALLEGRO_BITMAP *cenario =
        al_load_bitmap("./materiais/sprites/background.png");

    // ----------
    // Loop Principal
    // ----------
    ALLEGRO_EVENT evento;
    for (;;) {
        al_wait_for_event(fila, &evento);
        capturar_movimento(evento, &canga.movimento);
        capturar_mira(evento, &canga.mira);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_Q) {
                gerar_bala(evento, &balas, &quant_balas, canga);
            }
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            efetuar_movimento(canga.movimento, &canga);

            // ----------
            // Frames
            // ----------
            al_draw_bitmap(cenario, 0, 0, ALLEGRO_FLIP_HORIZONTAL);
            mover_balas(balas, quant_balas);

            al_draw_bitmap(canga.sprite, canga.x - 32, canga.y - 32,
                           ALLEGRO_FLIP_HORIZONTAL);
            al_draw_filled_circle(canga.x, canga.y, 5,
                                  al_map_rgb(100, 100, 255));
            al_flip_display();
        }
    }

    al_destroy_display(tela);
    return 0;
}