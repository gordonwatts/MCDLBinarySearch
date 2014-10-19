Param(
  [double]$beamCM = 13.0,
  [switch]$BinarySearch,
  [switch]$Timing
)

# Parse the command line switches
$exe = @()
if ($BinarySearch) {
  $exe += "BinaryDLSearch"
}
if ($Timing) {
  $exe += "Timing"
}
$beamCM = $beamCM * 1000.0

# Run 1 masses
$masses = @( `
	@(100, @(10, 25)),
	@(126, @(10, 25, 40)),
	@(140, @(10, 20, 40)),
	@(300, @(50)),
	@(600, @(50, 150)),
	@(900, @(50, 150))
	)

# Configure everything
$env:PYTHIA8DATA=".\Release\Pythia8Data"
$env:PATH="$env:PATH;c:\root\bin"

foreach ($e in $exe) {
	foreach ($m in $masses) {
		$bosonMass = $m[0]
		foreach ($vpionMass in $m[1]) {
			$runJob = {
				param ($mPhi, $mVPion, $beamCM, $e, $dir)
				set-location $dir
				Write-Host "Running mPhi = $mPhi and mVpion = $mVPion at sqrt(s) = $beamCM"
				$tev = $beamCM/1000.0
				& ".\Release\$e.exe" -b $mPhi -v $mVPion -beam $beamCM | Out-File "${e}_mB_${mPhi}_mVP_${mVPion}_${tev}TeV.txt"
			}
			Start-Job $runJob -ArgumentList $bosonMass,$vpionMass, $beamCM,$e,$(pwd).Path
		}
	}
}

# Wait for them to all finish...
get-job | wait-job

# Getting the information back from the jobs
Get-Job | Receive-Job
