#!/usr/bin/env bash
# Lanza mem_map en background, espera a que arranque, captura
# /proc/PID/maps y pmap -x, y termina el proceso. Mantiene stdin
# abierto con `sleep` para que getchar() no reciba EOF inmediato.
set -u
cd "$(dirname "$0")/.."
(sleep 10; echo) | ./mem_map > /tmp/mm_out.txt &
PID=$(pgrep -n mem_map)
sleep 0.3
# Si pgrep no encontró nada, reintentar tras una pausa adicional.
if [ -z "$PID" ]; then sleep 0.5; PID=$(pgrep -n mem_map); fi
echo "=== Salida de mem_map ==="
cat /tmp/mm_out.txt
echo ""
echo "=== PID=$PID ==="
echo "=== /proc/$PID/maps ==="
cat /proc/$PID/maps
echo ""
echo "=== pmap -x $PID ==="
pmap -x $PID
kill $PID 2>/dev/null
wait 2>/dev/null
rm -f /tmp/mm_out.txt
