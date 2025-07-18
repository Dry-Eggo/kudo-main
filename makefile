SRCDIR := source
BINDIR := bin
OUTDIR := $(BINDIR)/toolchain
PROJECT := kudoc
COMPILER := $(OUTDIR)/$(PROJECT)
SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp, $(BINDIR)/%.o, $(SRCFILES))
INCLUDES := includes
HEADERS  := $(wildcard $(INCLUDES)/*.hpp)
FLAGS := -std=c++20 -I$(INCLUDES)

all: $(COMPILER)

$(COMPILER): $(OBJ)
	@mkdir -p $(OUTDIR)
	clang++ -o $@ $^ $(FLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	clang++ -c -o $@ $< $(FLAGS)
