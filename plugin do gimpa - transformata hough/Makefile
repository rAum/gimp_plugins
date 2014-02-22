CPPFILES=hough_plugin.cpp hough_kernel.cpp

hough_plugin: ${CPPFILES}
	 g++ -O2 -o hough_plugin `pkg-config --cflags --libs gimp-2.0`   ${CPPFILES}

install: hough_plugin
	if   cp -p hough_plugin ~/.gimp-2.6/plug-ins/  ; \
	then echo Installing to gimp 2.6 done  ; \
	else cp -p hough_plugin ~/.gimp-2.4/plug-ins/ ; \
	echo Installing to gimp 2.4 done       ; \
	fi
	
clean:
	rm -f *.o hough_plugin *.linkinfo *~
