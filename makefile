BINDIR:= out
SRCDIR:= source
INCDIR:= includes
SRC:= $(wildcard $(SRCDIR)/*.c)
OBJ:= $(patsubst $(SRCDIR)/%.c, $(BINDIR)/%.o, $(SRC))
FLAGS:= -I$(INCDIR) -Wall

TARGET:= $(BINDIR)/kudoc

all: $(TARGET)

$(TARGET): $(OBJ)
	gcc -o $@ $^ $(FLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.c
	gcc -c -o $@ $< $(FLAGS)
