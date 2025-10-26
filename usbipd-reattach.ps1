# usbipd-reattach.ps1
# Loops and automatically re-attaches all USB devices currently shared via usbipd
# Run it with
# powershell.exe -noprofile -executionpolicy bypass -file .\usbipd-reattach.ps1

# === CONFIGURATION ===
# Change this to your WSL distro or VM name (optional)
$attachToWSL = $true   # set to $false if attaching to remote host instead
$remoteHost = ""       # e.g. "192.168.1.10" (only used if $attachToWSL is $false)
$PollInterval = 10     # Seconds between checks

# === SCRIPT START ===
while ($true) {
    try {
        Write-Host "Checking shared USB devices..." -ForegroundColor Cyan

        # Get usbipd list output
        $usbipdList = usbipd list | Out-String
        $lines = $usbipdList -split "`n"

        # Extract devices explicitly marked as "Shared" (not "Not shared")
        $sharedDevices = @()

        foreach ($line in $lines) {
            # Split on multiple spaces which are the column borders
            $parts = $line -split '\s{2,}' | Where-Object { $_ -ne "" }

            # Expected columns: BUSID  VID:PID  DEVICE  STATE
            if ($parts.Count -ge 4) {
                $busId = $parts[0]
                $state = $parts[3].Trim()
                if ($state -eq "Attached") {
                    Write-Host -ForegroundColor Green $line
                } elseif ($state -eq "Shared") {
                    Write-Host -ForegroundColor Yellow $line
                    $sharedDevices += $busId
                }
            }
        }

        if ($sharedDevices.Count -eq 0) {
            Write-Host "No detached shared USB devices found." -ForegroundColor Yellow
            #exit 0
        } else {

            Write-Host "Found $($sharedDevices.Count) shared devices: $($sharedDevices -join ', ')" -ForegroundColor Green

            # Process each shared device
            foreach ($dev in $sharedDevices) {
                Write-Host "Reattaching $dev ..." -ForegroundColor Cyan
                
                # Detach first (ignore errors)
                usbipd detach --busid $dev 2>$null
                Start-Sleep -Seconds 1

                if ($attachToWSL) {
                    usbipd attach --busid $dev --wsl
                } elseif ($remoteHost -ne "") {
                    usbipd attach --busid $dev --remote $remoteHost
                } else {
                    usbipd attach --busid $dev
                }

                if ($LASTEXITCODE -eq 0) {
                    Write-Host "Successfully attached $dev" -ForegroundColor Green
                } else {
                    Write-Host "Failed to attach $dev" -ForegroundColor Yellow
                }

                Start-Sleep -Seconds 1
            }
            Write-Host "all shared devices processed" -ForegroundColor Cyan
        }
    } catch {
        Write-Warning $_.Exception.Message
    }
    Start-Sleep -Seconds $PollInterval
}


