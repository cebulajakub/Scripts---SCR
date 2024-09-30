
for plik in "$@"; do
    if [ ! -e "$plik" ]; then
        echo "Plik lub katalog '$plik' nie istnieje."
    elif [ -f "$plik" ]; then
        echo "$plik jest plikiem."
    elif [ -d "$plik" ]; then
        if [ -z "$(ls -A "$plik")" ]; then
            echo "$plik jest pustym katalogiem."
        else
            echo "$plik jest katalogiem, który nie jest pusty."
        fi
    fi
done








