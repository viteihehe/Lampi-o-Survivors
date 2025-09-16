#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define LARGURA 680
#define ALTURA 480

typedef struct {
	int px;
	int py;
} Segmento;

typedef struct {
	int tamanho;
	Segmento* corpo;
} Cobra;

static Cobra cobra;

typedef struct {
	int pos_x;
	int pos_y;
	int raio;
} Fruta;

static Fruta fruta;

enum {
	CIMA,
	BAIXO,
	ESQUERDA,
	DIREITA,
};


int erro(bool teste) {
	if (teste) {
		al_show_native_message_box(NULL, "ERRO", "AVISO:", "HOUVE UM ERRO AO INICIAR", NULL, ALLEGRO_MESSAGEBOX_WARN);
		return -1;
	}
	return 0;
};

bool colisao(Fruta fruta, Cobra cobra);

int main() {
	int pontos = 0;
	int i;
	bool colidir = false;
	cobra.tamanho = 1;
	cobra.corpo = (Segmento*)malloc(cobra.tamanho * sizeof(Segmento));
	cobra.corpo[0].px = LARGURA / 2;
	cobra.corpo[0].py = ALTURA / 2;
	bool fruta_existe = false;
	srand(time(NULL));
	int fps = 10;
	//bool teclas[] = { false, false, false, false };
	int movimento = 5;
	bool fim = false;
	ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_TIMER* timer = NULL;
	ALLEGRO_FONT* font20 = NULL;

	erro(!al_init());

	display = al_create_display(LARGURA, ALTURA);

	erro(!display);

	timer = al_create_timer(1.0 / fps);


	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();


	erro(!timer);

	font20 = al_load_font("C:\\Users\\victo\\OneDrive\\Desktop\\Geral\\Pastas\\Spritesjogo\\FiftiesMovies.ttf", 20, NULL);
	erro(!font20);

	erro(!al_install_keyboard());

	fila_eventos = al_create_event_queue();

	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	al_register_event_source(fila_eventos, al_get_display_event_source(display));
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_text(font20, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA / 2, ALLEGRO_ALIGN_CENTER, "JOGO DA COBRINHA");
	al_draw_text(font20, al_map_rgb(255, 255, 255), LARGURA / 2, ALTURA / 2 + 20, ALLEGRO_ALIGN_CENTER, "APERTE QUALQUER TECLA PARA COMEÇAR");

	al_flip_display();

	ALLEGRO_EVENT ev_start;
	al_wait_for_event(fila_eventos, &ev_start);

	al_start_timer(timer);

	while (!fim) {

		ALLEGRO_EVENT ev;

		al_wait_for_event(fila_eventos, &ev);

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					movimento = CIMA;
					break;
				case ALLEGRO_KEY_DOWN:
					movimento = BAIXO;
					break;
				case ALLEGRO_KEY_RIGHT:
					movimento = DIREITA;
					break;
				case ALLEGRO_KEY_LEFT:
					movimento = ESQUERDA;
					break;
				default:
					break;
			}
		}

		/*if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
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
			}
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			fim = true;
		}

		if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
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
			}
		}*/

		if (ev.type == ALLEGRO_EVENT_TIMER) {

		
			if (!fruta_existe) {
				fruta.pos_x = (rand() % 620);
				fruta.pos_y = (rand() % 460);
				fruta.raio = 20;
				fruta_existe = true;
			}


			for (i = cobra.tamanho - 1; i > 0; i--) {
				cobra.corpo[i] = cobra.corpo[i - 1];
			}

			if (movimento == CIMA) {
				cobra.corpo[0].py -= 30;
			}
			else if (movimento == BAIXO) {
				cobra.corpo[0].py += 30;
			}
			else if (movimento == DIREITA) {
				cobra.corpo[0].px += 30;
			}
			else if (movimento == ESQUERDA) {
				cobra.corpo[0].px -= 30;
			}
			/*
			if (teclas[CIMA]) {
				cobra.corpo[0].py -= 30;
			}
			else if (teclas[BAIXO]) {
				cobra.corpo[0].py += 30;

			}
			else if (teclas[ESQUERDA]) {
				cobra.corpo[0].px -= 30;

			}
			else if (teclas[DIREITA]) {
				cobra.corpo[0].px += 30;
			}
*/

			if (colisao(fruta, cobra)) {
				cobra.tamanho++;
				cobra.corpo = (Segmento*)realloc(cobra.corpo, cobra.tamanho * sizeof(Segmento));
				pontos += 10;
				fruta_existe = false;
			}

			// Desenho
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_filled_circle(fruta.pos_x, fruta.pos_y, fruta.raio, al_map_rgb(255, 0, 0));
			al_draw_textf(font20, al_map_rgb(255,255,255), LARGURA-100, ALTURA -450, ALLEGRO_ALIGN_CENTER, "PONTOS: %d", pontos);
			for (i = 0; i < cobra.tamanho; i++) {
				al_draw_filled_rectangle(cobra.corpo[i].px, cobra.corpo[i].py, cobra.corpo[i].px + 30, cobra.corpo[i].py + 30, al_map_rgb(0, 255, 0));
			}
			al_flip_display();
		}

	}


	al_destroy_display(display);
	al_destroy_font(font20);
	al_destroy_timer(timer);
	al_destroy_event_queue(fila_eventos);
	free(cobra.corpo);


	return 0;
}

bool colisao(Fruta fruta, Cobra cobra) {
	int proximo_x = cobra.corpo[0].px;
	int proximo_y = cobra.corpo[0].py;
	int largura = 30;
	int altura = 30;

	int circ_x = fruta.pos_x;
	int circ_y = fruta.pos_y;
	int circ_raio = fruta.raio;

	float percirc_x = circ_x;
	float percirc_y = circ_y;

	if (percirc_x < proximo_x) {
		percirc_x = proximo_x;
	}
	else if (percirc_x > proximo_x + largura) {
		percirc_x = proximo_x + largura;
	}

	if (percirc_y < proximo_y) {
		percirc_y = proximo_y;
	}
	else if (percirc_y > proximo_y + altura) {
		percirc_y = proximo_y + altura;
	}

	float distan_x = percirc_x - circ_x;
	float distan_y = percirc_y - circ_y;
	float distan_qua = (distan_x * distan_x) + (distan_y * distan_y);

	if (distan_qua <= (circ_raio * circ_raio)) {
		return true;
	}
	return false;
}