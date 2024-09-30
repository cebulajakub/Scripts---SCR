LC_ALL=C

dzien=$(date +%u)

if [ $dzien -le 5 ]; then
	echo "Dzis jest dzien roboczy"
else
	echo "Dzis jest weekend"
fi
