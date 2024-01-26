::Msg "%username%" developer: %1, PROJECT_NAME: %2, LIBRARY_OUTPUT_PATH: %3, lib_output: %4

@echo POST_BUILD_PART

@set "$Title=Built mod install"
@set "$Message=Done: "

@echo "working at LIBRARY_OUTPUT_PATH: %3"
@cd "%3"

@echo "remove old renamed dll (to leave rename new): %1.%2.dll"
@if exist "%1.%2.dll" del "%1.%2.dll"
@echo "rename dll: %2.dll"
@if not exist "%2.dll" set "$Message=Can´t found %3/%2.dll to rename dll!"
@if exist "%2.dll" ren "%2.dll" "%1.%2.dll"

@echo "copy dll to game: %1.%2.dll => %4"
@if not exist "%1.%2.dll" set "$Message=Can´t found %3/%1.%2.dll to copy dll!"
if exist "%1.%2.dll" copy "%1.%2.dll" %4

::BEEEp if smth wrong
@if not "%$Message%" == "Done: " rundll32 user32.dll,MessageBeep
::add warn emoji
@if not "%$Message%" == "Done: " set "$Title=⚠️ Failed to %$Title%"

@if "%$Message%" == "Done: " set "$Message=%$Message%%4"

::push notify
@powershell -Command "& {Add-Type -AssemblyName System.Windows.Forms; Add-Type -AssemblyName System.Drawing; $notify = New-Object System.Windows.Forms.NotifyIcon; $notify.Icon = [System.Drawing.SystemIcons]::Information; $notify.Visible = $true; $notify.ShowBalloonTip(0, '%$Title%', '%$Message%', [System.Windows.Forms.ToolTipIcon]::None)}"
@echo [%$Title%]: %$Message%