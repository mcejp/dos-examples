all: keybbios.exe keybintr.exe

clean:
	rm -f *.exe *.o

%.exe: %.c
	wcl -q -bt=dos $<

%-test: %.exe
	dosbox -c "MOUNT C ." -c "C:" -c $<
