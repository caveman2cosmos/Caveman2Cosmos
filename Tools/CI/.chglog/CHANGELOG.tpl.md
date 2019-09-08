{{ $repourl := .Info.RepositoryURL -}}
# CHANGELOG
{{ range .Versions -}}
{{ if .Commits }}
## {{ .Tag.Name }} - {{ datetime "2006-01-02" .Tag.Date }}
{{ if .CommitGroups -}}
{{ range .CommitGroups -}}
### {{ .Title }}
{{ range .Commits -}}
- {{ if .Scope }}**{{ .Scope }}:** {{ end }}{{ .Subject }}({{ .Author.Name }})
{{- if .Refs }}
{{- range .Refs }}
{{- if .Action }}
    **{{ .Action }} [#{{ .Ref }}]({{ $repourl }}/issues/{{ .Ref }})**
{{- end }}
{{- end }}
{{- end }}
{{ end }}
{{- end }}
{{- end }}
### All Changes
{{ range .Commits -}}
- {{ if .Scope }}**{{ .Scope }}:** {{ .Subject }}{{ else }}{{ .Header }}{{ end }} ({{ .Author.Name }})
{{- if .Refs }}
{{- range .Refs }}
{{- if .Action }}
    **{{ .Action }} [#{{ .Ref }}]({{ $repourl }}/issues/{{ .Ref }})**
{{- end }}
{{- end }}
{{- end }}
{{ end }}
{{- end }}
{{- end }}
