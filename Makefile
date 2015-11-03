RESULT_EXE = rctank.elf

CFLAGS +=
CPPFLAGS += -I./include -DBUILD_PLATFORM=1 -DBUILD_MODE=0
CXXFLAGS += -std=c++0x
LDFLAGS += -lwiringPi -lwiringPiDev

C_SOURCES = ${wildcard ./src/*.c}
CXX_SOURCES = ${wildcard ./src/*.cpp}

#OBJECTS_DIR = ./build
C_OBJECTS = ${C_SOURCES:.c=.o}
CXX_OBJECTS = ${CXX_SOURCES:.cpp=.o}
OBJECTS = ${C_OBJECTS} ${CXX_OBJECTS}


all: ${RESULT_EXE}

${RESULT_EXE}: ${OBJECTS}
	${LINK.cc} ${OBJECTS} -o ${RESULT_EXE}


clean:
	@- ${RM} ${RESULT_EXE}
	@- ${RM} ${OBJECTS}

distclean: clean
