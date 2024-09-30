echo "Numer PID: $$"

trap 'echo "Sygnal zignorowany"' FPE
trap 'echo "WINDOWS CHANGE - redraw"' WINCH
trap 'echo "Przejecie SIGKILL"' KILL

licznik=0

trap '
licznik=$((licznik+1))
if [ "$licznik" -eq 1 ]; then
	echo "Jesli chcesz wylaczyc projektor wyslij ponownie sygnal SIGFPE"
	sleep 10
elif [ "$licznik" -eq 2 ]; then
	echo "Projektor wylaczony"
	exit 0
fi ' USR1


while true; do
echo "Projektor"
sleep 10
done

