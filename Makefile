ARQUIVO_MAIN := ./codigo/main.c

# ----------
# Nenhum
# ----------

all:
	@echo "Especifique o sistema."
	@echo "Use 'make linux' ou 'make windows'."

# ----------
# Linux
# ----------

LIN_SAIDA := jogo.out
LIN_ALLEGRO := \
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
LIN_FLAGS := ${LIN_ALLEGRO} -g -Wall


linux: ${ARQUIVO_MAIN}
	gcc ${ARQUIVO_MAIN} ${LIN_FLAGS} -c -o main.o
	gcc main.o -o ${LIN_SAIDA} ${LIN_FLAGS}

	@rm *.o
	@./${LIN_SAIDA}

# ----------
# Windows (NÃO TESTADO!)
# ----------

WIN_SAIDA := jogo.exe
WIN_ALLEGRO := C:/allegro/include
WIN_STATIC := liballegro_monolith.dll.a


windows: ${ARQUIVO_MAIN}
	gcc ${ARQUIVO_MAIN} -I ${WIN_ALLEGRO} -c -o main.o
	gcc main.o -I ${WIN_ALLEGRO} -o ${WIN_SAIDA} ${WIN_STATIC}

	${WIN_SAIDA}