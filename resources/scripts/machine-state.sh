
#!/bin/bash

echo -e "-------------------------------Machine State----------------------------"
echo -e "CPU Usage:\t"`cat /proc/stat | awk '/cpu/{printf("%.2f%%\n"), ($2+$4)*100/($2+$4+$5)}' |  awk '{print $0}' | head -1`
echo -e "Memory Usage:\t"`free | awk '/Mem/{printf("%.2f%%"), $3/$2*100}'`
echo -e "Swap Usage:\t"`free | awk '/Swap/{printf("%.2f%%"), $3/$2*100}'`
paste <(cat /sys/devices/platform/coretemp.0/hwmon/hwmon*/temp*_label) <(cat /sys/devices/platform/coretemp.0/hwmon/hwmon*/temp*_input) | column -s $'\t' -t | sed 's/\(.\)..$/.\1°C/'