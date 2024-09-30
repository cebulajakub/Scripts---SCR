if [ "$#" -ne 1 ]; then
    echo "Zła ilość argumentów"
    exit 1
fi

while true; do 
    pid=$1

    echo "1. SIGFPE"
    echo "2. SIGWINCH"
    echo "3. SIGUSR1"
    echo "4. Wylacz pilot"

    read sygnal

    if [ "$sygnal" -eq 1 ]; then
        echo "Signal SIGFPE"
        kill -8 "$pid"
    elif [ "$sygnal" -eq 2 ]; then
        echo "Sygnał SIGWINCH"
        kill -28 "$pid"
    elif [ "$sygnal" -eq 3 ]; then
        echo "Sygnał SIGUSR1"
        kill -10 "$pid"
    elif [ "$sygnal" -eq 4 ]; then
        echo "Wyłączanie pilota"
        exit 0
    else
        echo "Nieprawidłowy wybór sygnału"
    fi
done
