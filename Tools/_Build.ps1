$TARGET = $args[0] + "-build"
$CWD = Get-Location
$BUILD_DIR = Join-Path -Path $CWD -ChildPath "..\Build" -Resolve
$TARGET_DIR = Join-Path -Path $BUILD_DIR -ChildPath $args[0]
$DLL_PATH = Join-Path -Path $TARGET_DIR -ChildPath "CvGameCoreDLL.dll"
$PDB_PATH = Join-Path -Path $TARGET_DIR -ChildPath "CvGameCoreDLL.pdb"
$DEPLOY_DIR = Join-Path -Path $CWD -ChildPath "..\Assets" -Resolve
$FBUILD = Join-Path -Path $CWD -ChildPath "..\Tools\FBuild.exe" -Resolve
$FDB_PATH = Join-Path -Path $TARGET_DIR -ChildPath "CvGameCoreDLL.fdb"
$FBUILD_ARGS = ("-summary", "-wait", "-wrapper", "-graphdb", "$FDB_PATH", "-continueafterdbmove")
# $FBUILD_ARGS = ("-summary", "-wait", "-wrapper", "-cache", "-showcmds", "-verbose", "-showdeps")

# "Target = $TARGET"
# "FBUILD_ARGS = $FBUILD_ARGS"
# "CWD = $CWD"
# "BUILD_DIR = $BUILD_DIR"
# "TARGET_DIR = $TARGET_DIR"
# "DLL_PATH = $DLL_PATH"
# "PDB_PATH = $PDB_PATH"
# "DEPLOY_DIR = $DEPLOY_DIR"
# "FBUILD = $FBUILD"

For ($i = 1; $i -le ($args.Count - 1); $i++) {
    switch ($args[$i]) {
        "build" {
            Write-Host "Building $TARGET ..."
            & "$FBUILD" $FBUILD_ARGS $TARGET
            If ($LASTEXITCODE -ne 0) {
                exit $LASTEXITCODE
            }
        }
        "rebuild" {
            Write-Host "Rebuilding $TARGET ..."
            & "$FBUILD" $FBUILD_ARGS -clean $TARGET
            If ($LASTEXITCODE -ne 0) {
                exit $LASTEXITCODE
            }
        }
        "deploy" {
            Write-Host "Deploying to $DEPLOY_DIR ..."
            & xcopy "$DLL_PATH" "$DEPLOY_DIR" /R /Y
            If ($LASTEXITCODE -ne 0) {
                exit $LASTEXITCODE
            }
            & xcopy "$PDB_PATH" "$DEPLOY_DIR" /R /Y
            If ($LASTEXITCODE -ne 0) {
                exit $LASTEXITCODE
            }
        }
        "clean" {
            Set-Location "$BUILD_DIR"
            If( Test-Path "$TARGET_DIR" ) {
                Write-Host "Cleaning $TARGET_DIR ..."
                Remove-Item -Path "$TARGET_DIR" -Force -Recurse
            }
            # Clear-Content -Path "$TARGET_DIR" -Force -Exclude "C2C (VS2019).log","C2C.Build.CppClean.log"
        }
        Default {}
    }
}
