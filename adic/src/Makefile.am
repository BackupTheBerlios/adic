INCLUDES = @DOPE_CFLAGS@ -DADIC_DATAPATH=\"$(pkgdatadir):data\" @SDL_CFLAGS@

# automatic tests - are build and run by make check
TESTS =

# manual test programs - are build by make check
manualtests = \
	test-line \
	test-vector2d \
	test-world \
	test-poly

check_PROGRAMS = \
	$(TESTS) \
	$(manualtests)

bin_PROGRAMS = \
	adicserver adicclient adicbot

# all programs have in common
allcommon = \
	circle.h \
	line.h line.cpp \
	typedefs.h \
	vector2d.h \
	genericpointer.h \
	wall.h wall.cpp

polygon = \
	polygon.h polygon.cpp

# needed for the world
world = \
	$(allcommon) \
	$(polygon) \
	mesh.h \
	world.h world.cpp

# client and server have in common
gamecommon = \
	$(world) \
	gameobject.h \
	roundobject.h \
	door.h door.cpp \
	icon.h \
	playerdata.h playerdata.cpp \
	player.h player.cpp \
	game.h game.cpp \
	uriloader.h \
	version.h messages.h \
	input.h \
	team.h team.cpp \
	metaserver.h \
	commonconfig.h \
	utils.h utils.cpp

test_line_SOURCES = test-line.cpp $(allcommon)
test_vector2d_SOURCES = test-vector2d.cpp $(allcommon)
test_world_SOURCES = \
	$(world) \
	test-world.cpp

test_world_LDADD = @DOPE_LIBS@

test_poly_SOURCES = test-poly.cpp $(polygon) $(allcommon)

adicserver_SOURCES = $(gamecommon) server.h server.cpp
adicserver_LDADD = @DOPE_LIBS@

# did not work with automake 1.4
# adicclient_CFLAGS = @SDL_CFLAGS@
# adicclient_CXXFLAGS = @SDL_CFLAGS@

clientcommon = $(gamecommon) \
	netstream.h netstream.cpp

adicclient_SOURCES = $(clientcommon) \
	clientconfig.h \
	client.h client.cpp \
	guiconfig.h \
	gui.h gui.cpp \
	glfunctions.h minigl.h \
	sdlgl.h sdlgl.cpp \
	sdlglgui.h sdlglgui.cpp \
	texture.h texture.cpp \
	animation.h animation.cpp \
	glterminal.h glterminal.cpp \
	glfont.h glfont.cpp \
	soundconfig.h \
	sound.h sound.cpp \
	soundsdlmixer.h soundsdlmixer.cpp \
	sdlsigfactory.h \
	sdlinputdev.h sdlinputdev.cpp \
	sdlkeyboard.h sdlkeyboard.cpp \
	sdljoystick.h sdljoystick.cpp \
	sdlmouse.h sdlmouse.cpp \
	sdlinputfield.h sdlinputfield.cpp \
	sdlmenu.h sdlmenu.cpp

# we do not need @GL_LIBS@ since we now dlopen it
# todo check for SDL_image and SDL_mixer
adicclient_LDADD = @DOPE_LIBS@ @SDL_LIBS@ -lSDL_image -lSDL_mixer

adicbot_SOURCES = $(clientcommon) \
	botclient.h botclient.cpp \
	bot.h bot.cpp \
	karmebot.h karmebot.cpp

adicbot_LDADD = @DOPE_LIBS@



