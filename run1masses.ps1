Param(
  [switch]$BinarySearch,
  [switch]$Timing
)

$exe = @()
if ($BinarySearch) {
  $exe += "BinaryDLSearch"
}
if ($Timing) {
  $exe += "Timing"
}

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
				param ($mPhi, $mVPion, $e, $dir)
				set-location $dir
				Write-Host "Running mPhi = $mPhi and mVpion = $mVPion"
				Write-Host " -> Running in $(pwd)"
				& ".\Release\$e.exe" -b $mPhi -v $mVPion | Out-File "$e-$mPhi-$mVPion.txt"
			}
			Start-Job $runJob -ArgumentList $bosonMass,$vpionMass,$e,$(pwd).Path
		}
	}
}

# Wait for them to all finish...
get-job | wait-job

# Getting the information back from the jobs
Get-Job | Receive-Job
