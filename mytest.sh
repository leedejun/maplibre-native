#!/usr/bin/env bash
cd /home/fd/lidj/maplibre-native
export LD_LIBRARY_PATH=/home/fd/lidj/maplibre-native/lib:/usr/lib64:$LD_LIBRARY_PATH
./build/bin/mbgl-render --host "192.168.3.252" --port 8000
