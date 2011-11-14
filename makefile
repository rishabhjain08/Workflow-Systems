all: clean server client
	@echo Done!
server: src/daemon.cpp src/server.cpp src/everything2.cpp
	g++ src/daemon.cpp src/everything2.cpp src/server.cpp -lpthread -o server
client: src/clients.cpp src/client.cpp src/everything2.cpp
	g++ src/clients.cpp src/everything2.cpp src/client.cpp src/gui.cpp -lpthread -lglui -lglut -o client
clean:
	@touch server
	@touch client
	@rm server client
doc:
	pdflatex doc.tex

