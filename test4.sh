#!/bin/sh

echo "\nTest 4 START\n"

echo "Starting up the server..."
echo "Testing if  ./server --limit 3  works..."
echo "./server --limit 3\n"
./server --limit 3 &

echo "Waiting for 5 seconds to let the server setup a domain socket..."
echo "sleep 5\n"
sleep 5

echo "Starting up the client..."
echo "Testing if  ./client ls  works..."
echo "./client ls\n"
./client ls
if test $? = 0 ; then
	echo "Test SUCCESSFUL\n"
else
	echo "Test FAILED\n"
fi

echo "Killing the server..."
kill $!
echo "The server has been killed successfully!\n"

echo "Test 4 COMPLETED\n"
echo "--------------------------------------------------\n"
