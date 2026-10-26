param([string]$KeilRoot = 'C:\Keil_v5\C51')
$ErrorActionPreference = 'Stop'
foreach ($testName in @('environment_monitor')) {
    $buildDir = Join-Path $PSScriptRoot $testName
    New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'test.c') -Destination $buildDir -Force
    Copy-Item -LiteralPath (Join-Path $KeilRoot 'LIB\STARTUP.A51') -Destination $buildDir -Force
    Push-Location -LiteralPath $buildDir
    try {
        $compilerArgs = @('test.c', "INCDIR($KeilRoot\INC)", 'OBJECT(test.obj)', 'PRINT(test.lst)')
        if ($testName -eq 'lcd_only') { $compilerArgs += 'DEFINE(LCD_ONLY)' }
        & (Join-Path $KeilRoot 'BIN\C51.EXE') @compilerArgs
        if ($LASTEXITCODE -ne 0) { throw 'C51 errors or warnings.' }
        & (Join-Path $KeilRoot 'BIN\A51.EXE') 'STARTUP.A51' 'OBJECT(STARTUP.OBJ)' 'PRINT(STARTUP.LST)'
        if ($LASTEXITCODE -ne 0) { throw 'Assembly errors or warnings.' }
        & (Join-Path $KeilRoot 'BIN\BL51.EXE') "STARTUP.OBJ,test.obj,$KeilRoot\LIB\C51S.LIB" 'TO' $testName "PRINT($testName.m51)"
        if ($LASTEXITCODE -ne 0) { throw 'Link errors or warnings.' }
        & (Join-Path $KeilRoot 'BIN\OH51.EXE') $testName "HEXFILE($testName.hex)"
        if ($LASTEXITCODE -ne 0) { throw 'HEX conversion failed.' }
    } finally { Pop-Location }
}


