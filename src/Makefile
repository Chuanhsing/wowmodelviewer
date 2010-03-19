# basic makefile for UNIX

prefix = /usr/local
exec_prefix = /usr/local/bin
INSTALL = /usr/local/bin
SRC_DIR = .
OBJ_DIR = ./Objs
OUTPUT_DIR = .
TARGET = wowmodelview

C_INCLUDE_DIRS = -I/usr/include -I/usr/local/include -I/root/wxGTK-2.6.2/include -I/root/wxIFM/include -I./libmpq -I./CxImage

LIBS = -lmpq /root/cximage599c/build-linux/libCxImage.a /root/cximage599c/build-linux/libCxImage.a /root/cximage599c/build-linux/libjasper.a

WX_LIBS = /usr/local/lib/libwx_gtk2_qa-2.6.a /usr/local/lib/libwx_gtk2_adv-2.6.a /root/wxIFM/lib/libwx_gtk2_ifm-2.6.a /usr/local/lib/libwx_gtk2_core-2.6.a /usr/local/lib/libwx_base_xml-2.6.a /usr/local/lib/libwx_base_net-2.6.a /usr/local/lib/libwx_base-2.6.a /usr/local/lib/libwx_gtk2_gl-2.6.a

EXTRA_LIBS = -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -lXinerama -lXxf86vm -lpng -ljpeg -ltiff -lexpat -lz -ldl -lm -lGL -lGLU

LIB_DIRS = -L/usr/lib -L/usr/local/lib -L/root/cximage599c/build-linux -L./libmpq -L/root/wxIFM/lib

LIB_INCLUDES = -I/usr/local/lib/wx/include/gtk2-ansi-release-static-2.6 -I/usr/local/include/wx-2.6

C_PREPROC = 
AR = ar
CC = gcc
CXX = g++
CFLAGS = $(shell wx-config --cflags)
DEFINES = -DGTK_NO_CHECK_CASTS -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D_LARGEFILE_SOURCE=1 -D_UNIX
CXXFLAGS = -Wall
# -O2 
LDFLAGS = -L/usr/X11R6/lib

### Variables: ###

SRC_OBJS =  \
	$(OBJ_DIR)/app.o \
	$(OBJ_DIR)/animcontrol.o \
	$(OBJ_DIR)/animexporter.o \
	$(OBJ_DIR)/arrows.o \
	$(OBJ_DIR)/camera.o \
	$(OBJ_DIR)/charcontrol.o \
	$(OBJ_DIR)/database.o \
	$(OBJ_DIR)/dbcfile.o \
	$(OBJ_DIR)/ddslib.o \
	$(OBJ_DIR)/effects.o \
	$(OBJ_DIR)/filecontrol.o \
	$(OBJ_DIR)/globalvars.o \
	$(OBJ_DIR)/imagecontrol.o \
	$(OBJ_DIR)/itemselection.o \
	$(OBJ_DIR)/lightcontrol.o \
	$(OBJ_DIR)/model.o \
	$(OBJ_DIR)/modelbankcontrol.o \
	$(OBJ_DIR)/modelcanvas.o \
	$(OBJ_DIR)/modelcontrol.o \
	$(OBJ_DIR)/modelexport.o \
	$(OBJ_DIR)/modelexportoptions.o \
	$(OBJ_DIR)/modelexport-lwo.o \
	$(OBJ_DIR)/modelexport-x3d.o \
	$(OBJ_DIR)/modelviewer.o \
	$(OBJ_DIR)/mpqlib.o \
	$(OBJ_DIR)/particle.o \
	$(OBJ_DIR)/quantize.o \
	$(OBJ_DIR)/settings.o \
	$(OBJ_DIR)/util.o \
	$(OBJ_DIR)/video.o \
	$(OBJ_DIR)/wmo.o

define build_target
@echo Linking...
@echo $(CXX) $(CXXFLAGS) -o "$(TARGET)" $(SRC_OBJS) $(LDFLAGS)  $(DEFINES)  $(LIB_INCLUDES) $(LIB_DIRS) $(WX_LIBS) $(LIBS) $(EXTRA_LIBS)
@$(CXX) $(CXXFLAGS) -o "$(TARGET)" $(SRC_OBJS) $(LDFLAGS)  $(DEFINES) $(LIB_INCLUDES) $(LIB_DIRS) $(WX_LIBS) $(LIBS) $(EXTRA_LIBS)
@echo Done.
endef

define compile_source
@echo Compiling "$<"...
@$(CC) $(CFLAGS) $(C_PREPROC) $(C_INCLUDE_DIRS) -c "$<" -o "$@"
endef

.PHONY: print_header directories

$(TARGET): print_header directories $(SRC_OBJS)
	$(build_target)

print_header:
	@echo ------- Compiling WoW Model Viewer for Linux GTK+ ------

directories:
	-@if [ ! -d "$(OUTPUT_DIR)" ]; then mkdir "$(OUTPUT_DIR)"; fi
	-@if [ ! -d "$(OBJ_DIR)" ]; then mkdir "$(OBJ_DIR)"; fi

$(OBJ_DIR)/app.o: ./app.cpp
	$(compile_source)
	
$(OBJ_DIR)/animexporter.o: ./AnimExporter.cpp
	$(compile_source)

$(OBJ_DIR)/animcontrol.o: ./animcontrol.cpp
	$(compile_source)

$(OBJ_DIR)/arrows.o: ./arrows.cpp
	$(compile_source)
	
$(OBJ_DIR)/camera.o: ./camera.cpp
	$(compile_source)

$(OBJ_DIR)/charcontrol.o: ./charcontrol.cpp
	$(compile_source)
	
$(OBJ_DIR)/database.o: ./database.cpp
	$(compile_source)
	
$(OBJ_DIR)/dbcfile.o: ./dbcfile.cpp
	$(compile_source)
	
$(OBJ_DIR)/ddslib.o: ./ddslib.cpp
	$(compile_source)

$(OBJ_DIR)/effects.o: ./effects.cpp
	$(compile_source)
	
$(OBJ_DIR)/filecontrol.o: ./filecontrol.cpp
	$(compile_source)
	
$(OBJ_DIR)/globalvars.o: ./globalvars.cpp
	$(compile_source)

$(OBJ_DIR)/imagecontrol.o: ./imagecontrol.cpp
	$(compile_source)
	
$(OBJ_DIR)/itemselection.o: ./itemselection.cpp
	$(compile_source)

$(OBJ_DIR)/lightcontrol.o: ./lightcontrol.cpp
	$(compile_source)

$(OBJ_DIR)/model.o: ./model.cpp
	$(compile_source)

$(OBJ_DIR)/modelbankcontrol.o: ./modelbankcontrol.cpp
	$(compile_source)
	
$(OBJ_DIR)/modelcanvas.o: ./modelcanvas.cpp
	$(compile_source)

$(OBJ_DIR)/modelcontrol.o: ./modelcontrol.cpp
	$(compile_source)

$(OBJ_DIR)/modelexport.o: ./modelexport.cpp
	$(compile_source)

$(OBJ_DIR)/modelexportoptions.o: ./modelexportoptions.cpp
	$(compile_source)

$(OBJ_DIR)/modelexport-lwo.o: ./modelexport-lwo.cpp
	$(compile_source)

$(OBJ_DIR)/modelexport-x3d.o: ./modelexport-x3d.cpp
	$(compile_source)
	
$(OBJ_DIR)/modelviewer.o: ./modelviewer.cpp
	$(compile_source)
	
$(OBJ_DIR)/mpqlib.o: ./mpq_libmpq.cpp
	$(compile_source)

$(OBJ_DIR)/particle.o: ./particle.cpp
	$(compile_source)

$(OBJ_DIR)/quantize.o: ./Quantize.cpp
	$(compile_source)

$(OBJ_DIR)/rendertexture.o: ./RenderTexture.cpp
	$(compile_source)

$(OBJ_DIR)/settings.o: ./settings.cpp
	$(compile_source)

$(OBJ_DIR)/util.o: ./util.cpp
	$(compile_source)

$(OBJ_DIR)/video.o: ./video.cpp
	$(compile_source)

$(OBJ_DIR)/wmo.o: ./wmo.cpp
	$(compile_source)



