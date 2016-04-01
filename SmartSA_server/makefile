all: server

LIBS = `pkg-config --libs opencv`
CFLAGS = `pkg-config --cflags opencv`

server:  orb.o direct_upload.o SmartSA_upload.o server.o
	g++ $(CFLAGS) orb.o direct_upload.o SmartSA_upload.o server.o -o server $(LIBS)

orb.o: orb.cpp basic_header.h orb.h
	gcc $(CFLAGS) -c orb.cpp $(LIBS)
direct_upload.o: direct_upload.cpp basic_header.h orb.h direct_upload.h
	gcc $(CFLAGS) -c direct_upload.cpp $(LIBS)

SmartSA_upload.o: SmartSA_upload.cpp basic_header.h orb.h SmartSA_upload.h
	gcc $(CFLAGS) -c SmartSA_upload.cpp $(LIBS)

server.o: server.cpp basic_header.h orb.h direct_upload.h SmartSA_upload.h
	gcc $(CFLAGS) -c server.cpp $(LIBS)

clean:
	rm -r *.o
