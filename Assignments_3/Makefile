# 제공되는 Makefile은 정확하게 동작하지 않을 수 있으므로
# 환경에 맞게 수정해서 사용하기 바랍니다.


#############################################
CC       = gcc -g
CCOPTS   = -c #-Wall
LINK     = gcc -g

DEPEND  = makedepend

LINKOPTS = -lm
#############################################

SOURCES = ftl.c flashdevicedriver.c main.c

OBJECTS = ftl.o flashdevicedriver.o main.o

.SUFFIXES : .C .c .o

.c.o:
	$(CC) $(CCOPTS) $<

all: blockmapftl

blockmapftl: $(OBJECTS)
	$(LINK) -o blockmapftl $(OBJECTS) $(LINKOPTS)

clean:
	del *.o blockmapftl core

#depend : $(SOURCES)
#	$(DEPEND) $(DEFINES) -I/usr/local/lib/g++-include $(SOURCES)
depend:
	gcc -MM ftl.c flashdevicedriver.c main.c > $(OBJ_DIR)/depend


include depend

# DO NOT DELETE
