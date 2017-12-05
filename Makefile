CC=gcc
CFLAG=-g
ORACLE_HOME=/u01/oracle/product/11g
INC_PATH=$(ORACLE_HOME)/rdbms/public
LIB_PATH=$(ORACLE_HOME)/lib
RM=rm -f
LIB_LINK=$(LIB_PATH) -lclntsh

SQL_OBJ=query.o procedure.o

TARGET=orac01 orac02 orac03

all:$(TARGET)

%.o:%.c
	$(CC) $(CFLAG) -c $< -I$(INC_PATH)

orac01:orac01.o $(SQL_OBJ)
	$(CC) $(CFLAG) -o $@ orac01.o $(SQL_OBJ) -L$(LIB_LINK)

orac02:orac02.o $(SQL_OBJ)
	$(CC) $(CFLAG) -o $@ orac02.o $(SQL_OBJ) -L$(LIB_LINK)

orac03:orac03.o $(SQL_OBJ)
	$(CC) $(CFLAG) -o $@ orac03.o $(SQL_OBJ) -L$(LIB_LINK)

clean:
	$(RM) $(TARGET) *.o

