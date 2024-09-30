if [ "$#" -ne 1 ]; then
	echo "Zła liczba argumentow"
	exit 1
fi

while true; do

pid=$1

echo "1.Sygnal SIGFPE"
echo "2.Sygnal SIGWINCH"
echo "3.Sygnal SIGUSR"
echo "4.Wyjscie"
echo "5.SIGKILL"

read sygnal

if [ "$sygnal" -eq 1 ]; then
	echo "SIGFPE"
	kill -8 "$pid"
elif [ "$sygnal" -eq 2 ]; then
 	echo "SIGWINCH"
	kill -28 "$pid"
elif [ "$sygnal" -eq 3 ]; then
	echo "SIGUSR1"
	kill -10 "$pid"
elif [ "$sygnal" -eq 4 ]; then
	echo "Wyjście"
	exit 0
elif [ "$sygnal" -eq 5 ]; then
	echo "SIGKILL"
	kill -9 "$pid"
else
	echo "Nieprawidłowy wybor"
fi
done

