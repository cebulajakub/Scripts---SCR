echo "Czy utworzyć nazwę pliku? (tak/nie)"
read odpowiedz

if [ "$odpowiedz" = "tak" ]; then
    touch nazwa_pliku
    echo "Plik został utworzony."
elif [ "$odpowiedz" = "nie" ]; then
    echo "Plik nie został utworzony."
else
    echo "Nieprawidłowa odpowiedź. Prawidłowe odpowiedzi to 'tak' lub 'nie'."
fi

touch nazwa_pliku1
mkdir nazwa_katalogu
echo "nazwa_pliku" > nowa_nazwa
ln -s /home/jakub/nazwa_pliku1 /home/jakub/linkowanie
chmod a+w nazwa_pliku1
