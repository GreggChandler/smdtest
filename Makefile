##

#MODEL=fast
#MODELEXT=${EPIPHANY_HOME}/bsps/current/${MODEL}.ldf
ESXELIB=~/projects/esxelib
MODELEXT=${ESXELIB}/fastesx.ldf

CFLAGSEPI=	-DESXEPIPHANY						\
		-I ${ESXELIB}						\
		-T ${MODELEXT}						\
		--specs=nosys.specs

CFLAGSHOST=	-I ${EPIPHANY_HOME}/tools/host/include	\
		-I ${ESXELIB}				\
		-L ${EPIPHANY_HOME}/tools/host/lib	\
		-g

targets:			\
		e-smdtest.elf	\
		smdtest		\
		smdcomb

clean:
	rm	*.o  *.elf  *.map  smdtest

#	Epiphany smdtest

e-smdtest.o:	e-smdtest.cpp
	e-gcc	${CFLAGSEPI}						\
		-c  -O3 e-smdtest.cpp

e-smdtest.elf:	e-smdtest.o  ${ESXELIB}/esxecore.a  			\
		${ESXELIB}/fastesx.ldf
	e-gcc	${CFLAGSEPI}						\
		-o e-smdtest.elf					\
		e-smdtest.o						\
		${ESXELIB}/esxecore.a					\
		-le-lib  -lm  -lstdc++  -lsupc++ -lc			\
		-Wl,-M  >e-smdtest.map

#e-smdtest0.elf:	e-smdtest.elf
#	cp  e-smdtest.elf  e-smdtest0.elf

#	Host Test Code

smdtest.o:	smdtest.cpp
	gcc	${CFLAGSHOST}  -c  smdtest.cpp

smdtest:	smdtest.o  ${ESXELIB}/esxehost.a
	gcc	${CFLAGSHOST}  -o smdtest				\
		smdtest.o						\
		${ESXELIB}/esxehost.a					\
		-le-hal  -le-loader  -lstdc++  -lsupc++  -lpthread

#	Host Combine Code

smdcomb:	smdcomb.cpp  ${ESXELIB}/esxehost.a
	gcc	${CFLAGSHOST}						\
		-o smdcomb						\
		smdcomb.cpp						\
		${ESXELIB}/esxehost.a					\
		-lstdc++
