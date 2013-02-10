#!/usr/bin/env bash
pid=$1
while ps $pid >/dev/null
do
    ps -o vsz= ${pid}
    sleep 0.001
done | sort -n | tail -n1

