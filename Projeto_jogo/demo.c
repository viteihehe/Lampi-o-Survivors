#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define LARGURA 1280
#define ALTURA 720

enum {
	CIMA,
	BAIXO,
	DIREITA,
	ESQUERDA
};

typedef struct {
	int posx;
	int posy;
	int ultimo_movimento;
	ALLEGRO_BITMAP* personagem;
} Vasco;

typedef struct {
	int movimento;
	int inicial;
	int posx;
	int posy;
}Bala;

int teste(bool allegro) {
	if (!allegro) {
		al_show_native_message_box(NULL, "ERRO", "AVISO:", "ALGUM PROBLEMA NA EXECUCAO", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	return 0;
}

int main() {
	bool fim = false;
	bool teclas[] = { false, false, false, false };
	bool setas[] = { false, false, false, false };

	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_TIMER* timer = NULL;
	ALLEGRO_FONT* font = NULL;
	ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

	Bala bala;
	bala.movimento = NULL;
	bala.posx = NULL;
	bala.posy = NULL;
	bala.inicial = 0;
	Vasco canga;
	canga.posx = LARGURA / 2;
	canga.posy = ALTURA / 2;
	canga.ultimo_movimento = 2;

	int fps = 30;

	teste(al_init());

	display = al_create_display(LARGURA, ALTURA);
	teste(display);

	timer = al_create_timer(1.0 / fps);
	teste(timer);

	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	teste(al_init_image_addon());

	canga.personagem = al_load_bitmap("C:\\Users\\victo\\OneDrive\\Desktop\\Geral\\Pastas\\Spritesjogo\\Sprite-0002.png");
	teste(canga.personagem);

	ALLEGRO_BITMAP* background = al_load_bitmap("C:\\Users\\victo\\OneDrive\\Desktop\\Geral\\Pastas\\Spritesjogo\\backgroud.png");

	font = al_load_font("C:\\Users\\victo\\OneDrive\\Desktop\\Geral\\Pastas\\Spritesjogo\\FiftiesMovies.ttf", 20, NULL);
	teste(font);

	teste(al_install_keyboard());

	fila_eventos = al_create_event_queue();
	teste(fila_eventos);

	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	al_register_event_source(fila_eventos, al_get_display_event_source(display));
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_start_timer(timer);

	while (!fim) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(fila_eventos, &ev);

		// Separei a verificação de pressionar a tecla
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_W:
				teclas[CIMA] = true;
				bala.inicial = 0;
				break;
			case ALLEGRO_KEY_S:
				teclas[BAIXO] = true;
				bala.inicial = 2;
				break;
			case ALLEGRO_KEY_D:
				bala.inicial = 1;
				teclas[DIREITA] = true;
				break;
			case ALLEGRO_KEY_A:
				teclas[ESQUERDA] = true;
				bala.inicial = 3;
				break;
			default:
				break;
			}
		}
		// Separei a verificação de soltar a tecla
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
			default:
				break;
			}
		}

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				bala.movimento = CIMA;
				bala.inicial = 0;
				break;
			case ALLEGRO_KEY_DOWN:
				bala.movimento = BAIXO;
				bala.inicial = 1;
				break;
			case ALLEGRO_KEY_RIGHT:
				bala.movimento = DIREITA;
				bala.inicial = 2;
				break;
			case ALLEGRO_KEY_LEFT:
				bala.movimento = ESQUERDA;
				bala.inicial = 3;
				break;
			case ALLEGRO_KEY_K:
				switch (bala.inicial) {
				case 0:
					bala.posy -= 10;
					break;
				case 1:
					bala.posx += 10;
					break;
				case 2:
					bala.posy += 10;
					break;
				case 3:
					bala.posx -= 10;
					break;
				}
			default:
				break;
			}
		}
		
		
		// A lógica do timer agora está no lugar correto
		else if (ev.type == ALLEGRO_EVENT_TIMER) {
			if (teclas[CIMA]) {
				canga.posy -= 5;
			}
			else if (teclas[BAIXO]) {
				canga.posy += 5;
			}
			else if (teclas[DIREITA]) {
				canga.posx += 5;
			}
			else if (teclas[ESQUERDA]) {
				canga.posx -= 5;
			}

			/*if (bala.movimento == DIREITA) {
				bala.posx += 10;
				bala.posy = canga.posy;
			}
			else if (bala.movimento == ESQUERDA) {
				bala.posx -= 10;
				bala.posy = canga.posy;
			}
			else if (bala.movimento == CIMA) {
				bala.posy -= 10;
				bala.posx = canga.posx;
			}
			else if (bala.movimento == BAIXO) {
				bala.posy += 10;
				bala.posx = canga.posx;
			}*/

			

			// Desenho
			al_draw_bitmap(background, 0, 0, ALLEGRO_ALIGN_CENTRE);
			al_draw_bitmap(canga.personagem, canga.posx, canga.posy, 0);
			if (bala.movimento != NULL) {
				al_draw_filled_circle(bala.posx, bala.posy, 50, al_map_rgb(255, 0, 0), 0);
			}
			al_flip_display();
		}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			fim = true;
		}
	}

	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_font(font);
	al_destroy_event_queue(fila_eventos);
	al_destroy_bitmap(canga.personagem);

	return 0;
}