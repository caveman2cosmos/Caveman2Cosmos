'Setting build tag on git ...'

&git config --global credential.helper store
Add-Content "$HOME\.git-credentials" "https://$($env:git_access_token):x-oauth-basic@github.com`n"
&git config --global user.email "$($env:git_email)"
&git config --global user.name "$($env:git_user)"

&git tag -a $env:version $env:APPVEYOR_REPO_COMMIT -m "$($env:version)"
&git push --tags
