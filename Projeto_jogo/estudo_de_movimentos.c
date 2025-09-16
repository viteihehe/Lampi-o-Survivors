#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>

#define LARGURA 640
#define ALTURA 480

enum TECLAS
{
    CIMA,
    BAIXO,
    ESQUERDA,
    DIREITA,
};

int main()
{
    int pos_x = LARGURA / 2;
    int pos_y = ALTURA / 2;

    bool fim = false;
    bool teclas[] = {false, false, false, false};

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_FONT *font20 = NULL;
    ALLEGRO_FONT *font15 = NULL;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

    if (!al_init())
    {
        al_show_native_message_box(NULL, "ERRO", "AVISO:", "FALHA AO INICIAR ALLEGRO", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    display = al_create_display(LARGURA, ALTURA);
    if (!display)
    {
        al_show_native_message_box(NULL, "ERRO", "AVISO:", "FALHA AO CRIAR JANELA", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    font20 = al_load_ttf_font("C:\\Users\\victo\\OneDrive\\Desktop\\Geral\\Pastas\\Spritesjogo\\FiftiesMovies.ttf", 20, NULL);
    font15 = al_load_ttf_font("C:\\Users\\victo\\OneDrive\\Desktop\\Geral\\Pastas\\Spritesjogo\\FiftiesMovies.ttf", 15, NULL);

    if (!font20 || !font15)
    {
        al_show_native_message_box(NULL, "ERRO", "AVISO:", "AS FONTES NAO PUDERAM SER CARREGADAS!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_display(display);
        return -1;
    }

    fila_eventos = al_create_event_queue();

    al_install_keyboard();

    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(display));

    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_draw_text(font20, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTER, "OLHA ESSA GEOMETRIA!");
    al_draw_text(font15, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA / 2 + 30, ALLEGRO_ALIGN_CENTER, "PRECIONE QUALQUER TECLA PARA CONTINUAR");
    al_flip_display();

    ALLEGRO_EVENT ev_start;
    al_wait_for_event(fila_eventos, &ev_start);

    while (!fim)
    {
        ALLEGRO_EVENT ev;

        al_wait_for_event(fila_eventos, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                fim = true;
            }

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                teclas[CIMA] = true;
                break;
            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = true;
                break;
            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = true;
                break;
            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = true;
                break;
            default:
                break;
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                teclas[CIMA] = false;
                break;
            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = false;
                break;
            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = false;
                break;
            default:
                break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            fim = true;
        }

        pos_x += teclas[DIREITA] * 10;
        pos_x -= teclas[ESQUERDA] * 10;
        pos_y -= teclas[CIMA] * 10;
        pos_y += teclas[BAIXO] * 10;

        // desenho do quadrado
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_filled_rectangle(pos_x, pos_y, pos_x + 30, pos_y + 30, al_map_rgb(255, 0, 0));

        al_flip_display();
    }

    al_destroy_display(display);
    al_destroy_event_queue(fila_eventos);
    al_destroy_font(font20);
    al_destroy_font(font15);

    return 0;
}