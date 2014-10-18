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
			Write-Host "Running mPhi = $bosonMass and mVpion = $vpionMass"
			& ".\Release\$e.exe" -b $bosonMass -v $vpionMass | Out-File "$e-b$bosonMass-m$vpionMass.txt"
		}
	}
}
