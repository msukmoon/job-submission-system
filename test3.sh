#!/bin/sh

echo "\nTest 3 START\n"

echo "Starting up the server..."
echo "Testing if  ./server  works..."
echo "./server\n"
./server &

echo "Waiting for 5 seconds to let the server setup a domain socket..."
echo "sleep 5\n"
sleep 5

echo "Starting up the client..."
echo "Testing if  ./client --nice 13 ls  works..."
echo "./client --nice 13 ls\n"
./client --nice 13 ls
if test $? = 0 ; then
	echo "Test SUCCESSFUL\n"
else
	echo "Test FAILED\n"
fi

echo "Killing the server..."
kill $!
echo "The server has been killed successfully!\n"

echo "Test 3 COMPLETED\n"
echo "--------------------------------------------------\n"
