# Find all the local text files, search for the last good line, and print it...

foreach ($f in $(Get-ChildItem ./*.txt)) {
    $l = Get-Content $f | ? {$_ -match "For decay length"}
    if ($l) {
        $last = $l[-1]
        Write-Host "${f}: $last"
    }
}
