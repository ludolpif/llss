#!/bin/bash -e
i=0
while true
do
	i=$((i+1))
	sed -i -e "s/Hello from mod-template v[0-9]\\+/Hello from mod-template v$i/" hello.c
	make
	#sleep 4
done
