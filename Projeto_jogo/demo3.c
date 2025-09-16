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
	ALLEGRO_BITMAP* personagem;
	int colisao;
} Vasco;

typedef struct {
	int movimento;
	int posx;
	int posy;
	bool ativa;
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

	int temp = 0;

	Bala bala[2000];
	int cont = 0;
	bala[0].movimento = NULL;
	bala[0].posx = NULL;
	bala[0].posy = NULL;
	Vasco canga;
	canga.posx = LARGURA / 2;
	canga.posy = ALTURA / 2;

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
			case ALLEGRO_KEY_UP:
				if (cont < 2000) {
					bala[cont].movimento = CIMA;
					bala[cont].posx = canga.posx+30;
					bala[cont].posy = canga.posy+50;
					bala[cont].ativa = false;
					cont++;
				}
				break;
			case ALLEGRO_KEY_DOWN:
				if (cont < 2000) {
					bala[cont].movimento = BAIXO;
					bala[cont].posx = canga.posx+30;
					bala[cont].posy = canga.posy+50;
					bala[cont].ativa = false;
					cont++;
				}
					break;
			case ALLEGRO_KEY_LEFT:
				if (cont < 2000) {
					bala[cont].movimento = ESQUERDA;
					bala[cont].posx = canga.posx+30;
					bala[cont].posy = canga.posy+50;
					bala[cont].ativa = false;
					cont++;
				}
				break;
			case ALLEGRO_KEY_RIGHT:
				if (cont < 2000) {
					bala[cont].movimento = DIREITA;
					bala[cont].posx = canga.posx+30;
					bala[cont].posy = canga.posy+50;
					bala[cont].ativa = false;
					cont++;
				}
				break;
			default:
				break;
			}


		}
		
		
		// A lógica do timer agora está no lugar correto
		else if (ev.type == ALLEGRO_EVENT_TIMER) {
			if (teclas[CIMA]) {
				canga.posy -= 3;
			}
			else if (teclas[BAIXO]) {
				canga.posy += 3;
			}
			else if (teclas[DIREITA]) {
				canga.posx += 3;
			}
			else if (teclas[ESQUERDA]) {
				canga.posx -= 3;
			}

			for (int i = 0; i < cont; i++) {
				if (bala[i].movimento == CIMA) {
					bala[i].posy -= 10;
				} 
				 else if (bala[i].movimento == BAIXO) {
					bala[i].posy += 10;
				}
				 else if (bala[i].movimento == DIREITA) {
					bala[i].posx += 10;
				}
				else if (bala[i].movimento == ESQUERDA) {
					bala[i].posx -= 10;
				}
			}

			// Desenho
			al_draw_bitmap(background, 0, 0, ALLEGRO_ALIGN_CENTRE);
			al_draw_bitmap(canga.personagem, canga.posx, canga.posy, 0);
			al_draw_rectangle(canga.posx+10, canga.posy+10, canga.posx + 55, canga.posy + 55, al_map_rgb(255, 0, 0), 3);
			if (bala[cont].movimento != NULL) {
				for (int i = 0; i < cont; i++) {
					if (!bala[i].ativa && bala[cont].posx >= 1200 || bala[cont].posx <= 80 || bala[cont].posy <= 40 || bala[cont].posy >= 640) {
						al_draw_filled_circle(bala[i].posx, bala[i].posy, 5, al_map_rgb(255, 0, 0));
					}
				}
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
