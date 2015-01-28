PROJECT = lidarTest

CPPSRCS = main.cpp \
       lidar.cpp \
       uart.cpp \
       Motor.cpp

INCS = 

OBJS = $(notdir $(CPPSRCS:.cpp=.o))

IINC = $(pathsubst %, -I%, $(INCS))

OUTFILE = $(PROJECT)

SRCPATHS = $(sort $(dir $(SRCS)))
VPATH = $(SRCPATHS)

CPPFLAGS = 
LDFLAGS = -lmraa

all : $(OBJS) $(OUTFILE)

%.o : %.cpp
	@echo Compiling $<
	@g++ -c $(CPPFLAGS) $(IINC) $< -o $@

$(OUTFILE) : $(OBJS)
	@echo Linking $@ $(OBJS)
	@g++ $(OBJS) -o $(OUTFILE) $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(OUTFILE)
