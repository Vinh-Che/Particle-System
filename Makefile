IDIR= ./include
SRCDIR= ./src
IMGUI_DIR=./src/imgui
CC=g++
CFLAGS=-I$(IDIR)

ODIR=obj

LDIR=./lib/unix
LIBS= -L${LDIR} -lAntTweakBar -lglfw3 -lGL -lGLU -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

_IMGUI_DEPS = imconfig.h imgui_impl_glfw.h imgui_impl_opengl3.h imgui_internal.h imgui_stdlib.h imgui.h imstb_rectpack.h imstb_textedit.h imstb_truetype.h
_IM_GUI_OBJ = imgui_demo.o imgui_draw.o imgui_impl_glfw.o imgui_impl_opengl3.o imgui_stdlib.o imgui_widgets.o imgui.o

_DEPS = shader.h camera.h particle.h particle-system.h
_OBJ = main.o glad.o stb_image.o shader.o camera.o particle.o particle-system.o

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ)) $(patsubst %,$(ODIR)/%,$(_IM_GUI_OBJ))
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS)) $(patsubst %,$(IMGUI_DIR)/%,$(_IMGUI_DEPS))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(IMGUI_DIR)/%.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(IMGUI_DIR)/%.cpp $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

basic-particle-system: $(OBJ)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 