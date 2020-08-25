#!/bin/sh

echo "\nTest 5 START\n"

echo "Starting up the server..."
echo "Testing if  ./server --limit 4  works..."
echo "./server --limit 4\n"
./server --limit 4 &

echo "Waiting for 5 seconds to let the server setup a domain socket..."
echo "sleep 5\n"
sleep 5

echo "Starting up the client..."
echo "Testing if  ./client --nice 11 ls -al  works..."
echo "./client --nice 11 ls -al\n"
./client --nice 11 ls -al
if test $? = 0 ; then
	echo "Test SUCCESSFUL\n"
else
	echo "Test FAILED\n"
fi

echo "Killing the server..."
kill $!
echo "The server has been killed successfully!\n"

echo "Test 5 COMPLETED\n"
echo "--------------------------------------------------\n"
