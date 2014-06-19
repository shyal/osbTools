# loadPlugin("/home/shyal.beardsley/dev/osbTools/.build/maya2012/osbTools-${major}/osbTools-${major}.so")

major=1.0
patch=4

MAYAVER=2012sp1

BOOSTVER=1.44.0

PLATFORM=el6

CC=/code/devtools/el6/gcc-4.1.2/bin/g++

INCLUDEDIRS=-I/code/tpl/el6/gcc-4.4.6/boost/${BOOSTVER}/include -I/apps/autodesk/maya${MAYAVER}/include 

CFLAGS=	-shared -O3 -DNDEBUG -fno-strict-aliasing -c -fPIC -D_BOOL -DREQUIRE_IOSTREAM -DLINUX ${INCLUDEDIRS}

MAYALIBS=-L/apps/autodesk/maya${MAYAVER}/lib -lOpenMaya -lOpenMayaUI -lOpenMayaAnim

BOOSTLIBS=-L/code/tpl/el6/gcc-${GCCVER}/boost/${BOOSTVER}/lib

LIBS=${BOOSTLIBS} ${MAYALIBS}

WORKDIR=.build/maya${MAYAVER}/osbTools-${major}

BUILDDIR=${WORKDIR}/src

OBJ=${BUILDDIR}/UVReversed.o ${BUILDDIR}/foldedUVFaces.o ${BUILDDIR}/UVSeams.o ${BUILDDIR}/holesBorder.o ${BUILDDIR}/Renaming.o ${BUILDDIR}/UVCommon.o ${BUILDDIR}/overlappingUVShells.o ${BUILDDIR}/pluginMain.o ${BUILDDIR}/intersect.o ${BUILDDIR}/triangleOverlappingTest.o

TARGET=${WORKDIR}/osbTools-${major}.so

${TARGET}: ${OBJ}
	$(CC) -shared -Wl,-soname,osbTools-${major}.so.1 -Wl,-Bdynamic -o ${WORKDIR}/osbTools-${major}.so  ${OBJ} ${LIBS}

${WORKDIR}/%.o: %.cpp
	mkdir -p ${BUILDDIR}
	$(CC) -c -o $@ $< $(CFLAGS)

depend:
	makedepend -p ${WORKDIR}/ src/*.cpp
	rm -f Makefile.bak

clean:
	rm -rfv .build

release: ${TARGET}
	mkdir -p /code/addons/${PLATFORM}/maya${MAYAVER}/plugins/osbTools/
	cp .build/maya${MAYAVER}/osbTools-${major}/osbTools-${major}.so /code/addons/el6/maya${MAYAVER}/plugins/osbTools/osbTools.so.${major}.${patch}
	rm -f /code/addons/${PLATFORM}/maya${MAYAVER}/plugins/osbTools-${major}.so
	ln -s /code/addons/${PLATFORM}/maya${MAYAVER}/plugins/osbTools/osbTools.so.${major}.${patch} /code/addons/el6/maya${MAYAVER}/plugins/osbTools-${major}.so
	cp plugins/*.py /code/addons/${PLATFORM}/maya${MAYAVER}/plugins/
	cp plugins/*.py /code/addons/${PLATFORM}/maya${MAYAVER}/plugins/
	cp scripts/* /code/addons/${PLATFORM}/maya${MAYAVER}/scripts/ -r

local: ${TARGET}
	mkdir -p ${HOME}/devinstall/addons/${PLATFORM}/maya${MAYAVER}/plugins/osbTools/
	cp .build/maya${MAYAVER}/osbTools-${major}/osbTools-${major}.so ${HOME}/devinstall/addons/el6/maya${MAYAVER}/plugins/osbTools/osbTools.so.${major}.${patch}
	rm -f ${HOME}/devinstall/addons/${PLATFORM}/maya${MAYAVER}/plugins/osbTools-${major}.so
	ln -s ${HOME}/devinstall/addons/${PLATFORM}/maya${MAYAVER}/plugins/osbTools/osbTools.so.${major}.${patch} ${HOME}/devinstall/addons/el6/maya${MAYAVER}/plugins/osbTools-${major}.so
	cp plugins/*.py ${HOME}/devinstall/addons/${PLATFORM}/maya${MAYAVER}/plugins/
	cp plugins/*.py ${HOME}/devinstall/addons/${PLATFORM}/maya${MAYAVER}/plugins/
	mkdir -p ${HOME}/devinstall/addons/${PLATFORM}/maya${MAYAVER}/scripts/
	cp scripts/* ${HOME}/devinstall/addons/${PLATFORM}/maya${MAYAVER}/scripts/ -r
	
