NAME    = visualizer
CFLAGS  += -g -Wall -Werror -Wextra
CFLAGS  += $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs) -lm

SRC = ht.c main.c queue.c solver.c
OBJ = $(SRC:.c=.o)

UNAME	:= $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS += -Wno-deprecated-declarations
	LDFLAGS += -framework OpenGL
endif

ifeq ($(UNAME),Linux)
	LDFLAGS += -lGL -lGLU
endif

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)
