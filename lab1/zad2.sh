
if [ "$1" = "-a" ]; then
        ukryte=$(ls -1 -a | wc -l)
        ukryte=$((ukryte-2))
        echo "Wszystkich plikow jest $ukryte"
else
        wszystkie=$(ls -1 | wc -l)
        echo "Liczba plikow nieukrytych jest $wszystkie"
fi


