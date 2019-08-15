RM:=rm -f
RMDIR:=rmdir
CXXFLAG_O:=-O2 -mavx2 -L$(BOOST_HOME)/lib/release
CXXFLAG_D:=-ggdb3 -mavx2 -L$(BOOST_HOME)/lib/debug
CXX:=g++ -std=c++1z -Wall
#CC:=gcc -Wall
AR:=ar
LD:=ld
DFLAG:=$(PROROOT)/mak/.tmp.mak
DFLAG_DYNAMIC:=$(PROROOT)/mak/.dltmp.mak
SLFLAG:=-ruc
DLFLAG_OBJ:=-fPIC
DLFLAG_TARGET:=-shared
CP:=cp

#各类型文件存放路径
INCLUDEPATH:=-I$(PROROOT)/include -I$(BOOST_HOME)/include \
	          $(SINCLUDEPATH) -I$(OPENCV_HOME)/include -I$(DAHENG_ROOT)/sdk/include -I$(GENICAM_ROOT_V2_3)/library/CPP/include
LIBPATH:=$(PROROOT)/lib
LIBOUTPATH:=$(PROROOT)/lib
BINPATH:=$(PROROOT)/bin
DEPENDPATH:=depend
OBJPATH:=obj
OBJS:=$(addprefix $(OBJPATH)/, $(SOBJS))
LIBS:=$(SLIBS)

#各类型文件后缀
DLFIX:=.so
SLFIX:=.a
OBJFIX:=.o

#可执行程序
EXTTARGET:=$(BINPATH)/$(STARGET)

#动态库
DLTARGET:=$(LIBOUTPATH)/lib$(STARGET)$(DLFIX)

#静态库
SLTARGET:=$(LIBOUTPATH)/lib$(STARGET)$(SLFIX)

debugexec: setdebug $(EXTTARGET)
debugstatic: setdebug $(SLTARGET)
debugdynamic: setdebug setdynamic $(DLTARGET)
releaseexec: setrelease $(EXTTARGET)
releasedynamic: setrelease setdynamic $(DLTARGET)
releasestatic: setrelease $(SLTARGET)

$(EXTTARGET):$(OBJS)
	@echo "[$^ -> $@]"
	$(CXX) $^ `cat $(DFLAG)` -o $@ $(LIBS) $(addprefix -L, $(LIBPATH))

$(DLTARGET):$(OBJS)
	@echo "[$^ -> $@]"
	$(CXX) $^ `cat $(DFLAG)` $(DLFLAG_TARGET) -o $@ $(INCLUDEPATH)


$(SLTARGET):$(OBJS)
	@echo "[$^ -> $@]"
	$(AR) $(SLFLAG) $@ $^


.SUFFIXES:
.SUFFIXES: .c .cpp .pc .sqc .java .class .o .cc

# 隐式推断
#$(BINPATH)/%:$(OBJPATH)/$(OBJS)
#	@echo "[$^ -> $@]"
#	$(CXX) $< `cat $(DFLAG)` -o $@ $(LIBS) $(addprefix -L, $(LIBPATH))

$(OBJPATH)/%.o: %.cpp $(DEPENDPATH)/%.d
	@echo "[$^ -> $@]"
	$(CXX) -c $< `cat $(DFLAG)` `cat $(DFLAG_DYNAMIC)` -o $@ $(INCLUDEPATH)

$(OBJPATH)/%.o: %.cc $(DEPENDPATH)/%.d
	@echo "[$^ -> $@]"
	$(CXX) -c $< `cat $(DFLAG)` `cat $(DFLAG_DYNAMIC)` -o $@ $(INCLUDEPATH)

$(DEPENDPATH)/%.d:%.cc
	@echo "[$^ -> $@]"
	$(CXX) -MM $< -o $@ $(INCLUDEPATH)
	sed -i 's,\($*\)\.o[ :]*,\1.o $@ :,g' $@

$(DEPENDPATH)/%.d:%.cpp
	@echo "[$^ -> $@]"
	$(CXX) -MM $< -o $@ $(INCLUDEPATH)
	sed -i 's,\($*\)\.o[ :]*,\1.o $@ :,g' $@

sinclude $(addprefix $(DEPENDPATH)/, $(patsubst %.o, %.d, $(SOBJS)))

clean:
	$(RM) $(EXTTARGET)
	$(RM) $(SLTARGET)
	$(RM) $(DLTARGET)
	$(RM) $(OBJS)
	$(RM) $(addprefix $(DEPENDPATH)/, $(patsubst %.o,%.d,$(SOBJS)))
	$(RMDIR) obj depend

setdebug:
	@echo -n "$(CXXFLAG_D) " > $(DFLAG)
	@echo -n "" > $(DFLAG_DYNAMIC)
#@echo "OBJS:$(OBJS)"
#@echo "TARGET:$(EXTTARGET)"
#@echo "SLTARGET:$(SLTARGET)"
#@echo "DLTARGET:$(DLTARGET)"
#@echo "LIBS:$(LIBS)"
#@echo "INCLUDEPATH:$(INCLUDEPATH)"
#echo "$*"

setrelease:
	@echo -n "$(CXXFLAG_O) " > $(DFLAG)
	@echo -n "" > $(DFLAG_DYNAMIC)

setdynamic:
	@echo -n "$(DLFLAG_OBJ) " > $(DFLAG_DYNAMIC)
