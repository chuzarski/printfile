nonlandlock:
	gcc -g -o printfile printfile.c
landlock:
	gcc -o printfile printfile.c -D USE_LANDLOCK=1
install:
	cp ./printfile /bin/printfile
	cp ./printfile /usr/bin/printfile
	chmod +x /bin/printfile
	chmod +x /usr/bin/printfile
clean:
	rm printfile
