INCLUDES = @DOPE_CFLAGS@ @SDL_CFLAGS@ @GL_CFLAGS@

# automatic tests - are build and run by make check
TESTS =

# manual test programs - are build by make check
manualtests = \
	test-line \
	test-vector2d \
	test-world

check_PROGRAMS = \
	$(TESTS) \
	$(manualtests)

bin_PROGRAMS = \
	@PACKAGE@server @PACKAGE@client

# all programs have in common
allcommon = \
	circle.h \
	line.h line.cpp \
	typedefs.h \
	vector2d.h \
	genericpointer.h

# needed for the world
world = \
	$(allcommon) \
	mesh.h \
	wall.h wall.cpp \
	polygon.h polygon.cpp \
	world.h world.cpp

# client and server have in common
gamecommon = \
	$(world) \
	gameobject.h \
	roundobject.h \
	door.h door.cpp \
	icon.h \
	player.h player.cpp \
	game.h game.cpp \
	uriloader.h \
	version.h messages.h

test_line_SOURCES = test-line.cpp $(allcommon)
test_vector2d_SOURCES = test-vector2d.cpp $(allcommon)
test_world_SOURCES = \
	$(world) \
	test-world.cpp

test_world_LDADD = @DOPE_LIBS@

@PACKAGE@server_SOURCES = $(gamecommon) server.h server.cpp
@PACKAGE@server_LDADD = @DOPE_LIBS@

@PACKAGE@client_SOURCES = $(gamecommon) \
	client.h client.cpp \
	guiconfig.h \
	gui.h gui.cpp \
	sdlglgui.h sdlglgui.cpp

@PACKAGE@client_LDADD = @DOPE_LIBS@ @SDL_LIBS@ @GL_LIBS@


