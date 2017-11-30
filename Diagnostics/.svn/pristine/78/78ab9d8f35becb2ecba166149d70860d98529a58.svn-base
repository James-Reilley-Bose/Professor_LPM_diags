set LPM_PATH=%1\..\

REM AStyle script
%LPM_PATH%Tools\AStyle\bin\AStyle.exe --options=%LPM_PATH%Product\Configuration\AStyle_Product.astylerc --recursive %LPM_PATH%*.c %LPM_PATH%*.h

REM Version script
cmd /c "perl %LPM_PATH%\Tools\gen_version.pl < %LPM_PATH%\Product\Configuration\version.h.in > %LPM_PATH%\Product\Configuration\version.h"
