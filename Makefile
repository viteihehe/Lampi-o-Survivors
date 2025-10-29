CODIGOS := ${shell find -name *.c}

SAIDA := jogo.out
ALLEGRO := \
	-lallegro \
	-lallegro_main \
	-lallegro_audio \
	-lallegro_dialog \
	-lallegro_ttf \
	-lallegro_image \
	-lallegro_color \
	-lallegro_memfile \
	-lallegro_acodec \
	-lallegro_primitives \
	-lallegro_font
FLAGS := ${ALLEGRO} -g -Wall


linux: ${CODIGOS}
	gcc ${CODIGOS} ${FLAGS} -o jogo.out

	@./${SAIDA}
