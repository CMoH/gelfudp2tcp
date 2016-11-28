#!/bin/bash

MESSAGE='{"version": "1.1","host":"example.org","short_message":"Gelf test message","level":0,"_type":"test-message"}'
HOST=localhost
PORT=12201

case "$1" in
    "tcp")
        MESSAGE="${MESSAGE}\0"
        NC_OPTS=""
        echo "Sending test message to tcp://${HOST}:${PORT}"
        ;;
#    "ssl")
#        MESSAGE="${MESSAGE}\0"
#        NC_OPTS=""
#        echo "Sending test message to ssl://${HOST}:${PORT}"
#        ;;
    udp)
        NC_OPTS="-u"
        echo "Sending test message to udp://${HOST}:${PORT}"
        ;;
    **)
        echo "Usage: $(basename $0) [udp | tcp]"
        exit 1
        ;;
esac
        
echo -e "${MESSAGE}" | nc ${NC_OPTS} -w 1 ${HOST} ${PORT}


