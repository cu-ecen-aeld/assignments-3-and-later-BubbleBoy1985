# aesdsocket 
#! /bin/sh
#
#



case "$1" in
  start)
    echo "starting aesdserver"
    start-stop-daemon -S -n -x aesdsocket -a /usr/bin/aesdsocket -- -d
    ;;
  stop)
    echo "stopping aesdserver"
    start-stop-daemon -K -n aesdsocket
    ;;
  *)
    echo "Usage $0 {start | stop}"
  exit 1
esac

exit 0
