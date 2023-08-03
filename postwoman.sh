ARGC=$#
NAME='http://localhost:'
METHOD=$1
PORT=$2
LOCATION=$3
MAX_AC=3
MIN_AC=2
if [ $ARGC -eq $MIN_AC ]
then
	URL="${NAME}${PORT}/"
elif [ $ARGC -eq $MAX_AC ]
then
	URL="${NAME}${PORT}/${LOCATION}/"
else
	echo "./postwoman <method: (get, post, delete)> <port> <location (optional)>"
	exit 1
fi
echo "----- ${METHOD} : ${URL}------\n"

if [ $METHOD == 'get' ]
then
   curl -X GET $URL
elif [ $METHOD == 'delete' ]
then
   curl -X DELETE $URL
else
	echo "UNVALID METHOD\ntry : get, post or delete"
fi
