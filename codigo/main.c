#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h> // CORREÇÃO: Necessário para snprintf
#include <stdlib.h>
#include <time.h>

#define LARGURA 1280
#define ALTURA 720
#define MAX_BALAS 2000
#define INIMIGO_LARGURA 64
#define INIMIGO_ALTURA 64
#define LIMITE 30

// Caminho absoluto para o seu sistema Linux
#define SPRITES_DIR "./materiais/sprites/"

enum { CIMA, BAIXO, DIREITA, ESQUERDA };

typedef struct {
    int posx;
    int posy;
    ALLEGRO_BITMAP *personagem;
    int colisao;
} Vasco;

typedef struct {
    int posx;
    int posy;
    ALLEGRO_BITMAP *monstro;
    int vida;
    int dano;
    int velocidade;
    int vivo;
} Homem_tatu;

typedef struct {
    float posx;
    float posy;
    ALLEGRO_BITMAP *sprite;
    int vida;
    int dano;
    float velocidade;
    int vivo;
    int ultimo_disparo;

} Formiga;

typedef struct {
    int movimento;
    int posx;
    int posy;
    bool ativa;
} Bala;

// Função para checar a inicialização (retorna 99 em caso de falha para debug)
int teste(bool resultado_init, const char *mensagem) {
    if (!resultado_init) {
        al_show_native_message_box(NULL, "ERRO", "AVISO:", mensagem, NULL,
                                   ALLEGRO_MESSAGEBOX_ERROR);
        return 99; // Retorna 99, evitando o confuso -1/255
    }
    return 0;
}

int main() {
    srand(time(NULL));

    bool fim = false;
    bool teclas[] = {false, false, false, false};

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *font = NULL;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

    // Homem tatu
    Bala bala[MAX_BALAS];
    Homem_tatu homem_tatus[LIMITE];
    ALLEGRO_BITMAP *sprite_tatu = NULL;
    int indice_tatu = 0;
    float ultimo_spawn = 0;
    const float cooldown = 2;

    // //formiga
    Bala formiga_bala[MAX_BALAS];
    Formiga formigas[LIMITE];
    ALLEGRO_BITMAP *sprite_formiga = NULL;
    int indice_formiga = 0;
    float ultimo_spawn_formiga = 0;
    int frame_deslocamento = 0;
    int frame_tiro = 0;
    const float cooldown_formiga = 5;
    const float disparo_cooldown = 2;
    int cont_disparo = 0;

    // para animação
    int contador_frames = 0;
    const int frame_delay = 10;
    const int total_frames_tatu = 2;
    int frame_atual = 0;
    int cont = 0;

    Vasco canga;
    canga.posx = LARGURA / 2;
    canga.posy = ALTURA / 2;
    int velocidade_personagem = 3;
    int velocidade_bala = 10;
    int fps = 60;
    int retorno_erro = 0;
    char tempo[64];

    // --- INICIALIZAÇÕES DO ALLEGRO ---
    if ((retorno_erro = teste(al_init(), "Falha ao inicializar o Allegro")) !=
        0)
        return retorno_erro;

    // Instalações
    if ((retorno_erro =
             teste(al_install_keyboard(), "Falha ao instalar o teclado")) != 0)
        return retorno_erro;
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    if ((retorno_erro = teste(al_init_image_addon(),
                              "Falha ao inicializar o addon de imagem")) != 0)
        return retorno_erro;

    display = al_create_display(LARGURA, ALTURA);
    if ((retorno_erro = teste(display, "Falha ao criar o display")) != 0)
        return retorno_erro;

    timer = al_create_timer(1.0 / fps);
    if ((retorno_erro = teste(timer, "Falha ao criar o timer")) != 0)
        return retorno_erro;

    // --- CARREGAMENTO DE RECURSOS ---

    char path_personagem[256];
    char path_background[256];
    char path_font[256];
    char path_peba[256];
    char path_formiga[256];

    // Constrói os caminhos completos
    snprintf(path_personagem, sizeof(path_personagem), "%s%s", SPRITES_DIR,
             "canga.png");
    snprintf(path_background, sizeof(path_background), "%s%s", SPRITES_DIR,
             "backgroud2.png");
    snprintf(path_font, sizeof(path_font), "%s%s", "./materiais/fontes/",
             "FiftiesMovies.ttf");
    snprintf(path_peba, sizeof(path_peba), "%s%s", SPRITES_DIR, "peba2_1.png");
    snprintf(path_formiga, sizeof(path_formiga), "%s%s", SPRITES_DIR,
             "formiga2.png");

    canga.personagem = al_load_bitmap(path_personagem);
    if ((retorno_erro = teste(canga.personagem,
                              "Falha ao carregar a imagem do personagem. "
                              "Verifique o caminho e as dependências PNG.")) !=
        0)
        return retorno_erro;

    sprite_tatu = al_load_bitmap(path_peba);
    if ((retorno_erro = teste(
             sprite_tatu, "Falha ao carregar o sprite do homem peba.")) != 0)
        return retorno_erro;

    sprite_formiga = al_load_bitmap(path_formiga);
    if ((retorno_erro = teste(sprite_formiga,
                              "Falha ao carregar o sprite da formiga")) != 0)
        return retorno_erro;

    ALLEGRO_BITMAP *background = al_load_bitmap(path_background);
    if ((retorno_erro =
             teste(background, "Falha ao carregar a imagem de fundo.")) != 0)
        return retorno_erro;

    // Flags corrigidas (0 em vez de NULL)
    font = al_load_font(path_font, 20, 0);
    if ((retorno_erro =
             teste(font, "Falha ao carregar a fonte. Verifique o caminho e se "
                         "o Allegro TTF está instalado.")) != 0)
        return retorno_erro;

    fila_eventos = al_create_event_queue();
    if ((retorno_erro =
             teste(fila_eventos, "Falha ao criar a fila de eventos")) != 0)
        return retorno_erro;

    // Registro dos Eventos
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos,
                             al_get_display_event_source(display));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_start_timer(timer);

    // --- LOOP PRINCIPAL DO JOGO ---
    while (!fim) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila_eventos, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            fim = true;
        }

        // Processamento de Teclas DOWN
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_W:
                teclas[CIMA] = true;
                break;
            case ALLEGRO_KEY_S:
                teclas[BAIXO] = true;
                break;
            case ALLEGRO_KEY_D:
                teclas[DIREITA] = true;
                break;
            case ALLEGRO_KEY_A:
                teclas[ESQUERDA] = true;
                break;
            default:
                break;
            }
        }
        // Processamento de Teclas UP (Disparo)
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_W:
                teclas[CIMA] = false;
                break;
            case ALLEGRO_KEY_S:
                teclas[BAIXO] = false;
                break;
            case ALLEGRO_KEY_D:
                teclas[DIREITA] = false;
                break;
            case ALLEGRO_KEY_A:
                teclas[ESQUERDA] = false;
                break;

            case ALLEGRO_KEY_UP:
            case ALLEGRO_KEY_DOWN:
            case ALLEGRO_KEY_LEFT:
            case ALLEGRO_KEY_RIGHT:
                if (cont < MAX_BALAS) {
                    Bala *nova_bala = &bala[cont];

                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                        nova_bala->movimento = CIMA;
                    } else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                        nova_bala->movimento = BAIXO;
                    } else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                        nova_bala->movimento = ESQUERDA;
                    } else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                        nova_bala->movimento = DIREITA;
                    }

                    nova_bala->posx = canga.posx + 30;
                    nova_bala->posy = canga.posy + 50;
                    nova_bala->ativa = true;

                    cont++;
                }
                break;
            default:
                break;
            }
        }

        // Lógica de Atualização (Física e Movimento)
        else if (ev.type == ALLEGRO_EVENT_TIMER) {

            // Timer
            double count = al_get_timer_count(timer);
            double counts = count / 60;
            sprintf(tempo, "Tempo: %.2f", counts);

            // Movimento do Personagem
            if (teclas[CIMA])
                canga.posy -= velocidade_personagem;
            if (teclas[BAIXO])
                canga.posy += velocidade_personagem;
            if (teclas[DIREITA])
                canga.posx += velocidade_personagem;
            if (teclas[ESQUERDA])
                canga.posx -= velocidade_personagem;

            // Movimento das Balas
            for (int i = 0; i < cont; i++) {
                if (bala[i].ativa) {
                    if (bala[i].movimento == CIMA) {
                        bala[i].posy -= velocidade_bala;
                    } else if (bala[i].movimento == BAIXO) {
                        bala[i].posy += velocidade_bala;
                    } else if (bala[i].movimento == DIREITA) {
                        bala[i].posx += velocidade_bala;
                    } else if (bala[i].movimento == ESQUERDA) {
                        bala[i].posx -= velocidade_bala;
                    }

                    // Desativa a bala se sair da tela
                    if (bala[i].posx < 0 || bala[i].posx > LARGURA ||
                        bala[i].posy < 0 || bala[i].posy > ALTURA) {
                        bala[i].ativa = false;
                    }
                }
            }

            // frames
            contador_frames++;

            if (contador_frames >= frame_delay) {
                frame_atual++;
                frame_deslocamento++;
                if (frame_atual >= total_frames_tatu) {
                    frame_atual = 0;
                }
                if (frame_deslocamento >= 2) {
                    frame_deslocamento = 0;
                }
                contador_frames = 0;
            }

            // Homem peba

            if (counts - ultimo_spawn >= cooldown && indice_tatu < LIMITE) {
                homem_tatus[indice_tatu].monstro = sprite_tatu;
                homem_tatus[indice_tatu].velocidade = 1;
                homem_tatus[indice_tatu].vida = 3;
                homem_tatus[indice_tatu].dano = 1;
                homem_tatus[indice_tatu].vivo = 1;

                int spawn = rand() % 3;

                switch (spawn) {
                case 0:
                    homem_tatus[indice_tatu].posx = 80;
                    homem_tatus[indice_tatu].posy = ALTURA / 2;
                    break;
                case 1:
                    homem_tatus[indice_tatu].posx = LARGURA - 80;
                    homem_tatus[indice_tatu].posy = ALTURA / 2;
                    break;
                case 2:
                    homem_tatus[indice_tatu].posx = LARGURA / 2;
                    homem_tatus[indice_tatu].posy = 50;
                    break;
                case 3:
                    homem_tatus[indice_tatu].posx = LARGURA / 2;
                    homem_tatus[indice_tatu].posy = 680;
                    break;

                default:
                    break;
                }

                ultimo_spawn = counts;
                indice_tatu++;
            }

            // Logica tatu
            for (int i = 0; i < indice_tatu; i++) {
                if (homem_tatus[i].posx < canga.posx) {
                    homem_tatus[i].posx += homem_tatus[i].velocidade;
                }
                if (homem_tatus[i].posy < canga.posy) {
                    homem_tatus[i].posy += homem_tatus[i].velocidade;
                }
                if (homem_tatus[i].posx > canga.posx) {
                    homem_tatus[i].posx -= homem_tatus[i].velocidade;
                }
                if (homem_tatus[i].posy > canga.posy) {
                    homem_tatus[i].posy -= homem_tatus[i].velocidade;
                }
            }

            // Formiga

            if (counts - ultimo_spawn_formiga >= cooldown_formiga &&
                indice_formiga < LIMITE) {
                formigas[indice_formiga].dano = 1;
                formigas[indice_formiga].vida = 2;
                formigas[indice_formiga].velocidade = 0.5;
                formigas[indice_formiga].vivo = 1;
                formigas[indice_formiga].sprite = sprite_formiga;
                formigas[indice_formiga].ultimo_disparo = 0;

                int spawn = rand() % 3;

                switch (spawn) {
                case 0:
                    formigas[indice_formiga].posx = 80;
                    formigas[indice_formiga].posy = ALTURA / 2;
                    break;
                case 1:
                    formigas[indice_formiga].posx = LARGURA - 80;
                    formigas[indice_formiga].posy = ALTURA / 2;
                    break;
                case 2:
                    formigas[indice_formiga].posx = LARGURA / 2;
                    formigas[indice_formiga].posy = 50;
                    break;
                case 3:
                    formigas[indice_formiga].posx = LARGURA / 2;
                    formigas[indice_formiga].posy = 680;
                    break;

                default:
                    break;
                }
                ultimo_spawn_formiga = counts;
                indice_formiga++;
            }

            // logica formiga
            for (int i = 0; i < indice_formiga; i++) {
                if (formigas[i].vivo) {
                    if (formigas[i].posx > 10 && formigas[i].posx < 1270) {
                        if (formigas[i].posx >= canga.posx - 150) {
                            formigas[i].posx -= formigas[i].velocidade;
                        } else {
                            formigas[i].posx += formigas[i].velocidade;
                        }
                        if (formigas[i].posx <= canga.posx + 150) {
                            formigas[i].posx += formigas[i].velocidade;
                        } else {
                            formigas[i].posx -= formigas[i].velocidade;
                        }
                        if (formigas[i].posy >= canga.posy - 100) {
                            formigas[i].posy -= formigas[i].velocidade;
                        } else {
                            formigas[i].posy += formigas[i].velocidade;
                        }
                        if (formigas[i].posy <= canga.posy + 100) {
                            formigas[i].posy += formigas[i].velocidade;
                        } else {
                            formigas[i].posy -= formigas[i].velocidade;
                        }
                    }
                    if (counts - formigas[i].ultimo_disparo >=
                            disparo_cooldown &&
                        cont_disparo < MAX_BALAS) {
                        Bala *bala_formiga = &formiga_bala[cont_disparo];
                        bala_formiga->ativa = true;

                        if (canga.posx < formigas[i].posx - 48) {
                            bala_formiga->movimento = ESQUERDA;
                        } else if (canga.posx > formigas[i].posx + 48) {
                            bala_formiga->movimento = DIREITA;
                        } else if (canga.posy > formigas[i].posy - 48) {
                            bala_formiga->movimento = BAIXO;
                        } else if (canga.posy < formigas[i].posy - 48) {
                            bala_formiga->movimento = CIMA;
                        }

                        bala_formiga->posx = formigas[i].posx + 30;
                        bala_formiga->posy = formigas[i].posy + 20;
                        formigas[i].ultimo_disparo = counts;
                        cont_disparo++;
                    }
                }

                // Bala formiga *Terminar a lógica da bala*
                for (int i = 0; i < cont_disparo; i++) {
                    if (formiga_bala[i].ativa) {
                        if (formiga_bala[i].movimento == CIMA) {
                            formiga_bala[i].posy -= velocidade_bala - 7;
                        } else if (formiga_bala[i].movimento == BAIXO) {
                            formiga_bala[i].posy += velocidade_bala - 7;
                        } else if (formiga_bala[i].movimento == DIREITA) {
                            formiga_bala[i].posx += velocidade_bala - 7;
                        } else if (formiga_bala[i].movimento == ESQUERDA) {
                            formiga_bala[i].posx -= velocidade_bala - 7;
                        }

                        if (formiga_bala[i].posx < 0 ||
                            formiga_bala[i].posx > LARGURA ||
                            formiga_bala[i].posy < 0 ||
                            formiga_bala[i].posy > ALTURA) {
                            formiga_bala[i].ativa = false;
                        }
                    }
                }

                for (int i = 0; i < indice_tatu; i++) {
                    if (!homem_tatus[i].vivo)
                        continue;
                    for (int j = 0; j < indice_tatu; j++) {
                        if (!homem_tatus[i].vivo)
                            continue;
                        int colisao_x = INIMIGO_LARGURA;
                        int colisao_y = INIMIGO_ALTURA;
                        if (abs(homem_tatus[i].posx - homem_tatus[j].posx) <=
                                colisao_x &&
                            abs(homem_tatus[i].posy - homem_tatus[j].posy) <=
                                colisao_y) {
                            if (homem_tatus[i].posx < homem_tatus[j].posx) {
                                homem_tatus[i].posx -=
                                    homem_tatus[i].velocidade;
                                homem_tatus[j].posx +=
                                    homem_tatus[j].velocidade;
                            } else {
                                homem_tatus[i].posx +=
                                    homem_tatus[i].velocidade;
                                homem_tatus[j].posx -=
                                    homem_tatus[j].velocidade;
                            }
                            if (homem_tatus[i].posy < homem_tatus[j].posx) {
                                homem_tatus[i].posy -=
                                    homem_tatus[i].velocidade;
                                homem_tatus[j].posy +=
                                    homem_tatus[j].velocidade;
                            } else {
                                homem_tatus[i].posy +=
                                    homem_tatus[i].velocidade;
                                homem_tatus[j].posy -=
                                    homem_tatus[j].velocidade;
                            }
                        }
                    }
                }
                for (int i = 0; i < indice_formiga; i++) {
                    if (!formigas[i].vivo)
                        continue;
                    for (int j = 0; j < indice_formiga; j++) {
                        if (!formigas[j].vivo)
                            continue;
                        int colisao_x = 48;
                        int colisao_y = 48;
                        if (abs(formigas[i].posx - formigas[j].posx) <=
                                colisao_x &&
                            abs(formigas[i].posy - formigas[j].posy) <=
                                colisao_y) {
                            if (formigas[i].posx < formigas[j].posx) {
                                formigas[i].posx -= formigas[i].velocidade;
                                formigas[j].posx += formigas[j].velocidade;
                            } else {
                                formigas[i].posx += formigas[i].velocidade;
                                formigas[j].posx -= formigas[j].velocidade;
                            }
                            if (formigas[i].posy < formigas[j].posx) {
                                formigas[i].posy -= formigas[i].velocidade;
                                formigas[j].posy += formigas[j].velocidade;
                            } else {
                                formigas[i].posy += formigas[i].velocidade;
                                formigas[j].posy -= formigas[j].velocidade;
                            }
                        }
                    }
                }
            }

            // --- DESENHO (RENDERIZAÇÃO) ---
            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (background)
                al_draw_bitmap(background, 0, 0, 0);

            al_draw_bitmap(canga.personagem, canga.posx, canga.posy, 0);

            al_draw_rectangle(canga.posx + 10, canga.posy + 10, canga.posx + 55,
                              canga.posy + 55, al_map_rgb(255, 0, 0), 3);

            // Desenho do timer
            al_draw_text(font, al_map_rgb(0, 0, 0), 1150, 30, 0, tempo);

            // Desenho homem tatu

            for (int i = 0; i < indice_tatu; i++) {
                if (homem_tatus[i].vivo) {
                    int png_x = frame_atual * INIMIGO_LARGURA;
                    int png_y = 0;
                    int flip = 0;

                    if (homem_tatus[i].posx < canga.posx) {
                        flip = ALLEGRO_FLIP_HORIZONTAL;
                    }

                    al_draw_bitmap_region(homem_tatus[i].monstro, png_x, png_y,
                                          INIMIGO_LARGURA, INIMIGO_ALTURA,
                                          homem_tatus[i].posx,
                                          homem_tatus[i].posy, flip);
                }
            }

            // Desenho formiga
            for (int i = 0; i < indice_formiga; i++) {
                if (formigas[i].vivo) {
                    int png_x = frame_deslocamento * 48;
                    int png_y = 0;
                    int flip = 0;

                    if (formigas[i].posx > canga.posx) {
                        flip = ALLEGRO_FLIP_HORIZONTAL;
                    }

                    al_draw_bitmap_region(formigas[i].sprite, png_x, png_y, 48,
                                          48, formigas[i].posx,
                                          formigas[i].posy, flip);
                }
            }
            for (int i = 0; i < cont_disparo; i++) {
                if (formiga_bala[i].ativa) {
                    al_draw_filled_circle(formiga_bala[i].posx,
                                          formiga_bala[i].posy, 5,
                                          al_map_rgb(0, 0, 0));
                }
            }

            al_draw_filled_circle(80, ALTURA / 2, 15, al_map_rgb(255, 0, 0));
            al_draw_filled_circle(LARGURA - 80, ALTURA / 2, 15,
                                  al_map_rgb(255, 0, 0));
            al_draw_filled_circle(LARGURA / 2, 50, 15, al_map_rgb(255, 0, 0));
            al_draw_filled_circle(LARGURA / 2, 680, 15, al_map_rgb(255, 0, 0));

            // Desenha apenas as balas ATIVAS
            for (int i = 0; i < cont; i++) {
                if (bala[i].ativa) {
                    al_draw_filled_circle(bala[i].posx, bala[i].posy, 5,
                                          al_map_rgb(255, 255, 0));
                }
            }

            al_flip_display();
        }
    }

    // --- LIMPEZA ---
    if (canga.personagem)
        al_destroy_bitmap(canga.personagem);
    if (background)
        al_destroy_bitmap(background);
    if (sprite_tatu)
        al_destroy_bitmap(sprite_tatu);

    if (display)
        al_destroy_display(display);
    if (timer)
        al_destroy_timer(timer);
    if (font)
        al_destroy_font(font);
    if (fila_eventos)
        al_destroy_event_queue(fila_eventos);

    return 0;
}