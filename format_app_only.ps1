# Format application source only. Skip system/vendor and write-protected files.
$root = $PSScriptRoot
$count = 0
$skipped = 0
$skipNames = @(
    'lpc17xx_', 'GPDMA_LPC17xx', 'GPIO_LPC17xx', 'PIN_LPC17xx', 'SSP_LPC17xx',
    'Board_Joystick', 'Joystick_MCB1700', 'format_app_only', 'format_sources'
)
$files = Get-ChildItem -Path $root -File | Where-Object {
    $n = $_.Name
    ($n -match '\.(c|h)$') -and
    ($n -notmatch 'Conflict') -and
    -not ($skipNames | Where-Object { $n -like "${_}*" })
}
foreach ($f in $files) {
    $path = $f.FullName
    try {
        $content = Get-Content -Path $path -Raw -Encoding UTF8
    } catch {
        Write-Host "Skip (read): $($f.Name)"
        $skipped++
        continue
    }
    if ($null -eq $content) { $content = '' }
    $lines = $content -split "`r?`n"
    $out = [System.Collections.ArrayList]::new()
    $prevEmpty = $false
    foreach ($line in $lines) {
        $line = $line -replace "`t", '    '
        $line = $line.TrimEnd()
        $empty = ($line.Length -eq 0)
        if ($empty) {
            if ($prevEmpty) { continue }
            $prevEmpty = $true
        } else {
            $prevEmpty = $false
        }
        [void]$out.Add($line)
    }
    while ($out.Count -gt 0 -and $out[-1] -eq '') { $out.RemoveAt($out.Count - 1) }
    if ($out.Count -gt 0) { [void]$out.Add('') }
    $newContent = $out -join "`n"
    if ($newContent -ne $content) {
        try {
            Set-Content -Path $path -Value $newContent -NoNewline -Encoding UTF8
            Write-Host "Formatted: $($f.Name)"
            $count++
        } catch {
            Write-Host "Skip (write protected): $($f.Name)"
            $skipped++
        }
    }
}
Write-Host "Done. Formatted $count files. Skipped $skipped."
