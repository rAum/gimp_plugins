CPPFILES=dyst_dialog.cpp dyst_kernel.cpp atime.cpp

dyst_dialog: ${CPPFILES} atime.hpp
	g++ -o dyst_dialog `pkg-config --cflags --libs gimp-2.0 gimpui-2.0` ${CPPFILES}

install: dyst_dialog
	if   cp -p dyst_dialog ~/.gimp-2.6/plug-ins/  ; \
	then echo Installing to gimp 2.6 done  ; \
	else cp -p dyst_dialog ~/.gimp-2.4/plug-ins/ ; \
	echo Installing to gimp 2.4 done       ; \
	fi
	
clean:
	rm -f *.o dyst_dialog *.linkinfo *~
