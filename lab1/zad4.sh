if [ "$#" -ne "4" ]; then
    echo "Zla liczba argumentow"
    exit 1
fi

nazwa_katalogu="$1"
rozszerzenie="$2"
liczba_dni="$3"
nazwa_archiwum="$4"

find "$nazwa_katalogu" -writable -type f -name "*.$rozszerzenie" -mtime -"$liczba_dni" -exec tar -rf "${nazwa_archiwum}.tar" {} +

