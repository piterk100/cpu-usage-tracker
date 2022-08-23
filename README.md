# Program reads data from /proc/stat and calculates CPU usage.

## Threads

1. Reader - reads data from /proc/stat and sends them to the second thread.
2. Analyzer - calculates usage of every core and sends it to the third thread.
3. Printer - displays data in the console every second.
4. Watchdog - makes sure other threads work properly - it ends program if other threads did not send information for 2 seconds.
