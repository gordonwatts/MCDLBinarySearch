set-alias dl .\Release\BinaryDLSearch.exe
$env:PYTHIA8DATA=".\Release\Pythia8Data"
$env:PATH="$env:PATH;c:\root\bin"

dl -b 100 -v 10 | Out-File b100_m10_log.txt
dl -b 100 -v 25 | Out-File b100_m25_log.txt

dl -b 126 -v 10 | Out-File b126_m10_log.txt
dl -b 126 -v 25 | Out-File b126_m25_log.txt
dl -b 126 -v 40 | Out-File b126_m40_log.txt

dl -b 140 -v 10 | Out-File b140_m10_log.txt
dl -b 140 -v 20 | Out-File b140_m20_log.txt
dl -b 140 -v 40 | Out-File b140_m40_log.txt

dl -b 300 -v 50 | Out-File b300_m50_log.txt

dl -b 600 -v 50 | Out-File b600_m50_log.txt
dl -b 600 -v 150 | Out-File b600_m150_log.txt

dl -b 900 -v 50 | Out-File b900_m50_log.txt
dl -b 900 -v 150 | Out-File b900_m150_log.txt
