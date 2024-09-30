if [ ! -f "adresy.txt" ]; then
    echo "Plik z adresami (adresy.txt) nie istnieje."
    exit 1
fi

while read adres
do
    echo "Hej! Co u Ciebie?" | mailx -s "Jakub" "$adres"
    if [ $? -eq 0 ]; then
        echo "E-mail wysłany do: $adres"
    else
        echo "Błąd podczas wysyłania e-maila do: $adres"
    fi
done < adresy.txt

