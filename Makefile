CC = g++
LD = g++
TARGET = kankri
SOURCE = IRC.cpp main.cpp
MODULES = $(addsuffix .o,$(patsubst %.cpp,%,$(SOURCE)))
CCFLAGS = -I/usr/local/Cellar/yaml-cpp/0.5.0/include
LDFLAGS = -lyaml-cpp

$(TARGET): $(MODULES)
	$(LD) $(MODULES) $(LDFLAGS) -o $(TARGET)

$(MODULES): %.o: %.cpp
	$(CC) -c $? -o $@ $(CCFLAGS)