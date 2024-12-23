#for use with c++ files
.SUFFIXES : .cpp

#Compile with g++
CC = g++

#use -g for gnu debugger and -std= for c++11 compiling
CXXFLAGS = -g -std=c++11

OBJS = main.o
TARGET = newScheduling

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)
	rm -f $(OBJS)  

.cpp.o:
	$(CC) -c $(CXXFLAGS) $(INCDIR) $<

clean:
	rm -f $(OBJS) $(TARGET) core
