#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************




# mak file fuer unxhpxr
ASM=
AFLAGS=

CDEFS+=-D_PTHREADS -D_STD_NO_NAMESPACE -D_VOS_NO_NAMESPACE -D_UNO_NO_NAMESPACE
CDEFS+=-DSYSV -D_POSIX_PTHREAD_SEMANTICS  -D_HPUX_SOURCE -DRWSTD_MULTI_THREAD -D_REENTRANT -D__HPACC_NOEH -D__STL_NO_EXCEPTIONS
 #CDEFS+= -D__hppa__

# obernervige Warnungen abschalten
# 67  = invalid pragma name
# 251 = An object cannot be deleted using a pointer of type 'void *' since the 
#		type of the object allocated is unknown.
# 370 = The friend declaration is not in the correct form for either a function or a class.
#       (future error)	
# 600 = Type specifier is omitted; "int" is no longer assumed.
DISWARN=+W67,251,370,600

# architecture dependent flags for the C and C++ compiler that can be changed by
# exporting the variable ARCH_FLAGS="..." in the shell, which is used to start build
ARCH_FLAGS*=

#CXX=/opt/aCC/bin/aCC
#CC=/opt/ansic/bin/cc
CXX=g++
CC=gcc
CFLAGSWALL=-Wall
CFLAGS=-nostdinc -c
CFLAGSCC=-pipe -fguiding-decls $(ARCH_FLAGS)
CFLAGSCXX=-pipe -fguiding-decls $(ARCH_FLAGS)
PICSWITCH:=-fPIC
CFLAGSOBJGUIMT=
CFLAGSOBJCUIMT=
CFLAGSSLOGUIMT=$(PICSWITCH)
CFLAGSSLOCUIMT=$(PICSWITCH)
CFLAGSPROF=
CFLAGSDEBUG=-g
CFLAGSDBGUTIL=
CFLAGSOPT=-O2
CFLAGSNOOPT=
CFLAGSOUTOBJ=-o


 #THREADLIB=-L/opt/dce/lib -ldce
# use native aCC to build or link with shared libs, for the sake of
# global class-instances

STATIC		= -Bstatic
DYNAMIC		= -Bdynamic

#LINK=$(SOLARENV)/unxhpgr/lib/gcc-lib/hppa1.1-hp-hpux10.20/egcs-2.90.29/ld
LINK=ld
LINKFLAGS=-L/nw386/dev/s/solenv/unxhpgr/lib/gcc-lib/hppa1.1-hp-hpux10.20/egcs-2.91.57/threads -L/nw386/dev/s/solenv/unxhpgr/lib/gcc-lib/hppa1.1-hp-hpux10.20/egcs-2.91.57
LINKFLAGSAPPGUI= /usr/lib/crt0.o
LINKFLAGSSHLGUI= -b -noinhibit-exec -Bsymbolic -G $(THREADLIB)
LINKFLAGSAPPCUI= /usr/lib/crt0.o
LINKFLAGSSHLCUI= -b -noinhibit-exec -warn-once -Bsymbolic -G
LINKFLAGSTACK=
APPLINKSTATIC=-Bstatic
APPLINKSHARED=-Bsymbolic
APP_LINKTYPE=

# reihenfolge der libs NICHT egal!
STDOBJGUI=
STDSLOGUI=
STDOBJCUI=
STDSLOCUI=
#STDLIBGUIMT= -lX11 -lXext -lcma -lsec -lcxx -ldld -lc_r -lm
STDLIBGUIMT= -lX11 -lXext -ldce
STDLIBCUIMT= -lc_r -lgcc -ldce
#STDSHLGUIMT= -lX11 -lXext -lcma -lsec -lcxx -ldld -lc_r
STDSHLGUIMT= -lcma -lsec -ldld -lc_r
STDSHLCUIMT=
# -L/opt/dce/lib -ldce -lsec -lcxx

LIBMGR=ar
LIBFLAGS=-r
# LIBEXT=.so

IMPLIB=
IMPLIBFLAGS=

MAPSYM=
MAPSYMFLAGS=

RC=
RCFLAGS=
RCLINK=
RCLINKFLAGS=
RCSETVERSION=

DLLPOSTFIX=
DLLPRE=lib
DLLPOST=.sl

LDUMP=c++filt

.IF "$(PRJNAME)"=="uno" || "$(PRJNAME)"=="vos"
#CFLAGS += -D_PTHREADS  -U_REENTRANT -U_POSIX_PTHREAD_SEMANTICS
#CFLAGSSLO += -instances=extern -template=wholeclass -ptr$(SLO)
#CFLAGSOBJ += -instances=global -template=wholeclass -ptr$(SLO)
#LINKFLAGS+= -temp=/tmp -PIC -instances=extern -ptr$(SLO)
.ENDIF

#SHLLINKARCONLY=yes

