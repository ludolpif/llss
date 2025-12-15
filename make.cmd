set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
%MSBUILD% app.sln -p:Configuration=Release
%MSBUILD% app.sln -p:Configuration=Debug
%MSBUILD% mod-template.sln -p:Configuration=Release
%MSBUILD% mod-template.sln -p:Configuration=Debug
