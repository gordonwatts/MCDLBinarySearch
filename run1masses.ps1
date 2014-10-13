set-alias dl .\Release\BinaryDLSearch.exe
$env:PYTHIA8DATA=".\Release\Pythia8Data"
$env:PATH="$env:PATH;c:\root\bin"

dl -b 100 -m 10 | Out-File b100_m10_log.txt
dl -b 100 -m 25 | Out-File b100_m25_log.txt

dl -b 126 -m 10 | Out-File b126_m10_log.txt
dl -b 126 -m 25 | Out-File b126_m25_log.txt
dl -b 126 -m 40 | Out-File b126_m40_log.txt

dl -b 140 -m 10 | Out-File b140_m10_log.txt
dl -b 140 -m 20 | Out-File b140_m20_log.txt
dl -b 140 -m 40 | Out-File b140_m40_log.txt

dl -b 300 -m 50 | Out-File b300_m50_log.txt

dl -b 600 -m 50 | Out-File b600_m50_log.txt
dl -b 600 -m 150 | Out-File b600_m150_log.txt

dl -b 900 -m 50 | Out-File b900_m50_log.txt
dl -b 900 -m 150 | Out-File b900_m150_log.txt
