all: comintr.exe keybbios.exe keybintr.exe

clean:
	rm -f *.exe *.o

%.exe: %.c
	wcl -q -bt=dos $<

comintr-test: comintr.exe
	dosbox -c "serial1=nullmodem port:5000 transparent:1" -c "MOUNT C ." -c "C:" -c $<

%-test: %.exe
	dosbox -c "MOUNT C ." -c "C:" -c $<
