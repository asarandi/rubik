NAME    = rubik
CFLAGS  += -O1 -Wall -Werror -Wextra -I include
LDFLAGS += $(shell sdl2-config --libs) -lm

SRC = $(addprefix src/, hashtable.c main.c queue.c solver.c stack.c utils.c)
OBJ = $(SRC:.c=.o)

src/main.o : CFLAGS += $(shell sdl2-config --cflags)

UNAME	:= $(shell uname -s)

ifeq ($(UNAME),Darwin)
    src/main.o : CFLAGS += -Wno-deprecated-declarations
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
