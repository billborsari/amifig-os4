CC = gcc
CXX = g++
PREFIX  = 

DATADIR = 
CFLAGS  = -O2 -Wall
CXXFLAGS= $(CFLAGS)
ifeq ($(shell uname -s),AROS)
INCLUDES= -I/development/include/libxml2
LDFLAGS = -lxml2 -lz -s
else
INCLUDES= 
LDFLAGS = -lxml2 -lpthread -lz 
endif

OBJS    = 	svg2fig.o src_colors.o fig_colors.o path2poly.o fig_save.o

TARGET  = svg2fig

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

src_colors.c: colors.gperf
	gperf --output-file=$@ $<
	sed -i -e 's/#ifdef __GNUC_STDC_INLINE__/#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__/' $@
	
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
	
clean:
	rm -f $(OBJS) $(TARGET) *~ src/*~
