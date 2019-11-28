NAME    = visualizer
CFLAGS  += -g -Wall -Werror -Wextra
CFLAGS  += $(shell sdl2-config --cflags)
LDFLAGS += $(shell sdl2-config --libs) -lGL -lGLU -lm

SRC = main.c queue.c
OBJ = $(SRC:.c=.o)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)
