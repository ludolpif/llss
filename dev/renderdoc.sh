#!/bin/sh
if [ $# -ne 2 ]; then
	echo "Usage: $0 <llss-git-root-absolutepath> <llss-binary-relativepath-from-git-root>" >&2
	echo "Example: $0 $HOME/git/llss program/x64/Debug/llss" >&2
	exit 1
fi
cat >llss-autostart.cap <<EOT
{
    "rdocCaptureSettings": 1,
    "settings": {
        "autoStart": true,
        "commandLine": "",
        "environment": [
        ],
        "executable": "$1/$2",
        "inject": false,
        "numQueuedFrames": 0,
        "options": {
            "allowFullscreen": true,
            "allowVSync": true,
            "apiValidation": false,
            "captureAllCmdLists": false,
            "captureCallstacks": false,
            "captureCallstacksOnlyDraws": false,
            "debugOutputMute": true,
            "delayForDebugger": 0,
            "hookIntoChildren": false,
            "refAllResources": false,
            "softMemoryLimit": 0,
            "verifyBufferAccess": false
        },
        "queuedFrameCap": 0,
        "workingDir": "$1"
    }
}
EOT
set -x
qrenderdoc llss-autostart.cap
