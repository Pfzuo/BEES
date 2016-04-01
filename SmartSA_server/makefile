all: server orblib_init

LIBS = `pkg-config --libs opencv`
CFLAGS = `pkg-config --cflags opencv`

server:  orb.o direct_upload.o SmartSA_upload.o server.o
	g++ $(CFLAGS) orb.o direct_upload.o SmartSA_upload.o server.o -o server $(LIBS)

orblib_init: orblib_init.o orb.o
	g++ $(CFLAGS)  orblib_init.o orb.o -o orblib_init $(LIBS)

orb.o: orb.cpp basic_header.h orb.h
	g++ $(CFLAGS) -c orb.cpp $(LIBS)

direct_upload.o: direct_upload.cpp basic_header.h orb.h direct_upload.h
	g++ $(CFLAGS) -c direct_upload.cpp $(LIBS)

SmartSA_upload.o: SmartSA_upload.cpp basic_header.h orb.h SmartSA_upload.h
	g++ $(CFLAGS) -c SmartSA_upload.cpp $(LIBS)

server.o: server.cpp basic_header.h orb.h direct_upload.h SmartSA_upload.h
	g++ $(CFLAGS) -c server.cpp $(LIBS)

orblib_init.o: orblib_init.cpp basic_header.h orb.h
	g++ $(CFLAGS)  -c orblib_init.cpp $(LIBS)


clean:
	rm -r *.o
