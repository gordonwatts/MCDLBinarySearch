Param(
  [double]$beamCM = 13.0,
  [double]$BosonMass = 140,
  [double]$VPionMass = 40,
  [string]$DecayLengthScan = "",
  [double]$DecayLength = 1.5,
  [int]$nEvents = 5000,
  [switch]$BinarySearch,
  [switch]$DecayProducts,
  [switch]$Timing,
  [switch]$Run1Masses,
  [switch]$NoWait
)

# Parse the command line switches
$exe = @()
if ($BinarySearch) {
  $exe += "BinaryDLSearch"
}
if ($Timing) {
  $exe += "Timing"
}
if ($DecayProducts) {
  $exe += "DecayProducts"
}

# Run 1 masses?
$masses = @()
if ($Run1Masses) {
	$masses +=
		@(100, @(10, 25)),
		@(126, @(10, 25, 40)),
		@(140, @(10, 20, 40)),
		@(300, @(50)),
		@(600, @(50, 150)),
		@(900, @(50, 150))
} else {
	$masses += @($BosonMass, @($VPionMass)), @(,@())
}

# Anything special with decay lengths?
$decayLengths = @()
if ($DecayLengthScan -ne "") {
	$dlLoopSpec = $DecayLengthScan -split "-"
	if ($dlLoopSpec.length -ne 3) {
		Write-Host "DecayLengthScan flag must be writting <n>,<low>,<step-size> e.g. 10-1-1 to do 1 to 11 meters"
		return
	}
	$n = [int] $dlLoopSpec[0]
	$ctau = [double] $dlLoopSpec[1]
	$stepSize = [double] $dlLoopSpec[2]
	for ($index = 0; $index -lt $n; $index++) {
		$decayLengths += $ctau
		$ctau += $stepSize
	}
} else {
	$decayLengths += $DecayLength
}

# Configure everything
$env:PYTHIA8DATA=".\Release\Pythia8Data"
$env:PATH="$env:PATH;c:\root\bin"

$jobs = @()
foreach ($e in $exe) {
	foreach ($m in $masses) {
		if ($m[0]) {
			$bosonMass = $m[0]
			foreach ($ctau in $decayLengths) {
				foreach ($vpionMass in $m[1]) {
					$runJob = {
						param ($mPhi, $mVPion, $ctau, $beamCM, $e, $nEvents, $dir)
						[System.Threading.Thread]::CurrentThread.Priority = 'BelowNormal'
						set-location $dir
						Write-Host "Running mPhi = $mPhi and mVpion = $mVPion at sqrt(s) = $beamCM"
						& ".\Release\$e.exe" -b $mPhi -v $mVPion -beam $beamCM -dl $ctau -n $nEvents | Out-File "${e}_mB_${mPhi}_mVP_${mVPion}_ctau_${ctau}_${beamCM}TeV.txt"
					}
					$jobs += Start-Job $runJob -ArgumentList $bosonMass,$vpionMass,$ctau,$beamCM,$e,$nEvents,$(pwd).Path
				}
			}
		}
	}
}

if (! $NoWait) {
	# Wait for them to all finish...
	$jobs | wait-job

	# Getting the information back from the jobs
	$jobs | Receive-Job
	$jobs | remove-job
} else {
	return $jobs
}
