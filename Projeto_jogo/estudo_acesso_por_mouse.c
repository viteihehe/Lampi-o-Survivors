#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>

#define LARGURA 640
#define ALTURA 480

enum BOTOES {
	ESQUERDO,
	DIREITO,
};

int main() {
	int pos_x = LARGURA / 2;
	int pos_y = ALTURA / 2;
	
	bool fim = false;

	bool botoes_mouse[] = { false, false};

	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

	if (!al_init()) {
		al_show_native_message_box(NULL, "ERRO", "AVISO:", "FALHA AO INICIAR ALLEGRO", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	display = al_create_display(LARGURA, ALTURA);
	if (!display) {
		al_show_native_message_box(NULL, "ERRO", "AVISO:", "FALHA AO CRIAR JANELA", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	if (!font20 || !font15) {
		al_show_native_message_box(NULL, "ERRO", "AVISO:", "AS FONTES NAO PUDERAM SER CARREGADAS!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(display);
		return -1;
	}

	fila_eventos = al_create_event_queue();
	
	al_install_mouse();

	al_register_event_source(fila_eventos, al_get_display_event_source(display));
	al_register_event_source(fila_eventos, al_get_mouse_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_hide_mouse_cursor(display);
	while (!fim) {
		ALLEGRO_EVENT ev;

		al_wait_for_event(fila_eventos, &ev);

		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		fim = true;
			}

		if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
			pos_x = ev.mouse.x;
			pos_y = ev.mouse.y;
		}
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			switch (ev.mouse.button) {
				case 1:  
					botoes_mouse[ESQUERDO] = true;
	
					break;
				case 2: 
					botoes_mouse[DIREITO] = true;
					break;
				default:
					break;
			}

		}
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			switch (ev.mouse.button) {
			case 1:
				botoes_mouse[ESQUERDO] = false;

				break;
			case 2:
				botoes_mouse[DIREITO] = false;
				break;
			default:
				break;
			}

		}


		//desenho do quadrado
		if (botoes_mouse[ESQUERDO]) {
			al_draw_filled_rectangle(pos_x, pos_y, pos_x + 30, pos_y + 30, al_map_rgb(255, 0, 0));
		}
		else if (botoes_mouse[DIREITO]) {
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}

		al_flip_display();

	}

	al_destroy_display(display);
	al_destroy_event_queue(fila_eventos);

	return 0;
}