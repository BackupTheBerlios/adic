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
	adicserver \
	adicclient \
	adicbot

# all programs have this headers in common
allcommonh = \
	circle.h \
	line.h \
	typedefs.h \
	vector2d.h \
	genericpointer.h \
	wall.h 

# all programs have this cpp-files in common
allcommoncpp = \
	line.cpp \
	wall.cpp


# all programs have this files in common
allcommon = \
	$(allcommonh) \
	$(allcommoncpp)

polygonh = polygon.h
polygoncpp = polygon.cpp

polygon = $(polygonh) $(polygoncpp)

# needed for the world
worldh = \
	$(allcommonh) \
	$(polygonh) \
	mesh.h \
	world.h

worldcpp = \
	$(allcommoncpp) \
	$(polygoncpp) \
	world.cpp

world = $(worldh) $(worldcpp)

# clients and server have in common
gamecommonh = \
	$(worldh) \
	gameobject.h \
	roundobject.h \
	door.h \
	icon.h \
	playerdata.h \
	player.h \
	game.h \
	uriloader.h \
	version.h \
	messages.h \
	input.h \
	team.h \
	metaserver.h \
	commonconfig.h \
	utils.h

realgamecommoncpp = \
	$(worldcpp) \
	door.cpp \
	playerdata.cpp \
	player.cpp \
	game.cpp \
	team.cpp \
	utils.cpp

if FASTCOMPILE
gamecommoncpp = gamecommon.cpp
gamecommon.cpp :
	for a in $(realgamecommoncpp); do echo "#include \""$$a"\"" >> $@; done
else
gamecommoncpp = $(realgamecommoncpp)
endif

gamecommon = $(gamecommonh) $(gamecommoncpp)

test_line_SOURCES = test-line.cpp $(allcommon)
test_vector2d_SOURCES = test-vector2d.cpp $(allcommon)
test_world_SOURCES = \
	$(world) \
	test-world.cpp

test_world_LDADD = @DOPE_LIBS@

test_poly_SOURCES = test-poly.cpp $(polygon) $(allcommon)

# adicserver
adicserver_SOURCES = \
	$(gamecommon) \
	server.h \
	server.cpp

adicserver_LDADD = @DOPE_LIBS@ @SERVER_LDADD@

# did not work with automake 1.4
# adicclient_CFLAGS = @SDL_CFLAGS@
# adicclient_CXXFLAGS = @SDL_CFLAGS@

clientcommonh = \
	$(gamecommonh) \
	netstream.h

clientcommoncpp = \
	$(gamecommoncpp) \
	netstream.cpp

clientcommon = $(clientcommonh) $(clientcommoncpp)

adicclienth = \
	$(clientcommonh) \
	clientconfig.h \
	client.h \
	guiconfig.h \
	gui.h \
	glfunctions.h \
	minigl.h \
	sdlgl.h \
	sdlglgui.h \
	texture.h \
	animation.h \
	glterminal.h \
	glfont.h \
	soundconfig.h \
	sound.h \
	soundsdlmixer.h \
	sdlsigfactory.h \
	sdlinputdev.h \
	sdlkeyboard.h \
	sdljoystick.h \
	sdlmouse.h \
	sdlinputfield.h \
	sdlmenu.h \
	glpoly.h \
	camera.h

realadicclientcpp = \
	client.cpp \
	gui.cpp \
	sdlgl.cpp \
	sdlglgui.cpp \
	texture.cpp \
	animation.cpp \
	glterminal.cpp \
	glfont.cpp \
	sound.cpp \
	soundsdlmixer.cpp \
	sdlinputdev.cpp \
	sdlkeyboard.cpp \
	sdljoystick.cpp \
	sdlmouse.cpp \
	sdlinputfield.cpp \
	sdlmenu.cpp \
	glpoly.cpp \
	camera.cpp

if FASTCOMPILE
adicclientcpp = $(clientcommoncpp) adicclient.cpp
adicclient.cpp :
	for a in $(realadicclientcpp); do echo "#include \""$$a"\"" >> $@; done
else
adicclientcpp = $(clientcommoncpp) $(realadicclientcpp)
endif


adicclient_SOURCES = $(adicclienth) $(adicclientcpp)

# todo check for SDL_image and SDL_mixer
adicclient_LDADD = @DOPE_LIBS@ @SDL_LIBS@ -lSDL_image -lSDL_mixer @GUICLIENT_LDADD@

adicboth = \
	$(clientcommonh) \
	botclient.h \
	bot.h \
	karmebot.h \
	blackbot.h

realadicbotcpp = \
	botclient.cpp \
	bot.cpp \
	karmebot.cpp \
	blackbot.cpp

if FASTCOMPILE
adicbotcpp = $(clientcommoncpp) adicbot.cpp
adicbot.cpp :
	for a in $(realadicbotcpp); do echo "#include \""$$a"\"" >> $@; done
else
adicbotcpp = $(clientcommoncpp) $(realadicbotcpp)
endif


adicbot_SOURCES = $(adicboth) $(adicbotcpp)

adicbot_LDADD = @DOPE_LIBS@ @BOTCLIENT_LDADD@

# needed to get make dist working with fastcompile
extradist = $(realgamecommoncpp) $(realadicclientcpp) $(realadicbotcpp)

EXTRA_DIST = $(extradist)
