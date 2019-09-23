include makefile.define

#default:all

PATS = ../Common/CTimerEvent.o \
	   ../Common/CTimerNodeList.o \
	   ../Common/ILogicQueue.o \
	   ../Common/ThreadLogic.o \
	   ../Common/XmlOpeation.o \
	   ../TinyXML/tinystr.o \
	   ../TinyXML/tinyxml.o \
	   ../TinyXML/tinyxmlerror.o \
	   ../TinyXML/tinyxmlparser.o \
	   ../TimerMessageFrame.o \
	   ../CTMService.o

LIB_BASE_OBJS = CTimerEvent.o \
				CTimerNodeList.o \
				ILogicQueue.o \
				ThreadLogic.o \
				XmlOpeation.o \
				tinystr.o \
				tinyxml.o \
				tinyxmlerror.o \
				tinyxmlparser.o \
				TimerMessageFrame.o \
				CTMService.o 

APP_NAME = TMService

$(APP_NAME):$(PATS)
	$(CC) -rdynamic -o $(APP_NAME) $(CFLAGS) ${INCLUDES} $(LIB_BASE_OBJS) $(LIBS)
clean:
	rm -rf *.o  $(LIB_BASE_OBJS)
cl:
	rm -rf *.o 
